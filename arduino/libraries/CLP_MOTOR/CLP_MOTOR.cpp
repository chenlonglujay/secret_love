// For Arduino 1.0 and earlier
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <CLP_MOTOR.h>

//====================================
//建構
//====================================
CLPMTR::CLPMTR(void){
	DIRST=cw;
}
//==============================

//====================================
//解構
//====================================
CLPMTR::~CLPMTR(){

}
//==============================


//====================================
//Initial
//====================================
void CLPMTR::CLP_MOTOR_Initial(uint8_t PUL,uint8_t DIR){
	CLPMTR_DIR=DIR;
	CLPMTR_PUL=PUL;	
	pinMode(CLPMTR_DIR, OUTPUT);
	pinMode(CLPMTR_PUL, OUTPUT);
	digitalWrite(CLPMTR_DIR, LOW);  //CW 0/CCW 1	
	setCLPMTR_LOW();
	delay(10);
}
//=============================

//====================================
//Initial ALL
//====================================
void CLPMTR::CLP_MOTOR_Initial_all(uint8_t PUL,uint8_t DIR,uint8_t ENA){
	CLPMTR_DIR=DIR;
	CLPMTR_PUL=PUL;
	CLPMTR_ENA=ENA;
	pinMode(CLPMTR_DIR, OUTPUT);
	pinMode(CLPMTR_PUL, OUTPUT);	
	pinMode(CLPMTR_ENA, OUTPUT);
	setCLPMTR_Enable();
	delay(10);	
	digitalWrite(CLPMTR_DIR, LOW);  //CW 0/CCW 1
	setCLPMTR_LOW();
	delay(10);
}
//=============================

//====================================
//面對馬達 逆時針轉
//====================================
void  CLPMTR::setCLPMTR_Reverse() {
  digitalWrite(CLPMTR_DIR, LOW);   //CW 0/CCW 1
  DIRST=ccw;                    
}
//=============================

//====================================
//面對馬達 順時針轉
//====================================
void  CLPMTR::setCLPMTR_Forward(){
	digitalWrite(CLPMTR_DIR,HIGH);
	DIRST=cw;
}
//=============================

//====================================
//set Motor rotate CW
//====================================
boolean  CLPMTR::setCLPMTR_CW() {
	digitalWrite(CLPMTR_DIR, HIGH);   //CW 0/CCW 1
	return 0;                    
}
//=============================

//====================================
//set Motor rotate CCW
//====================================
boolean  CLPMTR::setCLPMTR_CCW(){
	digitalWrite(CLPMTR_DIR, LOW);
	return 1;
}
//=============================

//====================================
//SET CLP MOTOR set HIGH for pulse 
//give BJT B LOW then driver PUL get HIGH
//====================================
void  CLPMTR::setCLPMTR_HIGH(){
	digitalWrite(CLPMTR_PUL,LOW);
	
}
//=============================

//====================================
//SET CLP MOTOR set LOW for pulse
//give BJT B HIGH then driver PUL get LOW
//====================================
void CLPMTR::setCLPMTR_LOW(){
	digitalWrite(CLPMTR_PUL,HIGH);
	
}
//=============================

//====================================
//SET CLP MOTOR disable
//give BJT B HIGH then driver ENA get LOW
//====================================
void CLPMTR::setCLPMTR_Disable(){
	digitalWrite(CLPMTR_ENA,HIGH);	
}
//=============================

//====================================
//SET CLP MOTOR enable
//give BJT B LOW then driver ENA get HIGH
//====================================
void CLPMTR::setCLPMTR_Enable(){
	digitalWrite(CLPMTR_ENA,LOW);	
}
//=============================
//*===========================================*

