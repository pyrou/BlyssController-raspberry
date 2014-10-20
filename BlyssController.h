/**
  BlyssController - Arduino libary for remote control blyss/liveez components
*/

#ifndef _BlyssController_h
#define _BlyssController_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
  #include "Energia.h"
#else
  #include "WProgram.h"
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

    void setKey(byte *key);
    void setKey(byte *key, bool overwrite);

    void enableTransmit(int nTransmitterPin);
    void disableTransmit();

  private:

    void set_channel_status(int channel, byte status);

    void transmit(bool value);

    void generate_rolling_code();
    void generate_token();

    void send_header();
    void send_footer();

    void send_command();
    void send_buffer();

    void set_status(byte status);
    void set_channel(byte channel);
    void set_global_channel(byte channel);

    void send_one();
    void send_zero();
    void send_quarter_MSB(byte data);
    void send_quarter_LSB(byte data);

    int nTransmitterPin;
    /** Frame-data buffer (key ID + status flag + rolling code + token */
    byte buffer[7];
};

#endif
