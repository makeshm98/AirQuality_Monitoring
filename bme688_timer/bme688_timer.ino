#include <bsec2.h>
#include<Wire.h>

#define SLAVE_ADDR 0x68
#define ANSWERSIZE 7

byte data[ANSWERSIZE];
byte data_01 [ANSWERSIZE];

int year, month, day, hour, minute, second;
int flag = 0;
int a = 0;
String startTime_01;
String startTime_02;
String startTime_03;

String array_01[100];
String array_02[100];
String array_03[100];
int currentIndex = 0;

/* Macros used */
#define PANIC_LED   LED_BUILTIN
#define ERROR_DUR   1000

/* Helper functions declarations */
/**
 * @brief : This function toggles the led when a fault was detected
 */
void errLeds(void);

/**
 * @brief : This function checks the BSEC status, prints the respective error code. Halts in case of error
 * @param[in] bsec  : Bsec2 class object
 */
void checkBsecStatus(Bsec2 bsec);

/**
 * @brief : This function is called by the BSEC library when a new output is available
 * @param[in] input     : BME68X sensor data before processing
 * @param[in] outputs   : Processed BSEC BSEC output data
 * @param[in] bsec      : Instance of BSEC2 calling the callback
 */
void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec);

/* Create an object of the class Bsec2 */
Bsec2 envSensor;
bsec_bme_settings_t settings;
/* Entry point for the example */
void setup(void)
{
    Wire.begin();

    /* Desired subscription list of BSEC2 outputs */
    bsecSensor sensorList[] = {
            BSEC_OUTPUT_IAQ,
            BSEC_OUTPUT_RAW_TEMPERATURE,
            BSEC_OUTPUT_RAW_PRESSURE,
            BSEC_OUTPUT_RAW_HUMIDITY,
            BSEC_OUTPUT_RAW_GAS,
            BSEC_OUTPUT_STABILIZATION_STATUS,
            BSEC_OUTPUT_RUN_IN_STATUS,
            BSEC_OUTPUT_STATIC_IAQ,
            BSEC_OUTPUT_CO2_EQUIVALENT,
            BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
            BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
            BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
            BSEC_OUTPUT_GAS_PERCENTAGE
    };

    /* Initialize the communication interfaces */
    Serial.begin(115200);
    Wire.begin();
    pinMode(PANIC_LED, OUTPUT);

    /* Valid for boards with USB-COM. Wait until the port is open */
    while(!Serial) delay(10);

    /* Initialize the library and interfaces */
    if (!envSensor.begin(BME68X_I2C_ADDR_HIGH, Wire))
    {
        checkBsecStatus(envSensor);
    }

    /* Subsribe to the desired BSEC2 outputs */
    if (!envSensor.updateSubscription(sensorList, ARRAY_LEN(sensorList), BSEC_SAMPLE_RATE_ULP))
    {
        checkBsecStatus(envSensor);
    }

    /* Whenever new data is available call the newDataCallback function */
    envSensor.attachCallback(newDataCallback);

    Serial.println("BSEC library version " + \
            String(envSensor.version.major) + "." \
            + String(envSensor.version.minor) + "." \
            + String(envSensor.version.major_bugfix) + "." \
            + String(envSensor.version.minor_bugfix));
      //settings.heater_temperature = 300; // 300°C
}

/* Function that is looped forever */
void loop(void)
{
    /* Call the run function often so that the library can 
     * check if it is time to read new data from the sensor  
     * and process it.
     */
    if (!envSensor.run())
    {
        checkBsecStatus(envSensor);
    }
    //Serial.print("heater_temperature: ");
    //Serial.println(settings.heater_temperature);
    //Serial.print("heater_duration: ");
    //Serial.println(settings.heater_duration);
    //Serial.print("pressure_oversampling: ");
    //Serial.println(settings.pressure_oversampling);
    //delay(1000);

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

  second = data_01[0];
  minute = data_01[1];
  hour   = data_01[2];
  day    = data_01[3];
  month  = data_01[4];
  year   = data_01[5];

  static bool executed_01 = false;
  static bool executed_02 = false;
  static bool executed_03 = false;

  static bool repeat_01 = false;
  static bool repeat_02 = false;
  static bool repeat_03 = false;

  if((flag == 1 && a == 1 && !executed_01) || (repeat_01))
  {
    startTime_01 = time(second, minute, hour, day, month, year);
    executed_01 = true;
    repeat_02 = true;
    repeat_03 = true;
    if(currentIndex < 50)
    {
      array_01[currentIndex] = startTime_01;
      currentIndex++;
    }
  }
    Serial.print("First accuracy Start Time : ");
    Serial.println(startTime_01);
    printArray_01();
    
  if((flag == 2 && a == 2 && !executed_02)  && (repeat_02))
  {
    startTime_02 = time(second, minute, hour, day, month, year);
    executed_02 = true;
    repeat_01 = true;
    repeat_03 = true;
    if(currentIndex < 50)
    {
      array_02[currentIndex] = startTime_02;
      currentIndex++;
    }
  }
    Serial.print("Second accuracy Start Time : ");
    Serial.println(startTime_02);
    printArray_02();

  if((flag == 3 && a == 3 && !executed_03)  && (repeat_03))
  {
    startTime_03 = time(second, minute, hour, day, month, year);
    executed_03 = true;
    repeat_01 = true;
    repeat_02 = true;
    if(currentIndex < 50)
    {
      array_03[currentIndex] = startTime_03;
      currentIndex++;
    }
  }
    Serial.print("Third accuracy Start Time : ");
    Serial.println(startTime_03);
    printArray_03();
  //array_01[currentIndex] = startTime_01;

  delay(1000);
}

void errLeds(void)
{
    while(1)
    {
        digitalWrite(PANIC_LED, HIGH);
        delay(ERROR_DUR);
        digitalWrite(PANIC_LED, LOW);
        delay(ERROR_DUR);
    }
}

void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec)
{
    if (!outputs.nOutputs)
    {
        return;
    }

    Serial.println("BSEC outputs:\n\ttimestamp = " + String((int) (outputs.output[0].time_stamp / INT64_C(1000000))));
    for (uint8_t i = 0; i < outputs.nOutputs; i++)
    {
        const bsecData output  = outputs.output[i];
        switch (output.sensor_id)
        {
            case BSEC_OUTPUT_IAQ:
                Serial.println("\tiaq = " + String(output.signal));
                Serial.println("\tiaq accuracy = " + String((int) output.accuracy));
                break;
            case BSEC_OUTPUT_RAW_TEMPERATURE:
                Serial.println("\ttemperature = " + String(output.signal));
                break;
            case BSEC_OUTPUT_RAW_PRESSURE:
                Serial.println("\tpressure = " + String(output.signal));
                break;
            case BSEC_OUTPUT_RAW_HUMIDITY:
                Serial.println("\thumidity = " + String(output.signal));
                break;
            case BSEC_OUTPUT_RAW_GAS:
                Serial.println("\tgas resistance = " + String(output.signal));
                break;
            case BSEC_OUTPUT_STABILIZATION_STATUS:
                Serial.println("\tstabilization status = " + String(output.signal));
                break;
            case BSEC_OUTPUT_RUN_IN_STATUS:
                Serial.println("\trun in status = " + String(output.signal));
                break;
            case BSEC_OUTPUT_STATIC_IAQ:
                break;
            case BSEC_OUTPUT_CO2_EQUIVALENT:
                Serial.println("\tBSEC_OUTPUT_CO2_EQUIVALENT = " + String(output.signal));
                //Serial.println("\tCO2 accuracy = " + String((int) output.accuracy));
                break;
            case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
                Serial.println("\tBSEC_OUTPUT_BREATH_VOC_EQUIVALENT = " + String(output.signal));
                break;
            case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
                Serial.println("\tBSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE = " + String(output.signal));
                break;
            case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
                Serial.println("\tBSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY = " + String(output.signal));
                break;
            case BSEC_OUTPUT_GAS_PERCENTAGE:
                Serial.println("\tBSEC_OUTPUT_GAS_PERCENTAGE = " + String(output.signal));
                break;
            default:
                break;
        }
    
     if(output.accuracy == 1)
      {
        //first_accuracy(output.accuracy);
        flag = 1;
        a = 1;
      }
      else if(output.accuracy == 2)
      {
        flag = 2;
        a = 2;
      }

      else if(output.accuracy == 3)
      {
        flag = 3;
        a = 3;
      }
      /*else if(output.accuracy == 2)
      {
        second_accuracy(output.accuracy);
      }
      else if(output.accuracy == 3)
      {
        third_accuracy(output.accuracy);
      }*/
    }

}

/*void first_accuracy(int)
{
  
}

void second_accuracy(int)
{

}

void third_accuracy(int)
{

}*/

String time(int, int, int, int, int, int)
{
  Serial.println("Hello World..!!");
  String startTime = "Date: " + String(data_01[4]) + "/" + String(data_01[5]) + "/" + String(data_01[6]) +
                      "\tTime: " + String(data_01[2]) + ":" + String(data_01[1]) + ":" + String(data_01[0]);
  Serial.println(startTime);
  //a = 1;
  return startTime;
}

void checkBsecStatus(Bsec2 bsec)
{
    if (bsec.status < BSEC_OK)
    {
        Serial.println("BSEC error code : " + String(bsec.status));
        errLeds(); /* Halt in case of failure */
    }
    else if (bsec.status > BSEC_OK)
    {
        Serial.println("BSEC warning code : " + String(bsec.status));
    }

    if (bsec.sensor.status < BME68X_OK)
    {
        Serial.println("BME68X error code : " + String(bsec.sensor.status));
        errLeds(); /* Halt in case of failure */
    }
    else if (bsec.sensor.status > BME68X_OK)
    {
        Serial.println("BME68X warning code : " + String(bsec.sensor.status));
    }
}

void printArray_01()
{
  Serial.print("First accuracy Array: ");
  for(int i = 0; i < currentIndex; i++)
  {
    Serial.println(array_01[i]);
  }
}

void printArray_02()
{
  Serial.print("Second accuracy Array: ");
  for(int i = 0; i < currentIndex; i++)
  {
    Serial.println(array_02[i]);
  }
}

void printArray_03()
{
  Serial.print("Third accuracy Array: ");
  for(int i = 0; i < currentIndex; i++)
  {
    Serial.println(array_03[i]);
  }
}