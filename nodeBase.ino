//  Page 1

// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "--"
#define BLYNK_DEVICE_NAME "--"
#define BLYNK_AUTH_TOKEN "--"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
#define USE_WROVER_BOARD
//#define USE_TTGO_T7

#include "BlynkEdgent.h"
BlynkTimer timer;


//  NRF24L01
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
#include <printf.h>
RF24 radio(4, 5);              // nRF24L01 (CE,CSN) (7,8)--Arduino || (4,5)--ESP32
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 00;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t node01 = 01;
const uint16_t node02 = 02;
const uint16_t node03 = 03;
const uint16_t node04 = 04;
uint16_t allNode[4] = {node01, node02, node03, node04};

//  Line and Wifi
#include <TridentTD_LineNotify.h>
//#define SSID        "--"      // Wifi's name
//#define PASSWORD    "--"     // Wifi's password
#define LINE_TOKEN  "--"   // รหัส TOKEN ที่ได้มาจากข้างบน



//  Struct for data
struct dataRX {
  byte room = NULL;
  bool alert = false;
  bool stat = false;
};;
struct dataRX dataRX;
bool roomSt = false;  // Room status  // Input form blynk

//  All of function
void _runT();
void _chSt(byte inc);

void setup() {
  Serial.begin(115200);
  delay(100);
  
  
  SPI.begin();
  radio.begin();
  network.begin(99, this_node); //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  radio.printDetails();
  
  Serial.println(LINE.getVersion());
  LINE.setToken(LINE_TOKEN);
  LINE.notify("Hello from ESP32 Main Security");
  
  BlynkEdgent.begin();
  
  timer.setInterval(1, _runT);
}

void loop() {
  BlynkEdgent.run();
  timer.run();
}

//---------------------------------------
//  Page 2

BLYNK_WRITE(V1) {
  roomSt = param.asInt();
  if (roomSt, roomSt) {
    _chSt(01);
    roomSt = false;
  }
}

BLYNK_WRITE(V2) {
  roomSt = param.asInt();
  if (roomSt, roomSt) {
    _chSt(02);
    roomSt = false;
  }
}

BLYNK_WRITE(V3) {
  roomSt = param.asInt();
  if (roomSt) {
    _chSt(03);
    roomSt = false;
  }
}

BLYNK_WRITE(V4) {
  roomSt = param.asInt();
  if (roomSt) {
    _chSt(04);
    roomSt = false;
  }
}


//---------------------------------------
//  Page 3

void _chSt(byte inc) {
  inc--;
  const bool stas = true;
  network.update();
  RF24NetworkHeader header00(allNode[inc]); // (Address where the data is going)
  bool ok = network.write(header00, &stas, sizeof(stas));
}


void _runT() {
  dataRX.room = NULL;
  dataRX.alert = false;
  dataRX.stat = false;

  network.update();
  while ( network.available() ) {
    RF24NetworkHeader header00;
    network.read(header00, &dataRX, sizeof(dataRX));
  }

  if (dataRX.alert) {
    for (byte i = 1; i <= 4; i++) {
      if (dataRX.room == i) {
        if (dataRX.stat) {
          Serial.println("No problem at Room" + String(i) + "!\t");
          LINE.notify("No problem at Room" + String(i) + "!");
        }
        else {
          Serial.println("Detected at Room" + String(i) + "! \t");
          LINE.notify("Detected at Room" + String(i) + "!");
        }
      }
    }
  }
}

