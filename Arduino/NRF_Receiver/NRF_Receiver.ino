
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

RF24 radio(7, 8);  // using pin 7 for the CE pin, and pin 8 for the CSN pin

void setup() {
  Serial.begin(9600);

  radio.begin();

  // Configuration settings.
  radio.setChannel(2);
  radio.setPayloadSize(10);//
  radio.setDataRate(1); // 1 = 2Mbits
  radio.openReadingPipe(0,  (uint8_t*)"gyroc");  // using pipe 1
  radio.setAutoAck(false); // disable AutoAck.
  // END Configuration settings.
  
  radio.startListening();  // put radio in RX mode
  
    
}  // setup
void loop() {

  static uint8_t buffer[7];
  static char msg[30] = {0};
  if (radio.available()) {
      radio.read(buffer, 7);  // Read message into buffer.
      sprintf(msg,"Received: %s",buffer);
      Serial.println(msg);  
  }
 
}
