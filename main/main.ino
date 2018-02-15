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
| OP_IDS:           L (Lower threshold update), U (Upper threshold update)
|
| Example:          ' L010050' --> Update lower threshold of sensor 01 to 00.50
| Example:          ' U000350' --> Update upper threshold of sensor 00 to 03.50
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

#define ER '-'

#define OP_UPDATE_SENSOR 'S'

#define OP_UPDATE_LOWER_THRESHOLD 'L'

#define OP_UPDATE_UPPER_THRESHOLD 'U'

const int sensors[MAX_SENSOR]= {0,1,2,3};

char sensor[SENSOR_LENGTH + 1];

char command[RESPONSE_LENGTH + 1];

float value;

char formatted_value[INTEGER_LENGTH + DECIMALS_LENGTH + 1];

char response[RESPONSE_LENGTH];

void setup()
{
  pinMode(PIN, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  Serial.begin(FREQUENCY);
  BTSerial.begin(FREQUENCY);  // HC-05 default speed in AT command more
}

void loop()
{
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available() == RESPONSE_LENGTH) {
    for (int i = 0; i < RESPONSE_LENGTH; i++){
      command[i]= BTSerial.read();
    }
    command[RESPONSE_LENGTH]= '\0';
    // TODO something with incoming command
  }

  for (int i = 0; i < MAX_SENSOR; i++) {
    response[0] = OK_STATUS;
    response[STATUS_LENGTH] = OP_UPDATE_SENSOR;
    
    sprintf(sensor, "%02d", sensors[i]);                                                                
    for (int j = 0; j < SENSOR_LENGTH; j++) {
      response[STATUS_LENGTH + OPERATION_LENGTH + j] = sensor[j];
    }
    
    value = random(0, 500) / 100.0;
    dtostrf(value, INTEGER_LENGTH + DECIMALS_LENGTH + 1, DECIMALS_LENGTH, formatted_value);

    for (int j = 0; j < INTEGER_LENGTH; j++) {
      if (formatted_value[j] == ' ') {
        response[STATUS_LENGTH + OPERATION_LENGTH + SENSOR_LENGTH + j] = '0';
      } else {
        response[STATUS_LENGTH + OPERATION_LENGTH + SENSOR_LENGTH + j] = formatted_value[j];
      }
    }
    for (int j = 0; j < DECIMALS_LENGTH; j++) {
       response[STATUS_LENGTH + OPERATION_LENGTH + SENSOR_LENGTH + INTEGER_LENGTH + j] = formatted_value[INTEGER_LENGTH + 1 + j];
    }
    
    for (int j = 0; j < RESPONSE_LENGTH; j++){
      BTSerial.write(response[j]);
//      Serial.println(response[j]);
    }
  }
  
  delay(1000);
}
