// Loovee @ 2015-8-26
#include <Arduino.h>
#include <math.h>
const int B = 4275000; // B value of the thermistor
const int R0 = 100000; // R0 = 100k
const int pinTempSensor = A0; // Grove - Temperature Sensor connect to A0

//my variables lmao
float temperature_data_array[720]; // array to store temperature data to process in DFT
int sampling_rate = 1000; // sampling rate in ms, changeable, active mode first so we need 1hz first
int numSamples = 0; // number of samples to collect
float real[720]; // array to store real part of DFT results, size to handle worse case 4hz for 3 min, which is 720 samples
float imag[720]; // array to store imaginary part of DFT results 
float magnitude[720]; // array to store magnitude of DFT calculations
float fk; // variable to store frequency of dominant frequency component
unsigned long sample_time[720];
const int ACTIVE = 0;
const int IDLE = 1;
const int POWER_DOWN = 2;
int power_mode = ACTIVE; // variable to track current power mode, start in active mode

void collect_temperature_data(){ //collects data and stores it in temperature_data_array
  unsigned long start_time = millis();
  int index = 0; // index for storing data in array
  
  while (millis() - start_time < 180000 && index < 720) { // collect data for 3 minutes at the start (180000 ms), && prevents index overflow
    int a = analogRead(pinTempSensor);
    float R = 1023.0/a-1.0;
    R = R0*R;
    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // temperature calculations
    temperature_data_array[index] = temperature; // store temperature data in array using index variable!
    sample_time[index] = millis(); // store sample time
    index++;
    delay(sampling_rate);
  }
  numSamples = index; // update numSamples to reflect actual number of samples collected
}

float* apply_dft(){
 int bigIndex = 1; // variable to track index of largest magnitude, start at 1 to ignore DC component
 float sampling_rateHz = 1000.0/sampling_rate; // variable to store sampling rate in Hz, calculated from sampling_rate in ms
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

void send_data_to_pc(){
  float sampling_rateHz = 1000.0/sampling_rate;
  for (int i = 0; i < numSamples; i++){
   Serial.print("Time: ");
   Serial.print(sample_time[i]);
   Serial.print(" ms, Temperature: ");
   Serial.print(temperature_data_array[i]);
   Serial.print(" °C, Frequency: ");
   Serial.print(i * sampling_rateHz / numSamples);
   Serial.print(" Hz, Magnitude: ");
   Serial.println(magnitude[i]);
  }
}

int moving_average(int data[], int size, int window) {
  
}

int decide_power_mode(){
 if (fk <= 0.1){ // if dominant frequency is less than 0.1 Hz, we can go to sleep mode
   return POWER_DOWN;
 } else if (fk > 0.5){ // if dominant frequency is greater than 0.5 hz, we go to active mode
   return ACTIVE;
 } else { // if dominant frequency is between 0.1 and 0.5 Hz, go to idle mode
   return IDLE;
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