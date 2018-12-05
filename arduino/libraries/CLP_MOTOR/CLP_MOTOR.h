//程式名稱:閉迴路步進馬達 控制實驗
//版本：V1.4
//配合硬體:HB860H驅動器
//配合硬體:閉迴路步進馬達
//配合硬體:TIP120 電晶體控制電路
//驗證Y/N:N
//備註：
//使用Atmel ATmega 2560 TIMER撰寫PULSE
//設定pulse 15(15us)速度最快，不可低於15
//設定65531為20us中斷一次
//設定按下1次按鈕 馬達跑CLP_StepSet個步級
//驅動器設定為1600個pulse為一圈
//新增ENABLE控制
//修改錯誤Pulse HI LOW控制BJT原本相反
//程式編寫日期：2017/10/29(日)
#ifndef CLP_MOTOR_h
#define CLP_MOTOR_h
#include <inttypes.h>

class CLPMTR
{
private:  
  uint8_t CLPMTR_PUL;
  uint8_t CLPMTR_DIR;  
  uint8_t CLPMTR_ENA;  
public:    
  CLPMTR();    //建構
  ~CLPMTR();       //解構
  //boolean cw_ccw;
  uint8_t DIRST;
  enum DIR{cw=1,ccw};
  void CLP_MOTOR_Initial(uint8_t PUL,uint8_t DIR);
  void CLP_MOTOR_Initial_all(uint8_t PUL,uint8_t DIR,uint8_t ENA);
  void setCLPMTR_Forward();
  void setCLPMTR_Reverse();  
  boolean setCLPMTR_CW();
  boolean setCLPMTR_CCW();
  void setCLPMTR_HIGH();
  void setCLPMTR_LOW();
  void setCLPMTR_Enable();
  void setCLPMTR_Disable();	
};

#endif
