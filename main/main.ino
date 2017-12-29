#include <SoftwareSerial.h>

/**
* "Protocol: 
* OP(1) + Sensor number(2) + value (NN.DD (5))
* Ex: 
* R0000.00  = read sensor 00 value
* Response: OK+05.23

* W0004.35 = write the value 04.35 in the sensor 00
* Response: OK+ or ER- if a error was encountered
*/


SoftwareSerial BTSerial(10, 11); // RX | TX

#define MAX_SENSOR 4

#define FREQUENCY 9600

#define PIN 13

#define RESPONSE 8

#define S 2

#define V 5

#define READ 'R'

#define WRITE 'W'

#define WIDTH 5

#define DECIMALS 2

#define STATUS 3

#define OK "OK+"

#define ER "ER-"

float sensors[MAX_SENSOR]= {1,2,3,4};

char data;

char command[RESPONSE + 1];
  
char cmd;
  
int sensor;
  
float value;
  
char response[RESPONSE];

char s[S + 1];

char v[V + 1];

char ok[STATUS] = OK

char er[STATUS] = ER

void setup()
{
  pinMode(PIN, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  Serial.begin(FREQUENCY);
  BTSerial.begin(FREQUENCY);  // HC-05 default speed in AT command more
}

void loop()
{
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available() == RESPONSE) {
    for (int i = 0; i < RESPONSE; i++){
      command[i]= BTSerial.read();
      BTSerial.write(command[i]);
    }
    command[RESPONSE]= '\0';
    
    cmd  = command[0];

    for (int i = 0; i < S; i++) {
      s[i] = command[i + 1];
    }
    s[S] = '\0';
    
    sensor= atoi(s);

    for (int i = 0; i < V; i++) {
      v[i] = command[i + 3];
    }
    v[V]='\0';
    
    value= atof(v);
     
    if (cmd == READ) {
       value = sensors[sensor];
       dtostrf(value, WIDTH, DECIMALS, &response[STATUS]);

       for (int i = 0; i < STATUS; i++) {
        response[i] = ok[i];
       }
       
       for (int j = 0; j < RESPONSE; j++){
           BTSerial.write(response[j]);
       }
    } else if (cmd == WRITE){
        if (sensor < MAX_SENSOR) {
             sensors[sensor] = value;
             for (int i = 0; i < STATUS; i++) {
                response[i] = ok[i];
             }
        } else {
             for (int i = 0; i < STATUS; i++) {
                response[i] = er[i];
             }
        }
        for (int j = 0; j < STATUS; j++){
           BTSerial.write(response[j]);   
        }
    }
  }
  delay(1000);
}
