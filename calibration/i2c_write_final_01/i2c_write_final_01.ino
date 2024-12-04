#include <Wire.h>
#define SLAVE_ADDR 0x62
#define ANSWERSIZE 9
byte data[ANSWERSIZE];

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("I2C Master Demonstration");
}



void loop() {
  delay(1000);


  
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0xec05);
  Wire.endTransmission();
  Serial.println("Receive data");
  Wire.requestFrom(SLAVE_ADDR,ANSWERSIZE);
  delay(1);
  int index = 0;
  
  while (Wire.available())
  {
    byte response = Wire.read();
    Serial.println(response);
    data[index] = response;
    index++;
    delay(1000);
  }

  Serial.println("The entire response from sensor is: ");
  for(int i = 0; i<ANSWERSIZE; i++)
  {
    Serial.print(data[i]);
    Serial.println(" ");
  }
    Serial.println();

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
    Serial.println(" in ppm");

    int c = data[3];
    int d = data[4];

    /*char hex_temp[5];
    snprintf(hex_temp, sizeof(hex_temp), "%02X%02X", c, d);

    String mergedHex_temp = hex_temp;

    long merged_temp = strtol(mergedHex_temp.c_str(), NULL, 16);

    long Temp = -45 + 175 * ((merged_temp)/65536);

    Serial.print("temp: ");
    Serial.print(Temp);
    Serial.println(" degree celsius");*/
  
    unsigned int merged_temp = (c << 8) | d; // Combine bytes into a 16-bit unsigned integer

    float Temp = -45.0 + 175.0 * (static_cast<float>(merged_temp) / 65536.0);

    Serial.print("Temperature: ");
    Serial.print(Temp);
    Serial.println(" degrees Celsius");

    int e = data[6];
    int f = data[7];

    unsigned int merged_hum = (e << 8) | f;
    float Relative_Humidity = 100.0 * (static_cast<float>(merged_hum) / 65536.0);

    Serial.print("Humidity: ");
    Serial.print(Relative_Humidity);
    Serial.println(" % RH");
}
