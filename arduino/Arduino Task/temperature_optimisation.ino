#include <Arduino.h>
#include <math.h>
const long B = 4275000; // B value of the thermistor
const long R0 = 100000; // R0 = 100k
const int pinTempSensor = A0; // Grove - Temperature Sensor connect to A0

//my variables
float temperature_data_array[60]; // array to store temperature data to process in DFT
int sampling_rate = 1000; // sampling rate in ms, changeable, 3 min first
int numSamples = 0; // number of samples to collect
float real[60]; // array to store real part of DFT results,
float imag[60]; // array to store imaginary part of DFT results 
float magnitude[60]; // array to store magnitude of DFT calculations
float fk; // variable to store frequency of dominant frequency component
unsigned long sample_time[60]; //array to store time of when sample is collected 
const int ACTIVE = 0;
const int IDLE = 1;
const int POWER_DOWN = 2;
int power_mode = ACTIVE; // variable to track current power mode, start in active mode
unsigned long timecollecting = 18000; //variable for time spent collecting data
int IDLEcyclecount = 0; //counter for number of cycles in IDLE mode, if 5 are idle straight it goes to power down
float temp_differences[60]; // array to store difference between consecutive temp readings
float temp_moving_avg = 0; // calculate moving average of temperature differences

void collect_temperature_data(){ //collects data and stores it in temperature_data_array
  unsigned long start_time = millis();
  int index = 0; // index for storing data in array
  while (millis() - start_time < timecollecting && index < 60) { // collect data for 3 minutes at the start (180000 ms), && prevents index overflow
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
 if (magnitude[bigIndex] <= 0.01) {
    fk = 0;
    return &fk;} else{
      return &fk; // return pointer to frequency of dominant frequency component
    }
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

float moving_average(){
  if (numSamples < 10){
    for (int i = 1; i < numSamples; i++){
      temp_differences[i] = temperature_data_array[i] - temperature_data_array[i-1]; //calculates differences between each consecutive temp reading
      }
   float temp_diff_sum = 0;
   for (int i = 1; i < numSamples; i++){
     temp_diff_sum += fabs(temp_differences[i]); // sums up absolute value of differences
      }
      float temp_diff_avg = temp_diff_sum / (numSamples - 1); //calculates average of differences, divide by numSamples - 1 because we have one less difference than number of samples
      return temp_diff_avg; // returns average of differences, which is the moving average of the last 10 temperature readings
  } else {
    for (int i = numSamples - 10; i < numSamples; i++){
      temp_differences[i] = temperature_data_array[i] - temperature_data_array[i-1]; //calculates differences between each consecutive temp reading
      }
   float temp_diff_sum = 0;
   for (int i = numSamples - 10; i < numSamples; i++){
     temp_diff_sum += fabs(temp_differences[i]); // sums up absolute value of differences
      }
      float temp_diff_avg = temp_diff_sum / 10; //calculates average of differences,
      return temp_diff_avg; // returns average of differences, which is the moving average of the last 10 temperature readings
      } 
}

int decide_power_mode(){
 if (fk <= 0.1 && temp_moving_avg <= 0.1){ // if dominant frequency is less than 0.1 Hz, we can go to sleep mode. temp_moving_avg set to 0.1 as a low threshold
   return POWER_DOWN;
 } else if (fk > 0.5 || temp_moving_avg >= 0.5){ // if dominant frequency is greater than 0.5 hz, we go to active mode. temp_moving_avg set to 0.5 degrees as a high difference in change. selected OR function as it could have high oscillations but low change in magnitude or vice versa, so it should only work when both magnitude OR fk is high
   return ACTIVE;
 } else { // if dominant frequency is between 0.1 and 0.5 Hz, go to idle mode
   return IDLE;
 }
}

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("Collecting Data...\n"); // visual print out on serial monitor to assure that the program is running without memory failure/overload
  collect_temperature_data(); // collect temperature data for 3 minutes at the start
  timecollecting = 60000;
  apply_dft(); // apply DFT to collected data to find dominant frequency
  send_data_to_pc(); // send collected data and DFT results to PC for analysis
  temp_moving_avg = moving_average();
  power_mode = decide_power_mode(); // decide power mode based on dominant frequency
  Serial.print("Moving Average of Temperature Differences: ");
  Serial.println(temp_moving_avg);
  if (power_mode == ACTIVE){ //decision for when power mode is active
    Serial.println("Power Mode: ACTIVE");
    sampling_rate = 1000/(fk*2); // 
    if (sampling_rate < 250){
    sampling_rate = 250;
      } else if (sampling_rate > 2000){
      sampling_rate = 2000; 
      }
    IDLEcyclecount = 0;
  } else if (power_mode == IDLE){
      Serial.println("Power Mode: IDLE");
      sampling_rate = 5000;
      IDLEcyclecount++;
  } else {
        Serial.println("Power Mode: POWER DOWN");
        sampling_rate = 30000;
        IDLEcyclecount = 0;
      }
  if (IDLEcyclecount >= 5){
    Serial.print("5 IDLE cycles reached, moving to POWER DOWN mode");
    power_mode = POWER_DOWN;
    sampling_rate = 30000;
    IDLEcyclecount = 0;
  }
  Serial.print("Sampling Rate:");
  Serial.println(sampling_rate);
  Serial.print("Fk:");
  Serial.println(fk);
}