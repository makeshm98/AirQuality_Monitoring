#include<Wire.h>

#define SLAVE_ADDR 0x68
#define ANSWERSIZE 7

byte data[ANSWERSIZE];
byte data_01 [ANSWERSIZE];

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  Serial.println("I2C Master Demonstration");
}

void loop()
{
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0x06); //year
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0x05); //Month
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0x04); //Date
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0x03); //Day
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0x02); //Hours
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0x01); //Minutes
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0x00); //Seconds
  Wire.endTransmission();

  Wire.requestFrom(SLAVE_ADDR, ANSWERSIZE);

  for(int i = 0; i < ANSWERSIZE; i++){
    if(Wire.available()) {
      data[i] = Wire.read();
      data_01[i] = ((data[i] / 16 * 10) + (data[i] % 16));
      //Serial.print(data_01[i]);
      //Serial.print(".");
    }
  }
  Serial.print("\n");
  Serial.print("Date : ");
  Serial.print(data_01[4]);
  Serial.print("/");
  Serial.print(data_01[5]);
  Serial.print("/");
  Serial.print(data_01[6]);

  Serial.print("\t");
  Serial.print("Time : ");
  Serial.print(data_01[2]);
  Serial.print(":");
  Serial.print(data_01[1]);
  Serial.print(":");
  Serial.print(data_01[0]);

  Serial.println();
  delay(1000);
}