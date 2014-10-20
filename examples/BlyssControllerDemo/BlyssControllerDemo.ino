/**
 * Blyss protocol spoofer example
 */

#include <BlyssController.h>

BlyssController controller = BlyssController();

/** Key ID to spoof */
byte RF_KEY[] = {
  //0x79, 0x5F, 0x78 // Micro émetteur @SkyWodd
  //0x39, 0x21, 0xA8 // Télécommande @SkyWodd
  0x72, 0x8C, 0xE8 // room @pyrou
};

/** setup() */
void setup() {
  controller.enableTransmit(7);
  controller.setKey(RF_KEY);

  /* Activity led as output and low */
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

/** loop() */
void loop() {

  /* Virtual switch state */
  static byte state = false;
  digitalWrite(13, state);

  if(state) {
    controller.switchOn(CH_ALL);
  } else {
    controller.switchOff(CH_ALL);
  }

  /* Wait some time (don't want to blow-up my ligth bulb !) */
  delay(1000);

  /* Toggle virtual switch state */
  state = !state;
}
