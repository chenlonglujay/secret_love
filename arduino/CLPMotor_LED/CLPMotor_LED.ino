#include "CLPMotor_LED.h"

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println(F("start"));
    Wire.begin(SLAVE_ADDRESS);
    I2C_initial();
    CLPMTR_initial();
    Timer4_initial();
    Timer5_initial();
    LED_initial();
    LED_BTN_initial();
 #if turn_therad_state
    thread_initial(); 
 #endif  
 delay(3000);     //delay wait for CLP motor and driver
}

void loop() {
 #if turn_therad_state
    controll.run();      
 #else if
  MOTOR_thread_Callback();
  LED_BTN_thread_Callback();
#endif
}


void LED_initial() {
    LED_ON_OFF(face_LED_R, face_LED_L, 0);
}     

void LED_BTN_initial() {
    pinMode(LED_BTN, INPUT);     
    pinMode(LED_BTN, INPUT);     
    pinMode(LED_BTN, INPUT);     
}

 #if turn_therad_state
//thread_initial
void  thread_initial() {  
 
 #if turn_LED_BTN_thread_state  
    LED_BTN_thread.onRun(LED_BTN_thread_Callback);
    LED_BTN_thread.setInterval(100);
    controll.add(&LED_BTN_thread);  // Add LED_BTN_thread to the controller
#endif    
    MOTOR_thread.onRun(MOTOR_thread_Callback);
    MOTOR_thread.setInterval(100);
    controll.add(&MOTOR_thread);  // Add MOTOR_BTN_thread to the controller
}
#endif

void I2C_initial() {
    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    //Wire.onRequest(sendData);  
    I2C_data_initial(&I2C_data);
}

void LED_BTN_thread_Callback() {
     bool LED_SW = digitalRead(LED_BTN);
    //true: no press, false:press
     LED_ON_OFF(face_LED_R, face_LED_L, !LED_SW);
}

void MOTOR_thread_Callback() {
    if (motor_set.control_mode == motor_set.by_command) {
          if( I2C_data.counter == 3) {
            combine_command(&I2C_data);
            judge_command(&I2C_data);
          }
    } else if (motor_set.control_mode == motor_set.by_btn) {
          bool  CW =   digitalRead(CW_BTN);     
          bool  CCW =   digitalRead(CCW_BTN);        
          if( CW && CCW ||  !CW && !CCW) {
              // both button has no press or both has press
              CLPMTR_JogStepSet( 0, 1);      //stop motor
          } else if (!CW && CCW) {
              CLPMTR_JogStepSet( 0, 0);    
          } else if (!CCW && CW) {
              CLPMTR_JogStepSet( 1, 0);
          }
    } else if (motor_set.control_mode == motor_set.by_step) {
             if (motor_set.arrive && run_once==0){
                Serial.println(F("motor run"));  
                motor_set.set_speed = 6;
                motor_set.DIR = 0; 
                CLPMTR_JogStepSet(motor_set.DIR, 0);
             }
    }
}

//timer set and CLPM StepSet
void CLPMTR_JogStepSet(bool CW_CCW, bool CLPM_arrive) {
    //Serial.println(F( "CLPMTR_JogStepSet"));
    set_motor_DIR(CW_CCW);   
    TCNT4 = Timer4CountSet[motor_set.set_speed];
    TCNT5 = Timer5CountSet[timer5set];
   motor_set.arrive = CLPM_arrive;
    TimerStart();
    //Serial.println(F( "CLPMTR_JogStepSet over"));
}

//CLP motor initial
void CLPMTR_initial() {
    CLPM_tester->CLP_MOTOR_Initial(testerPUL,testerDIR); 
    CLPM_tester->setCLPMTR_LOW();
    CLPM_tester->setCLPMTR_CW();
    motor_set_initial(&motor_set, motor_set.by_command);
}


//timer4 initial
void Timer4_initial() {
    Timer4CountSet[0] = 65531; //0.02ms 20us中斷設定
    Timer4CountSet[1] = 65524; //0.048ms 48us中斷設定
    Timer4CountSet[2] = 65511; //0.1ms 100us中斷設定
    Timer4CountSet[3] = 65473; //0.25ms 250us中斷設定
    Timer4CountSet[4] = 65411; //0.5ms 500us中斷設定
    Timer4CountSet[5] = 65286; //1ms 1000us中斷設定
    Timer4CountSet[6] = 65036; //2ms 2000us中斷設定
    Timer4CountSet[7] = 63036; //10ms 中斷設定
    Timer4CountSet[8] = 53036; //50ms 中斷設定
    Timer4CountSet[9] = 40536; //100ms 中斷設定
    TCCR4A =  0x00;
    TCCR4B =  0X03;          //設定 除頻=3 16Mhz/64=0.25Mhz
    //1/0.25Mhz=4us,每4us計數一次,假設設定為65531,
    //共65536-65531=5,第5次發生計時中斷,共經時間5*4us=20us
    TCCR4C =  0x00;
    TIMSK4 =  0x00;      //timer4 stop
}

//timer5 initial
void Timer5_initial() {
    Timer5CountSet[0] = 65531; //20us中斷設定
    Timer5CountSet[1] = 65521;    //0.06ms 60us中斷設定
    Timer5CountSet[2] = 65511; //0.1ms 100us中斷設定
    Timer5CountSet[3] = 65473;  //0.25ms 250us中斷設定
    Timer5CountSet[4] = 65411;  //0.5ms 500us中斷設定
    Timer5CountSet[5] = 65286;   //1ms 1000us中斷設定
    Timer5CountSet[6] = 65036;   //2ms 2000us中斷設定
    TCCR5A =  0x00;
    TCCR5B =  0X03;          //除頻=5 16Mhz/1024 除頻=3 16Mhz/64
    TCCR5C =  0x00;
    TIMSK5 =  0x00;      //timer5 stop
}


//timer start
void TimerStart() {
    TIMSK4 = 0x01;       //timer4 start
    TIMSK5 = 0x01;       //timer5 start
}

//timer stop
void TimerStop() {
    TIMSK4 = 0x00;     //timer4 stop
    TIMSK5 = 0x00;     //timer5 stop
}

//timer4 interrput ISR
//create Pulse for CLPMOTOR driver
ISR (TIMER4_OVF_vect) {
    TIMSK4 = 0x00;     //timer4 stop
     motor_set.TimerSW = ! motor_set.TimerSW;
    TCNT4 = Timer4CountSet[motor_set.set_speed];    
    if (!motor_set.arrive) {
        if ( motor_set.TimerSW) {
            CLPM_tester->setCLPMTR_HIGH();      
        }  else {
            CLPM_tester->setCLPMTR_LOW();  
             motor_set.pulse_count = motor_set.pulse_count + 1;
        }
        TIMSK4 = 0x01;       //timer4 start
    } else {
        CLPM_tester->setCLPMTR_LOW(); 
        TIMSK4 = 0x00;     //timer4 stop
    }
}

//timer5 interrput ISR
ISR (TIMER5_OVF_vect) {
    TIMSK5 = 0x00;     //timer5 stop
    TCNT5 = Timer5CountSet[timer5set];  
    if(motor_set.control_mode == motor_set.by_command) {
        if(motor_set.act_mode.mode == motor_set.act_mode.by_step) {     
                  if (motor_set.pulse_count >= motor_set.set_pulse)  {     
                      motor_set.pulse_count = 0;
                       motor_set.arrive = 1;    
                    } 
          }
    } else if( motor_set.control_mode == motor_set.by_step) {
          if (motor_set.pulse_count == test_pulse)  {            
             motor_set.arrive = 1;     
             run_once = 1;     
             motor_set.pulse_count = 0;
          }
    }
    if (!motor_set.arrive) {
      TIMSK5 = 0x01;       //timer5 start
    } else {
      TIMSK5 = 0x00;     //timer5 stop
    }
}


// callback for received data
void receiveData(int byteCount) {
  while (Wire.available()) {
             uint8_t receive_data = Wire.read();
             save_data_from_I2C(&I2C_data, receive_data);          
    }   
}

void judge_command(struct I2C_get_data *input) {
      switch (input->number) {
       case 0x00:
          //nothing          
       break;
       case 0x01:
          //motor direction CW or CCW
          set_motor_DIR(bool(input->command));
       break;
      case 0x02:
          //motor rotate step
          motor_set.set_step = input->command;
          motor_set.set_pulse = motor_set.set_step * 16 /10;
          //Serial.print(F("motor_set.set_pulse :  "));
          //Serial.println(motor_set.set_pulse );
          motor_set.act_mode.mode = motor_set.act_mode.by_step;
          CLPMTR_JogStepSet(motor_set.DIR , 0);
       break;
       case 0x03:
       {
          motor_set.act_mode.mode = motor_set.act_mode.by_jog;
          CLPMTR_JogStepSet(motor_set.DIR , 0);
       }
       break;
       case 0x04:
          //motor stop
          motor_set.pulse_count = 0;
          motor_set.set_pulse = 0;
          TimerStop();          
       break;
        case 0x05:
        {
          //motor speed
          uint8_t sp = (uint8_t)input->command;
          sp = motor_speed_check(sp);
          motor_set.set_speed = sp;
        }
       break;
       case 0x06:
       {
          //LED ON/OFF
            bool LED_SW= (bool)input->command; 
            LED_ON_OFF(face_LED_R, face_LED_L, LED_SW);
       }
       break;
      };    
         I2C_data_initial(&I2C_data);     
}

void set_motor_DIR(bool DIR) {
    if (DIR == motor_set.CW) {    
         motor_set.DIR = CLPM_tester->setCLPMTR_CW();      
    } else if (DIR == motor_set.CCW) {    
         motor_set.DIR= CLPM_tester->setCLPMTR_CCW();
    }
}

/*
// callback for sending data
void sendData() {
  Wire.write(number);
}*/



