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

#include <CLP_MOTOR.h>
#include <Thread.h>
#include <ThreadController.h>

#define turn_therad_on true
#if turn_therad_on
ThreadController controll = ThreadController();
Thread  LED_BTN_thread = Thread();
Thread  MOTOR_thread = Thread();
#endif

#define face_LED_R 38
#define face_LED_L 35
#define LED_BTN 24
#define CW_BTN 25
#define CCW_BTN 26

//CLP STEP MOTOR setting
CLPMTR *CLPM_tester = new CLPMTR;
#define testerPUL  23
#define testerDIR  22


//timer setting
unsigned int Timer4CountSet[10];
unsigned int Timer5CountSet[7];
#define timer5set 5

struct  motor_control{
  bool arrive;
  bool CW_CCW;      //0 CW ,1 CCW
  bool TimerSW;      //pulse high low change
  uint8_t set_speed;
  int pulse_count;
  uint8_t control_mode;
  enum {by_btn = 0, by_step, by_command};
} motor_set;

int test_step = 500;
int test_pulse = test_step * 16 / 10;
bool run_once = 0;
#endif
