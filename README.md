BlyssController-raspberry
===

BlyssController-raspberry is for controlling 433(.92)Mhz Blyss (Liveez) receiver with the Raspberry pi. 

This code is based on the hard work on [reverse engineering achieved by Skywodd](http://skyduino.wordpress.com/2012/07/19/hack-partie-2-reverse-engineering-des-interrupteurs-domotique-blyss/) over the Blyss protocol


### Dependencies
wiringPi : [https://projects.drogon.net/raspberry-pi/wiringpi/]()

### Building WiringPi

```bash
sudo apt-get install git-core
git clone git://git.drogon.net/wiringPi
cd wiringPi/wiringPi
./build
```

## Usage

BlyssController-raspberry is shipped with an example program *send* that you have to compile by executing *make*. 
You may want to change the used GPIO pin before compilation in the send.cpp source file.

```bash
git clone git@github.com:pyrou/X10RF-raspberry.git
cd X10RF-raspberry
make
```

The program *send* require the RF-key of the remote paired with the device you want to control. This key can be found using a cheap 433Mhz receiver and an Arduino loaded with the [RF_Blyss_Sniffer sketch](https://github.com/skywodd/arduino_blyss_hack/tree/master/Blyss_arduino_code/RF_Blyss_Sniffer) by Skywodd

After getting your RF-key, use the send progam as following :

```bash
./sudo send 735AC8 3 0
````
This command should turn off the device on channel 3

```bash
./sudo send 735AC8 0 1
````
This command should turn on any device paired with the 735AC8 remote
