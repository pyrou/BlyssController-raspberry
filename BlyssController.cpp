/**
  BlyssController - Arduino libary for remote control blyss/liveez components
*/

#include "BlyssController.h"

/* -------------------------------------------------------- */
/* ----                Blyss Spoofer API               ---- */
/* -------------------------------------------------------- */

/* Time constants */
const unsigned long H_TIME = 2400; // Header delay
const unsigned long T_TIME = 400;  // 1/3 frame delay
const byte nb_frames = 13; // Numbers of frames per command

/** "Rolling code" (normally avoid frame spoofing) */
byte RF_ROLLING_CODE[] = {
  0x98, 0xDA, 0x1E, 0xE6, 0x67
};

BlyssController::BlyssController() {
    this->nTransmitterPin = -1;
}

/**
 * Send header over RF
 */
void BlyssController::send_header() {
    digitalWrite(this->nTransmitterPin, HIGH);
  delayMicroseconds(H_TIME);
}

/**
 * Send footer over RF
 */
void BlyssController::send_footer(void) {
    digitalWrite(this->nTransmitterPin, LOW);
  delay(H_TIME * 10 / 1000);
}

/**
 * Send logical "1" over RF
 */
void BlyssController::send_one() {
    digitalWrite(this->nTransmitterPin, LOW);
    delayMicroseconds(T_TIME);
    digitalWrite(this->nTransmitterPin, HIGH);
  delayMicroseconds(T_TIME * 2);
}

/**
 * Send logical "0" over RF
 */
void BlyssController::send_zero() {
    digitalWrite(this->nTransmitterPin, LOW);
    delayMicroseconds(T_TIME * 2);
    digitalWrite(this->nTransmitterPin, HIGH);
  delayMicroseconds(T_TIME);
}

/**
 * Send a bits quarter (4 bits = MSB from 8 bits value) over RF
 *
 * @param data Source data to process and sent
 */
void BlyssController::send_quarter_MSB(byte data) {
  (bitRead(data, 7)) ? this->send_one() : this->send_zero();
  (bitRead(data, 6)) ? this->send_one() : this->send_zero();
  (bitRead(data, 5)) ? this->send_one() : this->send_zero();
  (bitRead(data, 4)) ? this->send_one() : this->send_zero();
}

/**
 * Send a bits quarter (4 bits = LSB from 8 bits value) over RF
 *
 * @param data Source data to process and sent
 */
void BlyssController::send_quarter_LSB(byte data) {
  (bitRead(data, 3)) ? this->send_one() : this->send_zero();
  (bitRead(data, 2)) ? this->send_one() : this->send_zero();
  (bitRead(data, 1)) ? this->send_one() : this->send_zero();
  (bitRead(data, 0)) ? this->send_one() : this->send_zero();
}

/**
 * Generate next valid token for RF transmission
 */
void BlyssController::generate_token() {
  static byte last_token = 0x7D;
  this->buffer[5] = (this->buffer[5] & 0xF0) | ((last_token & 0xF0) >> 4);
  this->buffer[6] = (last_token & 0x0F) << 4;
  last_token += 10;
}

/**
 * Generate next valid rolling code for RF transmission
 */
void BlyssController::generate_rolling_code() {
  static byte i = 0;
  this->buffer[4] = (this->buffer[4] & 0xF0) | ((RF_ROLLING_CODE[i] & 0xF0) >> 4);
  this->buffer[5] = (this->buffer[5] & 0x0F) |(RF_ROLLING_CODE[i] & 0x0F) << 4;
  if(++i >= sizeof(RF_ROLLING_CODE)) i = 0;
}

/**
 * Change the status (ON / OFF) of the transmitter
 *
 * @param status Status to use (ON or OFF)
 */
void BlyssController::set_status(byte status) {
  if(!status) this->buffer[4] = (this->buffer[4] & 0x0F) | 0x10;
  else this->buffer[4] &= 0x0F;
}

/**
 * Send a complete frame-data buffer over RF
 *
 * @param data Pointer to a RF frame-data buffer
 */
void BlyssController::send_buffer() {
  this->send_header();
  for(byte i = 0; i < 6; ++i) {
    this->send_quarter_MSB(this->buffer[i]);
    this->send_quarter_LSB(this->buffer[i]);
  }
  this->send_quarter_MSB(this->buffer[6]);
  this->send_footer();
}

/**
 * Send a complete frame-data buffer n times to be hooked by the target receiver
 */
void BlyssController::send_command() {
  for(byte i = 0; i < nb_frames; ++i)
    this->send_buffer();
}

/**
 * Copy a RF key ID into a frame-data buffer
 *
 * @param key Pointer to a RF key-data buffer
 * @param overwrite Set to true if you want to overwrite channel data and use data from key buffer
 */
void BlyssController::setKey(byte *key, bool overwrite) {
  this->buffer[0] = 0xFE;
  if(overwrite)
    this->buffer[1] = key[0];
  else
    this->buffer[1] = (this->buffer[1] & 0xF0) | (key[0] & 0x0F);
  this->buffer[2] = key[1];
  if(overwrite)
    this->buffer[3] = key[2];
  else
    this->buffer[3] = (this->buffer[3] & 0x0F) | (key[2] & 0xF0);
}

/**
 * Copy a RF key ID into a frame-data buffer
 *
 * @param key Pointer to a RF key-data buffer
 */
void BlyssController::setKey(byte *key) {
  this->setKey(key, true);
}


/**
 * Set the target sub-channel of the transmitter
 *
 * @param channel Target channel
 */
void BlyssController::set_channel(byte channel) {
  this->buffer[3] = (this->buffer[3] & 0xF0) | (channel & 0x0F);
}

/**
 * Set the target global channel of the transmitter
 *
 * @param channel Target channel
 */
void BlyssController::set_global_channel(byte channel) {
  this->buffer[1] = (this->buffer[1] & 0x0F) | ((channel << 4) & 0xF0);
}

/**
 * Turn switch ON on specified channel
 *
 * @param channel Target channel
 */
void BlyssController::switchOn(int channel) {
    this->set_channel_status(channel, ON);
}

/**
 * Turn switch OFF on specified channel
 *
 * @param channel Target channel
 */
void BlyssController::switchOff(int channel) {
    this->set_channel_status(channel, OFF);
}

void BlyssController::set_channel_status(int channel, byte status) {

  this->set_channel(CH_ALL);
  this->set_status(status);

  /* Insert rolling code and token into frame-data buffer */
  this->generate_rolling_code();
  this->generate_token();

  /* Send RF frame */
  this->send_command();
}

/**
 * Enable transmissions
 *
 * @param nTransmitterPin    Arduino Pin to which the sender is connected to
 */
void BlyssController::enableTransmit(int nTransmitterPin) {
  this->nTransmitterPin = nTransmitterPin;
  pinMode(this->nTransmitterPin, OUTPUT);
}

/**
  * Disable transmissions
  */
void BlyssController::disableTransmit() {
  this->nTransmitterPin = -1;
}
