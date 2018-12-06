#include "CLPMotor_LED.h"

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println(F("start"));
    CLPMTR_initial();
    Timer4_initial();
    Timer5_initial();
    LED_initial();
    LED_BTN_initial();
 #if turn_therad_on
    thread_initial(); 
 #endif  
 delay(3000);     //delay wait CLP motor and driver
}

void loop() {
 #if turn_therad_on
    controll.run();      
 #else if
 MOTOR_thread_Callback();
#endif
}


void LED_initial() {
    pinMode(face_LED_R,OUTPUT);
    pinMode(face_LED_L,OUTPUT);
    digitalWrite(face_LED_R,LOW);   //LED OFF
    digitalWrite(face_LED_L,LOW);   //LED OFF
}     

void LED_BTN_initial() {
    pinMode(LED_BTN, INPUT);     
    pinMode(LED_BTN, INPUT);     
    pinMode(LED_BTN, INPUT);     
}

 #if turn_therad_on
//thread_initial
void  thread_initial() {  
    LED_BTN_thread.onRun(LED_BTN_thread_Callback);
    LED_BTN_thread.setInterval(100);
    controll.add(&LED_BTN_thread);  // Add LED_BTN_thread to the controller
    MOTOR_thread.onRun(MOTOR_thread_Callback);
    MOTOR_thread.setInterval(100);
    controll.add(&MOTOR_thread);  // Add MOTOR_BTN_thread to the controller
}
#endif

void LED_BTN_thread_Callback() {
     bool LED_SW = digitalRead(LED_BTN);
    //true: no press, false:press
    if( LED_SW ) {
        digitalWrite(face_LED_R,LOW);   //LED OFF
        digitalWrite(face_LED_L,LOW);   //LED OFF
        //Serial.println(F("LED OFF"));
    } else {
        digitalWrite(face_LED_R,HIGH);   //LED ON
        digitalWrite(face_LED_L,HIGH);   //LED ON 
        //Serial.println(F("LED ON"));  
    }
}

void MOTOR_thread_Callback() {

    if(motor_set.control_mode == motor_set.by_btn) {
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
                motor_set.CW_CCW = 0; 
                CLPMTR_JogStepSet(motor_set.CW_CCW, 0);
             }
    }
}

//timer set and CLPM StepSet
void CLPMTR_JogStepSet(bool CW_CCW, bool CLPM_arrive) {
    //Serial.println(F( "CLPMTR_JogStepSet"));
    if (CW_CCW == 0) {    
         CLPM_tester->setCLPMTR_CW();        
    } else if (CW_CCW == 1) {    
         CLPM_tester->setCLPMTR_CCW();
    }
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

    motor_set.arrive = true;
    motor_set.CW_CCW = 0;      //0 CW ,1 CCW
    motor_set.TimerSW = 0;      //pulse high low change
    motor_set.set_speed = 6;
    motor_set.pulse_count = 0;
    motor_set.control_mode =   motor_set.by_step;
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
    if( motor_set.control_mode == motor_set.by_step) {
          if (motor_set.pulse_count == test_pulse)  {
               digitalWrite(face_LED_R,HIGH);   //LED ON
              digitalWrite(face_LED_L,HIGH);   //LED ON 
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


