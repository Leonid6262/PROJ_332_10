#include "Tests.hpp"

CTESTS::CTESTS(
               CUART& rRS485_1, 
               CUART& rRS485_2, 
               CCAN& rCAN_1, 
               CCAN& rCAN_2, 
               CDAC0& rDAC0, 
               CDAC_PWM& rPWM_DAC1, 
               CDAC_PWM& rPWM_DAC2
                 ) : 
  rRS485_1(rRS485_1), 
  rRS485_2(rRS485_2), 
  rCAN_1(rCAN_1), 
  rCAN_2(rCAN_2), 
  rDAC0(rDAC0), 
  rPWM_DAC1(rPWM_DAC1), 
  rPWM_DAC2(rPWM_DAC2)
  {
    receive_RS485_1 = 0x01;
    receive_RS485_2 = 0x01; 
  }
  
void CTESTS::testDAC0()
{  
  static signed short data = CDAC0::DAC0_MIN_VAL;
  static unsigned int prev_TC0;
  
  unsigned int dTrs = LPC_TIM0->TC - prev_TC0; //Текущая дельта [0.1*mks]
  if(dTrs < 10000) return;  
  prev_TC0 = LPC_TIM0->TC;
  
  data++;
  if(data > CDAC0::DAC0_MAX_VAL) 
  {
    data = CDAC0::DAC0_MIN_VAL;
  }
  rDAC0.conv(data);
}

void CTESTS::testDAC1_PWM()
{  
  static signed short data = CDAC_PWM::_DAC_PWM_MIN_VAL;
  static unsigned int prev_TC0;
  
  unsigned int dTrs = LPC_TIM0->TC - prev_TC0; //Текущая дельта [0.1*mks]
  if(dTrs < 10000) return;  
  prev_TC0 = LPC_TIM0->TC;
  
  data++;
  if(data > CDAC_PWM::_DAC_PWM_MAX_VAL) 
  {
    data = CDAC_PWM::_DAC_PWM_MIN_VAL;
  }
  rPWM_DAC1.conv(data);
}

void CTESTS::testDAC2_PWM()
{    
  static signed short data = CDAC_PWM::_DAC_PWM_MAX_VAL;
  static unsigned int prev_TC0;
  
  unsigned int dTrs = LPC_TIM0->TC - prev_TC0; //Текущая дельта [0.1*mks]
  if(dTrs < 17000) return;  
  prev_TC0 = LPC_TIM0->TC;
  
  data--;
  if(data < CDAC_PWM::_DAC_PWM_MIN_VAL) data = CDAC_PWM::_DAC_PWM_MAX_VAL;
  rPWM_DAC2.conv(data);
}

void CTESTS::testRS485()
{  
  //loop test RS485  
  
  static char n_tr_485 = 1; 
  static unsigned int prev_TC0;  

  unsigned int dTrs = LPC_TIM0->TC - prev_TC0; //Текущая дельта [0.1*mks]
  if(dTrs < 5000000) return;
  prev_TC0 = LPC_TIM0->TC;

  switch(++n_tr_485 & 0x01)
  {
    
  case 0: //RS485-1
    receive_RS485_1 = rRS485_1.receive_char();    
    if(receive_RS485_1 > 0) 
    {
      receive_RS485_1++;
    }
    else 
    {
      receive_RS485_1 = 1;      
    }
    rRS485_1.transfer_char(receive_RS485_1);
    break;
    
  case 1: //RS485-2
    receive_RS485_2 = rRS485_2.receive_char();    
    if(receive_RS485_2 > 0) 
    {        
      receive_RS485_2++;
    }
    else 
    {
      receive_RS485_2 = 1;      
    }
    rRS485_2.transfer_char(receive_RS485_2);
    break;
    
  }
}

void CTESTS::testCAN()
{  
  //loop Test CAN
  
  static char n_tr_CAN = 1;
  static unsigned int prev_TC0;
  
  static unsigned short CAN1_data_tx;
  static unsigned short CAN2_data_tx;
  
  unsigned int dTrs = LPC_TIM0->TC - prev_TC0; //Текущая дельта [0.1*mks]
  if(dTrs < 5000000) return;  
  prev_TC0 = LPC_TIM0->TC;
  
  switch(++n_tr_CAN & 0x01)
  {
  case 0: //CAN-0
    CAN1_data_tx++;
    rCAN_1.transfer_short(CAN1_data_tx);
    Pause_us(15);
    CAN2_data_rx = rCAN_2.receive_short(); 
    break;
    
  case 1: //CAN-1
    CAN2_data_tx++;
    rCAN_2.transfer_short(CAN2_data_tx);
    Pause_us(15);
    CAN1_data_rx = rCAN_1.receive_short();  
    break;
  }  

}



