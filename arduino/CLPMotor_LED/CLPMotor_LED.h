//--------------------------------------
//hardware:
//MEGA 2560
//CLP MOTOR AND DRIVER
//function
//(1)CW (2)CCW
//use TIMER4  and TIMER5 to detect and create pulse
//CLPMotor driver(Hybird Servo Drive) Pulse needs to set 1600
//divide 1600,the smallest pulse is 8(5steps),so at least run once needs 5 steps
//go_step*pulseChange/10 
//5 * 16 /10=8step

//if there haven't thread,Please follow the steps listed below:
//1.Sketch->Include Library -> Manage Libraries
//2.type thread to search and install it
//--------------------------------------

#ifndef __CLPMOTOR_LED_H
#define __CLPMOTOR_LED_H

#include <Wire.h>
#include <CLP_MOTOR.h>
#include <Thread.h>
#include <ThreadController.h>

#define turn_therad_state true
#if turn_therad_state
ThreadController controll = ThreadController();
Thread  LED_BTN_thread = Thread();
Thread  MOTOR_thread = Thread();
#endif

#define turn_LED_BTN_thread_state false

//I2C setting
#define SLAVE_ADDRESS 0x04
struct I2C_get_data{
  int command;
  uint8_t number;
  uint8_t valueH;
  uint8_t valueL;
  uint8_t counter;  
}I2C_data;


//Button and LED pin and other setting
#define face_LED_R 29
#define face_LED_L 28
#define LED_BTN 25
#define CW_BTN 26
#define CCW_BTN 27
#define LED_value 128


//CLP STEP MOTOR setting
CLPMTR *CLPM_tester = new CLPMTR;
#define testerPUL  24
#define testerDIR  23
#define testerENA 22

//timer setting
unsigned int Timer4CountSet[10];
unsigned int Timer5CountSet[7];
#define timer5set 5

struct action_mode{
    uint8_t mode;
    enum{by_jog = 0, by_step};
};

struct  motor_control{
  bool arrive;
  bool DIR;      //0 CW ,1 CCW
  bool TimerSW;      //pulse high low change
  uint8_t set_speed;
  int pulse_count;
  uint8_t control_mode;
  int set_step;
  int set_pulse;
  enum {by_btn = 0, by_step, by_command};
  enum {CW = 0,CCW};
  enum {DISABLE = 0, ENABLE};
  struct action_mode act_mode;
} motor_set;

//-------------------------------
//test
int test_step = 500;
int test_pulse = test_step * 16 / 10;
bool run_once = 0;
//-------------------------------

void combine_command(struct I2C_get_data *input) { 
    int cul0 = input->valueH;
    cul0 = cul0 << 8;
    //Serial.println(cul0,HEX);
     int cul1 = input->valueL;
    //Serial.println(cul1,HEX);
    input->command = cul0+ cul1;
    /*
    Serial.print(F("receive number: "));
    Serial.println(input->number);
    Serial.print(F("receive command: DEC "));
    Serial.println(input->command );
    Serial.print(F("receive command: 0x"));
    Serial.println(input->command , HEX);     
    */
}

void LED_ON_OFF(uint8_t R, uint8_t L, bool state) {
  if( !state) {
   analogWrite(R, LED_value-5);    //the vaule at least needs 128
   analogWrite(L, LED_value-5);
   //Serial.println(F("LED OFF"));
  } else{
   analogWrite(R, LED_value);    //the vaule at least needs 128
   analogWrite(L, LED_value);
   //Serial.println(F("LED ON"));
  }
}

void save_data_from_I2C( struct I2C_get_data *input, uint8_t receive_data) {
            //Serial.print("receive_data: ");
            //Serial.println(receive_data);
            if(input->counter == 0) {
              input->number = receive_data;
              input->counter = 1;
               //Serial.print("number: ");
               //Serial.println(input->number);
            } else if (input->counter == 1) {
              input->valueH = receive_data;
             input->counter = 2;
              //Serial.print("valueH: ");
              //Serial.println(input->.valueH);
            } else if (input->counter == 2) {
              input->valueL = receive_data;
              input->counter = 3;
              //Serial.print("valueL: ");
              //Serial.println(input->valueL);
              //Serial.println("---------------------------");
            }
}

void I2C_data_initial(struct I2C_get_data *input){
    input->command = 0x00;
    input->number  = 0x00;
    input-> valueH = 0x00;
    input-> valueL = 0x00;
    input-> counter = 0x00;
}

 void motor_set_initial(struct  motor_control *input, uint8_t  mode){    
    input->arrive = true;
    input->DIR =  input->CW; 
    input->TimerSW = 0;      //pulse high low change
    input->set_speed = 6;
    input->pulse_count = 0;
    input->control_mode = mode;
    input->set_step = 0;
    input->set_pulse = 0;
    input->act_mode.mode = input->act_mode.by_jog;
}

uint8_t motor_speed_check( uint8_t  m_speed) {
   if( m_speed < 0 ) {
      m_speed = 0;
      return m_speed;
    }
    if( m_speed > 9) {
      m_speed = 9;
        return m_speed;
    }
    return m_speed;
}

#endif
