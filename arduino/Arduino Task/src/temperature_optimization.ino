// Loovee @ 2015-8-26
#include <Arduino.h>
#include <math.h>
const int B = 4275000; // B value of the thermistor
const int R0 = 100000; // R0 = 100k
const int pinTempSensor = A0; // Grove - Temperature Sensor connect to A0

//my variables lmao
int temperature_data_array[180]; // array to store temperature data to process in DFT
int sampling_rate = 1000; // sampling rate in ms, changeable, active mode first so we need 1hz first
int numSamples = 180; // number of samples to collect, active mode first so we need 180 samples for 3 min at 1hz
float real[720]; // array to store real part of DFT results, size to handle worse case 4hz for 3 min, which is 720 samples
float imag[720]; // array to store imaginary part of DFT results 
float magnitude[720]; // array to store magnitude of DFT calculations
float fk; // variable to store frequency of dominant frequency component

void collect_temperature_data(){ //collects data and stores it in temperature_data_array
  unsigned long start_time = millis();
  int index = 0; // index for storing data in array
  while (millis() - start_time < 180000 && index < numSamples) { // collect data for 3 minutes at the start (180000 ms), && prevents index overflow
    int a = analogRead(pinTempSensor);
    float R = 1023.0/a-1.0;
    R = R0*R;
    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // temperature calculations
    temperature_data_array[index] = temperature; // store temperature data in array using index variable!
    index++;
    delay(sampling_rate);
  }
  magnitude[index] = 0;
}

float* apply_dft(){
 int bigIndex = 1; // variable to track index of largest magnitude, start at 1 to ignore DC component
 int sampling_rateHz = 1000/sampling_rate; // convert sampling rate from ms to Hz
 for (int i = 0; i < numSamples; i++){ // zero out real and imag arrays to 0 before DFT calculations
    real[i] = 0;
    imag[i] = 0;
 }
 for (int k = 1; k < numSamples; k++){
   for (int n = 0; n < numSamples; n++){
     // DFT calculations here, using temperature_data_array[n] for the input data
     real[k] += temperature_data_array[n] * cos(2 * PI * k * n / numSamples);
     imag[k] -= temperature_data_array[n] * sin(2 * PI * k * n / numSamples);
   }
   magnitude[k] = sqrt(real[k] * real[k] + imag[k] * imag[k]);
   if (magnitude[k] > magnitude[bigIndex]){ //checks for largest magnitude and updates bigIndex
     bigIndex = k;
   }
  } 
 fk = (bigIndex * sampling_rateHz) / numSamples; // calculate frequency corresponding to index k, use static so it lives in program
return &fk; // return pointer to frequency of dominant frequency component
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