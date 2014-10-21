/*
 Usage: ./send <system_code> <unit_code> <command>
 system_code is a letter from A to P (case insensitive)
 Valid commands are
   0 for OFF
   1 for ON
   2 for BRIGHT
   3 for DIMMED
 */

#include "BlyssController.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

    /*
     output RF_PIN is hardcoded for testing purposes
     see https://projects.drogon.net/raspberry-pi/wiringpi/pins/
     for pin mapping of the raspberry pi GPIO connector
     */
    int RF_PIN = 15;

    char* systemCodeRaw = argv[1];
    int systemCodeRawInt = (int) strtoul(systemCodeRaw, NULL, 16);

    char RF_KEY[] = {
      (systemCodeRawInt & (0xFF << 16)) >> 16,
      (systemCodeRawInt & (0xFF << 8)) >> 8,
      (systemCodeRawInt & 0xFF)
    };

    int channel;
    int unitCode = atoi(argv[2]);
    int command  = atoi(argv[3]);

    if (wiringPiSetup () == -1) return 1;

    printf("sending systemCode[%s] unitCode[%i] command[%i]\n",
            systemCodeRaw,
            unitCode,
            command);

    BlyssController controller = BlyssController();
    controller.enableTransmit(RF_PIN);
    controller.setKey(RF_KEY);

    switch(unitCode) {
        case 1: channel = CH_1; break;
        case 2: channel = CH_2; break;
        case 3: channel = CH_3; break;
        case 4: channel = CH_4; break;
        case 5: channel = CH_5; break;
        case 0:
            channel = CH_ALL;
            break;
        default:
            printf("unitCode[%i] is unsupported\n", command);
            return -1;
    }

    switch(command) {
        case 0:
            controller.switchOff(channel);
            break;
        case 1:
            controller.switchOn(channel);
            break;
        default:
            printf("command[%i] is unsupported\n", command);
            return -1;
    }
    return 0;
}
