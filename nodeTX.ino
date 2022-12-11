//  Page 1
// TX
// Wemos D1 Mini
const byte tr = 02; // This Room

#include <SimpleTimer.h>
SimpleTimer timer;

#define inputPin D0 // choose the input pin (for PIR sensor)
bool pirState = false; // we start, assuming no motion detected
bool val = false; // variable for reading the pin status


#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
#include <printf.h>
#define cep D2  // CE pin 
#define cnp D8  // CNS pin
RF24 radio(cep, cnp);             // nRF24L01 (CE,CSN) (7,8)-- Aduino || (0,8)--Wemos D1 mini
RF24Network network(radio);       // Include the radio in the network
const uint16_t nodeBase = 00;
const uint16_t this_node = tr;    // Address of this node in Octal format ( 04, 031, etc)

//  Struct for sends
struct dataTX {
  const byte room = tr;
  bool alert = false;
  bool stat = false;
};
struct dataTX dataTX;

// Function
void _loopTx();

// Start!
void setup() {
  Serial.begin(115200);

  pinMode(inputPin, INPUT); // declare sensor as input

  SPI.begin();
  radio.begin();
  network.begin(99, this_node); //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  radio.printDetails();
  
  Serial.println("\nStart! : Room" + String(tr));

  timer.setInterval(1, _loopTx);
}

void loop() {
  timer.run();
}

//-------------------------------
//  Page 2
void _loopTx() {
  network.update();
  bool chSt = false;  // For data from NodeBase
  while ( network.available() ) {
    RF24NetworkHeader header00;
    network.read(header00, &chSt, sizeof(chSt));
//    Serial.println(String(chSt));
  }

  val = digitalRead(inputPin);
  if ((val == true) || (chSt == true)) {
    if (pirState == false) {
      Serial.print("\nMotion detected! \t");
      Serial.println(String(chSt));
      pirState = true;

      //  Send data
      dataTX.alert = true;
      dataTX.stat = chSt;
      RF24NetworkHeader header00(nodeBase); // (Address where the data is going)
      bool ok = network.write(header00, &dataTX, sizeof(dataTX));
    }
  }
  else {
    if (pirState == true) {
      Serial.print("Motion ended! \t");  // print on output change
      Serial.println(String(chSt));
      pirState = false;
      dataTX.alert = false;
      dataTX.stat = false;
    }
  }
}
