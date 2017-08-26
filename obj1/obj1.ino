#include "CurieTimerOne.h"
#define FREQUENCY 200  //Setup the target sampling frequency (Hz)
long initTime, count;
int EMGData;
void analogSampleIsr()   // callback function when interrupt is asserted
{
  EMGData = analogRead(A0);
  count++;
  Serial.print(count);
  Serial.print(" ");
  Serial.print(EMGData);
  Serial.print(" ");
  Serial.println(micros() - initTime);  //Calculate and print the time elasped after sampling begin
}

void setup() {
  //Setup for Analog input pins
  pinMode(A0, INPUT);
  Serial.begin(115200);  //Setup serial at 115200 baud
  while (!Serial);  //Wait until Serial Monitor is opened
  int period = 1000000 / FREQUENCY;  //Setup the sampling period
  CurieTimerOne.start(period, &analogSampleIsr);// set timer and callback to Analog Sampling
  initTime = micros();  //Get an initial time
}

void loop() {

}
