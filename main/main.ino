#include <SoftwareSerial.h>

/*
|--------------------------------------------------------------------------
| Protocol 8 bytes
|--------------------------------------------------------------------------
|
| Incoming message: [STATUS(1)][OP_ID(1)][SENSOR(2)][INTEGER(2)][DECIMAL(2)]
| OP_IDS:           S (Sensor update), L (Lower threshold update), U (Upper threshold update)
| STATUS:           + (Success), - (Error)
|
| Example:          '+S000322' --> Sensor 00 responded with success and value 03.22
| Example:          '-S010000' --> Sensor 01 responded with error
| Example:          '+L010100' --> Lower threshold of sensor 01 updated to 01.00 with success
| Example:          '-U000000' --> Upper threshold of sensor 00 cannot be updated
|
| Outgoing message: [ANY(1)][OP_ID(1)][SENSOR(2)][INTEGER(2)][DECIMAL(2)]
| OP_IDS:           L (Lower threshold update), U (Upper threshold update), R (Refresh sensors thresholds)
|
| Example:          ' L010050' --> Update lower threshold of sensor 01 to 00.50
| Example:          ' U000350' --> Update upper threshold of sensor 00 to 03.50
| Example:          ' R000000' --> Sync all sensors thresholds
|
*/

SoftwareSerial BTSerial(10, 11); // RX | TX

#define MAX_SENSOR 4

#define FREQUENCY 9600

#define PIN 13

#define RESPONSE_LENGTH 8

#define SENSOR_LENGTH 2

#define INTEGER_LENGTH 2

#define DECIMALS_LENGTH 2

#define OPERATION_LENGTH 1

#define STATUS_LENGTH 1

#define OK_STATUS '+'

#define ER_STATUS '-'

#define OP_UPDATE_SENSOR 'S'

#define OP_UPDATE_LOWER_THRESHOLD 'L'

#define OP_UPDATE_UPPER_THRESHOLD 'U'

#define OP_REFRESH_THRESHOLDS 'R'

#define MIN_VOLTAGE 0

#define MAX_VOLTAGE 5

#define PRECISION 20

#define INCOMING_COMMAND_START 1

const int sensors[MAX_SENSOR]= {0,1,2,3};

char command[RESPONSE_LENGTH + 1];

int sensors_lower_thresholds[MAX_SENSOR];

int sensors_upper_thresholds[MAX_SENSOR];

char response[RESPONSE_LENGTH];

char formatted_sensor[SENSOR_LENGTH + 1];

char formatted_value[INTEGER_LENGTH + DECIMALS_LENGTH + 1];

float convert_integer_to_voltage (int value) {
  return (value * MAX_VOLTAGE) / PRECISION;
}

void setup()
{
  pinMode(PIN, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  Serial.begin(FREQUENCY);
  BTSerial.begin(FREQUENCY);  // HC-05 default speed in AT command more

  for (int i = 0; i < MAX_SENSOR; i++) { // TODO: Ler da EPROM
    sensors_lower_thresholds[i] = random(1, 5); 
    sensors_upper_thresholds[i] = random(15, 20);
  }
}

static void build_response (char status_code, char operation, int sensor, float value) {
  response[0] = status_code;
  response[STATUS_LENGTH] = operation;
  sprintf(formatted_sensor, "%02d", sensor);                                                                
  for (int i = 0; i < SENSOR_LENGTH; i++) {
    response[STATUS_LENGTH + OPERATION_LENGTH + i] = formatted_sensor[i];
  }
  dtostrf(value, INTEGER_LENGTH + DECIMALS_LENGTH + 1, DECIMALS_LENGTH, formatted_value);
  for (int i = 0; i < INTEGER_LENGTH; i++) {
    if (formatted_value[i] == ' ') {
      response[STATUS_LENGTH + OPERATION_LENGTH + SENSOR_LENGTH + i] = '0';
    } else {
      response[STATUS_LENGTH + OPERATION_LENGTH + SENSOR_LENGTH + i] = formatted_value[i];
    }
  }
  for (int i = 0; i < DECIMALS_LENGTH; i++) {
     response[STATUS_LENGTH + OPERATION_LENGTH + SENSOR_LENGTH + INTEGER_LENGTH + i] = formatted_value[INTEGER_LENGTH + 1 + i];
  }
}

void loop()
{
  if (BTSerial.available() == RESPONSE_LENGTH) {
    for (int i = 0; i < RESPONSE_LENGTH; i++){
      command[i]= BTSerial.read();
    }
    
    command[RESPONSE_LENGTH]= '\0';

    if (command[INCOMING_COMMAND_START] == OP_REFRESH_THRESHOLDS) {
      for (int i = 0; i < MAX_SENSOR; i++) {
        build_response(OK_STATUS, OP_UPDATE_LOWER_THRESHOLD, i, convert_integer_to_voltage(sensors_lower_thresholds[i]));
        for (int j = 0; j < RESPONSE_LENGTH; j++){
          BTSerial.write(response[j]);
        }
        build_response(OK_STATUS, OP_UPDATE_LOWER_THRESHOLD, i, convert_integer_to_voltage(sensors_upper_thresholds[i]));
        for (int j = 0; j < RESPONSE_LENGTH; j++){
          BTSerial.write(response[j]);
        }
      }
    }
  }

  for (int i = 0; i < MAX_SENSOR; i++) {
    build_response(OK_STATUS, OP_UPDATE_SENSOR, i, random(1, 500) / 100.0);
    for (int j = 0; j < RESPONSE_LENGTH; j++){
      BTSerial.write(response[j]);
    }
  }
  
  delay(1000);
}
