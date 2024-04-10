
#include <Arduino.h>
#include <math.h>

#define DAC_PIN A4  // PA4
#define DAC_BITS 12
#define MAX_VALUE pow(2, DAC_BITS)
#define PAM_LEVELS 8
#define framerate 150
#define SC_PAM_K_PARAMETER 4
#define CALIBRATION_CYCLES 10
#define N_RANDOM_SYMBOLS 1250  //1250
#define N_ZEROS 10
#define N_ONES 10


float t_pulse = 4.0 / framerate * 1000;                         //in milliseconds
int numValues = 8;                                              // Number of values to generate
float base = 1.6;                                               // Base of the exponential function
float multiplier = MAX_VALUE / (pow(base, numValues - 1) - 1);  // Multiplier to scale the values
int exponentialValues[8];                                       // Array to store exponential values
int dacValue;
int random_index;
int transmission_no = 0;

void setup() {
  Serial.begin(115200);
  analogWriteResolution(DAC_BITS);
  //exponentialValues[0] = 0; // First value is set to 0
  for (int i = 0; i < numValues; i++) {
    float result = (pow(base, i) - 1) * multiplier;
    // Round the result to nearest integer
    int roundedResult = round(result);
    Serial.println(roundedResult);
    // Ensure the value doesn't exceed 12-bit limit
    if (roundedResult > 4095) {
      roundedResult = 4095;
    }
    exponentialValues[i] = roundedResult;
  }
}

void loop() {

  Serial.print("Transmission identifier: ");
  Serial.println(transmission_no);

  //ZEROS#define N_ZEROS 10
  for (int k = 0; k < N_ZEROS; k += 1) {
    analogWrite(DAC_PIN, 0);
    Serial.println(0);
    delay(t_pulse);
  }

  //ZEROS#define N_ONES 10
  for (int k = 0; k < N_ONES; k += 1) {
    dacValue = exponentialValues[7];
    analogWrite(DAC_PIN, dacValue);
    Serial.println(8);
    delay(t_pulse);
  }

  //CALIBRATION#define CALIBRATION_CYCLES 10
  for (int j = 0; j < CALIBRATION_CYCLES; j += 1) {
    for (int i = 0; i < PAM_LEVELS; i += 1) {
      //dacValue = i; // + pow(2,i);
      dacValue = exponentialValues[i];
      analogWrite(DAC_PIN, dacValue);
      Serial.println(i);
      delay(t_pulse);
    }
  }

  //DATA#define N_RANDOM_SYMBOLS 512

  // CP PAM (Othman Younus' Algorithm)
  for (int j = 0; j < N_RANDOM_SYMBOLS; j += 1) {
    random_index = random(PAM_LEVELS);
    dacValue = exponentialValues[random_index];
    analogWrite(DAC_PIN, dacValue);
    delay(t_pulse);
    Serial.println(random_index);
    dacValue = exponentialValues[PAM_LEVELS - random_index];
    analogWrite(DAC_PIN, dacValue);
    Serial.print(PAM_LEVELS - random_index);
    Serial.println("(SP)");
    delay(t_pulse);
  }

  /*
// CLASSIC PAM 
for (int j = 0; j<N_RANDOM_SYMBOLS; j +=1){
  random_index = random(PAM_LEVELS);
  dacValue = exponentialValues[random_index];
  analogWrite(DAC_PIN, dacValue);
  delay(t_pulse);
  Serial.println(random_index);
}

// SC-PAM (Monica Figueiredo's Algorithm)
for (int j = 0; j<N_RANDOM_SYMBOLS; j +=1){
  for (int level = 0; level<2; level+=1){
    for (int k=0; k<SC_PAM_K_PARAMETER; k+=1){
      random_index = random(int(PAM_LEVELS/2));
      dacValue = exponentialValues[abs(PAM_LEVELS*level-random_index)];
      analogWrite(DAC_PIN, dacValue);
      Serial.print(random_index);
      if (level == 0) Serial.println("(low)");
      else Serial.println("(high)");
      delay(t_pulse);
    }
  }
}

// CP PAM (Othman Younus' Algorithm)
for (int j = 0; j<N_RANDOM_SYMBOLS; j +=1){
  random_index = random(PAM_LEVELS);
  dacValue = exponentialValues[random_index];
  analogWrite(DAC_PIN, dacValue);
  delay(t_pulse);
  Serial.println(random_index);
  dacValue = exponentialValues[PAM_LEVELS-random_index];
  analogWrite(DAC_PIN, dacValue);
  Serial.print(PAM_LEVELS-random_index);
  Serial.println("(SP)");
  delay(t_pulse);
}
*/
  transmission_no += 1;
}
