#include <Wire.h>

#define SLAVE_ADDRESS 0x04
#define LED_R 38
#define LED_L 35
#define LED_value 128
int number = 0;
int state = 0;

struct I2C_get_data{
  uint8_t command;
  uint8_t valueH;
  uint8_t valueL;
  uint8_t counter;
}I2C_data;

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  Serial.println("Ready!");
}

void loop() {
  delay(2);
}

// callback for received data
void receiveData(int byteCount) {

  while (Wire.available()) {
    number = Wire.read();
    //Serial.print("data received: ");
    //Serial.println(number);
    if(I2C_data.counter == 0) {
      I2C_data.command = number;
      I2C_data.counter = 1;
       Serial.print("command: ");
       Serial.println(I2C_data.command);
    } else if (I2C_data.counter == 1) {
      I2C_data.valueH = number;
      I2C_data.counter = 2;
      Serial.print("valueH: ");
      Serial.println(I2C_data.valueH);
    } else if (I2C_data.counter == 2) {
      I2C_data.valueL = number;
      I2C_data.counter = 0;
      Serial.print("valueL: ");
      Serial.println(I2C_data.valueL);
      Serial.println("---------------------------");
    }

    
    if (number == 1) {

      if (state == 0) {
        digitalWrite(13, HIGH); // set the LED on
        analogWrite(LED_R, LED_value);    //the vaule at least needs 128
        analogWrite(LED_L, LED_value);
        state = 1;
      }
      else {
        digitalWrite(13, LOW); // set the LED off
        analogWrite(LED_R, LED_value-5);    //the vaule at least needs 128
        analogWrite(LED_L, LED_value-5);
        state = 0;
      }
    }
  }
}

// callback for sending data
void sendData() {
  Wire.write(number);
}

void I2C_data_init(struct I2C_get_data *input){
  input->command = 0x00;
  input-> valueH = 0x00;
  input-> valueL = 0x00;
  input-> counter = 0x00;
}

