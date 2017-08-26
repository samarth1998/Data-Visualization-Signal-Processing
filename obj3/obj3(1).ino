#include "CurieTimerOne.h"
#define FREQUENCY 200  //Setup the target sampling frequency (Hz)s

void setup() {
  //Setup for Analog input pins
  pinMode(A0, INPUT);
  Serial.begin(250000);  //Setup serial at 250000 baud
  while (!Serial);  //Wait until Serial Monitor is opened
  int period = 1000000 / FREQUENCY;  //Setup the sampling period
  CurieTimerOne.start(period, &analogSampleIsr);// set timer and callback to Analog Sampling
}
void analogSampleIsr()   // callback function when interrupt is asserted
{
  static long initTime, timeSpent;
  static double rawData, EMGData, highPassData, bandPassData, rectifiedData, boxOutput;
  //Read Data
  rawData = analogRead(A0);
  initTime = micros();
  //Start Calculation
  EMGData = ((double(rawData) / 1023.0) * 3300 - 1500) / 3600.0;
  highPassData = thirdOrderIIR_high(EMGData);
  bandPassData = thirdOrderIIR_low(highPassData);
  rectifiedData = abs(bandPassData);
  boxOutput = boxcarFilterSample(rectifiedData);
  //End of calculation. Calculate time spent
  timeSpent = micros() - initTime;
  //Print data to serial
  Serial.print(initTime); Serial.print(" "); Serial.print(rawData); Serial.print(" ");
  Serial.print(EMGData, 5); Serial.print(" "); Serial.print(highPassData, 5); Serial.print(" ");
  Serial.print(bandPassData, 5); Serial.print(" "); Serial.print(rectifiedData, 5); Serial.print(" ");
  Serial.print(boxOutput, 5); Serial.print(" "); Serial.println(timeSpent, 5);
}


float boxcarFilterSample(float sample)
{
  static const int boxcarWidth = 100; // Change this value to alter boxcar length
  static float recentSamples[boxcarWidth] = {0}; // hold onto recent samples
  static int readIndex = 0;              // the index of the current reading
  static float total = 0;                  // the running total
  static float average = 0;                // the average

  // subtract the last reading:
  total = total - recentSamples[readIndex];
  // add new sample to list (overwrite oldest sample)
  recentSamples[readIndex] = sample;
  // add the reading to the total:
  total = total + recentSamples[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= boxcarWidth) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / boxcarWidth;
  // send it to the computer as ASCII digits
  return average;
}

float thirdOrderIIR_high(float sample)
{
  //Coefficience 
  static const float a[4] = { 1. , -2.37409474, 1.92935567, -0.53207537};
  static const float b[4] = { 0.72944072, -2.18832217,  2.18832217, -0.72944072};

  // x array for holding recent inputs (newest input as index 0, delay of 1 at index 1, etc.
  static float x[4] = {0};
  // x array for holding recent inputs (newest input as index 0, delay of 1 at index 1, etc.
  static float y[4] = {0};

  x[0] = sample;

  // Calculate the output filtered signal based on a weighted sum of previous inputs/outputs
  y[0] = (b[0] * x[0] + b[1] * x[1] + b[2] * x[2] + b[3] * x[3]) - (a[1] * y[1] + a[2] * y[2] + a[3] * y[3]);
  y[0] /= a[0];

  // Shift the input signals by one timestep to prepare for the next call to this function
  x[3] = x[2];
  x[2] = x[1];
  x[1] = x[0];

  // Shift the previously calculated output signals by one time step to prepare for the next call to this function
  y[3] = y[2];
  y[2] = y[1];
  y[1] = y[0];

  return y[0];
}

float thirdOrderIIR_low(float sample)
{
  static const float a[4] = { 1.00000000e+00,  -2.77555756e-16, 3.33333333e-01, -1.85037171e-17};
  static const float b[4] = { 0.16666667,  0.5,  0.5, 0.16666667};

  // x array for holding recent inputs (newest input as index 0, delay of 1 at index 1, etc.
  static float x[4] = {0};
  // x array for holding recent inputs (newest input as index 0, delay of 1 at index 1, etc.
  static float y[4] = {0};

  x[0] = sample;

  // Calculate the output filtered signal based on a weighted sum of previous inputs/outputs
  y[0] = (b[0] * x[0] + b[1] * x[1] + b[2] * x[2] + b[3] * x[3]) - (a[1] * y[1] + a[2] * y[2] + a[3] * y[3]);
  y[0] /= a[0];

  // Shift the input signals by one timestep to prepare for the next call to this function
  x[3] = x[2];
  x[2] = x[1];
  x[1] = x[0];

  // Shift the previously calculated output signals by one time step to prepare for the next call to this function
  y[3] = y[2];
  y[2] = y[1];
  y[1] = y[0];

  return y[0];
}


void loop() {
}
