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
#include <CLP_MOTOR.h>

#include <Thread.h>
#include <ThreadController.h>
ThreadController controll = ThreadController();
Thread  LED_BTN_thread = Thread();
Thread  MOTOR_BTN_thread = Thread();

#define face_LED_R 38
#define face_LED_L 35
#define LED_BTN 24
#define CW_BTN 25
#define CCW_BTN 26

//CLP STEP MOTOR setting
CLPMTR *CLPM_tester = new CLPMTR;
#define testerPUL  23
#define testerDIR  22
bool CLPM_testerArrive = 0;
bool CLPM_CW_CCW = 0;      //0 CW ,1 CCW

//timer setting
bool TimerSW = 0;      //pulse high low change
unsigned int Timer4CountSet[10];
unsigned int Timer5CountSet[7];
uint8_t CLPMTRSpeed = 6;
#define timer5set 5

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println(F("start"));
    CLPMTR_initial();
    Timer4_initial();
    //Timer5_initial();
    LED_initial();
    LED_BTN_initial();
    thread_initial();  
}

void loop() {
    controll.run();        
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


//thread_initial
void  thread_initial() {  
  LED_BTN_thread.onRun(LED_BTN_thread_Callback);
  LED_BTN_thread.setInterval(100);
  controll.add(&LED_BTN_thread);  // Add LED_BTN_thread to the controller
  MOTOR_BTN_thread.onRun(MOTOR_BTN_thread_Callback);
  MOTOR_BTN_thread.setInterval(100);
  controll.add(&MOTOR_BTN_thread);  // Add MOTOR_BTN_thread to the controller
}

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

void MOTOR_BTN_thread_Callback() {
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
}


//CLP motor initial
void CLPMTR_initial() {
    CLPM_tester->CLP_MOTOR_Initial(testerPUL,testerDIR); 
    CLPM_tester->setCLPMTR_LOW();
    CLPM_CW_CCW = CLPM_tester->setCLPMTR_CW();
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
    TimerSW = true;
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
    //TIMSK5 = 0x01;       //timer5 start
}

//timer stop
void TimerStop() {
    TIMSK4 = 0x00;     //timer4 stop
    //TIMSK5 = 0x00;     //timer5 stop
}

//timer set and CLPM StepSet
void CLPMTR_JogStepSet(bool CW_CCW, uint8_t CLPM_arrive) {
    //Serial.println(F( "CLPMTR_JogStepSet"));
    if (CW_CCW == 0) {    
        CLPM_CW_CCW = CLPM_tester->setCLPMTR_CW();        
    } else if (CW_CCW == 1) {    
        CLPM_CW_CCW = CLPM_tester->setCLPMTR_CCW();
    }
    TCNT4 = Timer4CountSet[CLPMTRSpeed];
    //TCNT5 = Timer5CountSet[timer5set];
    CLPM_testerArrive = CLPM_arrive;
    TimerStart();
    //Serial.println(F( "CLPMTR_JogStepSet over"));
}

//timer4 interrput ISR
//create Pulse for CLPMOTOR driver
ISR (TIMER4_OVF_vect) {
    TIMSK4 = 0x00;     //timer4 stop
    TimerSW = !TimerSW;
    TCNT4 = Timer4CountSet[CLPMTRSpeed];    
    if (!CLPM_testerArrive) {
        if (TimerSW) {
            CLPM_tester->setCLPMTR_HIGH();      
        }  else {
            CLPM_tester->setCLPMTR_LOW();  
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
     
  if (!CLPM_testerArrive) {
    TIMSK5 = 0x01;       //timer5 start
  } else {
    TIMSK5 = 0x00;     //timer5 stop
  }
}


