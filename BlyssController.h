/**
  BlyssController - Arduino libary for remote control blyss/liveez components
*/

#ifndef _BlyssController_h
#define _BlyssController_h

#include <stdlib.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
  #include "Energia.h"
#else
  #include <wiringPi.h>
  #include <stdint.h>
  #include <ctype.h>
  #include "binary.h"

  #define bitRead(value, bit) (((value) >> (bit)) & 0x01)
  #define bitSet(value, bit) ((value) |= (1UL << (bit)))
  #define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
  #define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#endif

enum {
  OFF, ON,
  CH_1 = 8, CH_2 = 4, CH_3 = 2, CH_4 = 1, CH_5 = 3, CH_ALL = 0,
  CH_A = 0, CH_B = 1, CH_C = 2, CH_D = 3
};

class BlyssController {

  public:
    BlyssController();

    void switchOn(int channel);
    void switchOff(int channel);

    void setKey(char *key);
    void setKey(char *key, bool overwrite);

    void enableTransmit(int nTransmitterPin);
    void disableTransmit();

  private:

    void set_channel_status(int channel, char status);

    void transmit(bool value);

    void generate_rolling_code();
    void generate_token();

    void send_header();
    void send_footer();

    void send_command();
    void send_buffer();

    void set_status(char status);
    void set_channel(char channel);
    void set_global_channel(char channel);

    void send_one();
    void send_zero();
    void send_quarter_MSB(char data);
    void send_quarter_LSB(char data);

    int nTransmitterPin;
    /** Frame-data buffer (key ID + status flag + rolling code + token */
    char buffer[7];
};

#endif
