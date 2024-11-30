/*
  Software serial multple serial test

 Receives from the two software serial ports,
 sends to the hardware serial port.

 In order to listen on a software port, you call port.listen().
 When using two software serial ports, you have to switch ports
 by listen()ing on each one in turn. Pick a logical time to switch
 ports, like the end of an expected transmission, or when the
 buffer is empty. This example switches ports when there is nothing
 more to read from a port

 The circuit:
 Two devices which communicate serially are needed.
 * First serial device's TX attached to digital pin 10(RX), RX to pin 11(TX)
 * Second serial device's TX attached to digital pin 8(RX), RX to pin 9(TX)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created 18 Apr. 2011
 modified 19 March 2016
 by Tom Igoe
 based on Mikal Hart's twoPortRXExample

 This example code is in the public domain.

 */
#include <Wire.h>
#define SLAVE_ADDR 0x62
#define ANSWERSIZE 9
byte data[ANSWERSIZE];



#include <SoftwareSerial.h>
// software serial #1: RX = digital pin 10, TX = digital pin 11
SoftwareSerial portOne(13, 12);

// software serial #2: RX = digital pin 8, TX = digital pin 9
// on the Mega, use other pins instead, since 8 and 9 don't work on the Mega
SoftwareSerial portTwo(10, 11);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // Start each software serial port
  portOne.begin(9600);
  portTwo.begin(9600);

  //SCD4x setup
  Wire.begin();
  Serial.begin(9600);
  //Serial.println("I2C Master Demonstration");
  
}

void loop() {

  pranaair_co2();
  delay(1000);
  mhz19b();
  delay(1000);
  //Serial.println("Both of the sensor data extraction has been completed..!!!");
  //delay(1000);
  scd41x();
  delay(1000);

}

void mhz19b()
{
  // By default, the last intialized port is listening.
  // when you want to listen on a port, explicitly select it:
  portOne.listen();
  // Send the command to read CO2 concentration
  byte command[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  portOne.write(command, sizeof(command));

  // Wait for the response
  delay(100); // Adjust the delay based on the device's response time
  while (portOne.available() < 9) {} // Wait until a complete response is received

  // Read the response
  byte response_01[9];
  portOne.readBytes(response_01, 9);

  // Validate the response
  if (response_01[0] == 0xFF && response_01[1] == 0x86) {
    // Extract CO2 concentration from the response
    int co2High = response_01[2];
    int co2Low = response_01[3];
    int co2Concentration_01 = co2High * 256 + co2Low;

    // Print the CO2 concentration
    Serial.print("MHZ19B CO2 Concentration: ");
    Serial.println(co2Concentration_01);

  } else {
    Serial.println("Invalid response");
  }

  // Wait for some time before sending the next command
  delay(1000); // Adjust the delay based on your application needs
}

void pranaair_co2()
{
    // Now listen on the second port
  portTwo.listen();

  
  // Send the command to read CO2 concentration
  byte command[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  portTwo.write(command, sizeof(command));

  // Wait for the response
  delay(100); // Adjust the delay based on the device's response time
  while (portTwo.available() < 9) {} // Wait until a complete response is received

  // Read the response
  byte response_02[9];
  portTwo.readBytes(response_02, 9);

  // Validate the response
  if (response_02[0] == 0xFF && response_02[1] == 0x86) {
    // Extract CO2 concentration from the response
    int co2High = response_02[2];
    int co2Low = response_02[3];
    int co2Concentration_02 = co2High * 256 + co2Low;

    // Print the CO2 concentration
    Serial.print("Prana air CO2 Concentration: ");
    Serial.println(co2Concentration_02);

  } else {
    Serial.println("Invalid response");
  }

  // Wait for some time before sending the next command
  delay(1000); // Adjust the delay based on your application needs
}

void scd41x()
{
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0x21); // Send first byte of the 16-bit data
  Wire.write(0xb1); // Send second byte of the 16-bit data
  Wire.endTransmission();
  delay(5000);

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0xec); // Send first byte of the 16-bit data
  Wire.write(0x05); // Send second byte of the 16-bit data
  Wire.endTransmission();
  //Serial.println("Receive data");
  Wire.requestFrom(SLAVE_ADDR, ANSWERSIZE);
  
  // Read and print all received bytes
  for (int i = 0; i < ANSWERSIZE; i++) {
    if (Wire.available()) {
      data[i] = Wire.read();
      //Serial.print(data[i]);
      //Serial.print(" ");
    }
  }
  //Serial.println();
  delay(1000);



  //read co2 concentration
  int a = data[0];
  int b = data[1];

  char hexBuffer[5];
  snprintf(hexBuffer, sizeof(hexBuffer), "%02X%02X", a, b);    

  // Merging the two hexadecimal values into one
  String mergedHex = hexBuffer;

  // Converting the merged hexadecimal value back to decimal
  long merged_co2 = strtol(mergedHex.c_str(), NULL, 16);

  Serial.print("co2: ");
  Serial.print(merged_co2);
  Serial.print(" in ppm");

  //read temperature
  int c = data[3];
  int d = data[4];
  unsigned int merged_temp = (c << 8) | d; // Combine bytes into a 16-bit unsigned integer

  float Temp = -45.0 + 175.0 * (static_cast<float>(merged_temp) / 65536.0);

  Serial.print("\t Temperature: ");
  Serial.print(Temp);
  Serial.print(" degrees Celsius");


  //read relative humidity
  int e = data[6];
  int f = data[7];

  unsigned int merged_hum = (e << 8) | f;
  float Relative_Humidity = 100.0 * (static_cast<float>(merged_hum) / 65536.0);

  Serial.print("\t Humidity: ");
  Serial.print(Relative_Humidity);
  Serial.println(" % RH");

  //Serial.println(" ");
  delay(1000);
}
