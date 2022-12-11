//  Page 1
// RX
#include <SimpleTimer.h>
SimpleTimer timer;

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
uint16_t allNode[4] = {node01, node02, node03, node04}; // Start at number 0 --> 0+1=1

//  Line and Wifi
#include <TridentTD_LineNotify.h>
#define SSID        "vivo 1902"      // Wifi's name  // vivo 1902  // OPPO A95
#define PASSWORD    "gg123456"     // Wifi's password
#define LINE_TOKEN  "lSMmKS2JyJHKDHeauWNonMprq6Fwj6xmm42epuahaI2"   // รหัส TOKEN ที่ได้มาจากข้างบน

//  Function
void _ttime();
void _chSt(byte inc);
void _runT();

//  Struct for data
struct dataRX {
  byte room = NULL;
  bool alert = false;
  bool stat = false;
};
struct dataRX dataRX;


//  Start!
void setup() {
  Serial.begin(115200);

  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  byte i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    i++;
    if (i == 50) {
      break;
    }
    delay(100);
  }
  WiFi.status() == WL_CONNECTED ? Serial.print("\nWiFi connected\nIP : ") : Serial.println("Can't connected\n");
  Serial.println(WiFi.localIP());

  Serial.println(LINE.getVersion());
  LINE.setToken(LINE_TOKEN);
  LINE.notify("Hello from ESP32 Main Security");

  SPI.begin();
  radio.begin();
  network.begin(99, this_node); //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  radio.printDetails();

  timer.setInterval(1, _runT);
  timer.setInterval(30L * 1000L, _ttime);
}

void loop() {
  timer.run();
}


//---------------------------------------
//  Page 2


void _ttime() { // Function for test
  dataRX.room = NULL;
  dataRX.alert = false;
  dataRX.stat = false;
  _chSt(1); // Test send to node01
}


void _chSt(byte inc) {
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



