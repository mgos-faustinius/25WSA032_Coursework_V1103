// Loovee @ 2015-8-26
#include <Arduino.h>
#include <math.h>
const int B = 4275000; // B value of the thermistor
const int R0 = 100000; // R0 = 100k
const int pinTempSensor = A0; // Grove - Temperature Sensor connect to A0

//my variables lmao
int temperature_data_array[180]; // array to store temperature data to process in DFT
int sampling_rate = 1000; // sampling rate in ms, changeable, active mode first so we need 1hz first

void collect_temperature_data(){ //collects data and stores it in temperature_data_array
  unsigned long start_time = millis();
  int index = 0; // index for storing data in array
  while (millis() - start_time < 180000 && index < 180) { // collect data for 3 minutes at the start (180000 ms), && prevents index overflow
    int a = analogRead(pinTempSensor);
    float R = 1023.0/a-1.0;
    R = R0*R;
    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // temperature calculations
    temperature_data_array[index] = temperature; // store temperature data in array using index variable!
    index++;
    delay(sampling_rate);
  }
}

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int a = analogRead(pinTempSensor);
  float R = 1023.0/a-1.0;
  R = R0*R;
  float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet
  Serial.print("temperature = ");
  Serial.println(temperature);
  delay(100);
}