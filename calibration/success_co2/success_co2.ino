#include <Arduino.h>
#include <SoftwareSerial.h>

#define RX_PIN 13 //10
#define TX_PIN 12  //11
#define BAUDRATE 9600

SoftwareSerial mySerial(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(9600); 
  mySerial.begin(BAUDRATE); 
}

void loop() {
  // Send the command to read CO2 concentration
  byte command[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  mySerial.write(command, sizeof(command));

  // Wait for the response
  delay(100); // Adjust the delay based on the device's response time
  while (mySerial.available() < 9) {} // Wait until a complete response is received

  // Read the response
  byte response[9];
  mySerial.readBytes(response, 9);

  // Validate the response
  if (response[0] == 0xFF && response[1] == 0x86) {
    // Extract CO2 concentration from the response
    int co2High = response[2];
    int co2Low = response[3];
    int co2Concentration = co2High * 256 + co2Low;

    // Print the CO2 concentration
    Serial.print("MHZ19B CO2 Concentration: ");
    Serial.println(co2Concentration);

  } else {
    Serial.println("Invalid response");
  }

  // Wait for some time before sending the next command
  delay(1000); // Adjust the delay based on your application needs
}
