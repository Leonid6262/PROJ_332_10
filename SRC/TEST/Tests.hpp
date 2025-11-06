#pragma once

#include "uart.hpp"
#include "can.hpp"
#include "dac.hpp"
#include "pause_us.hpp"

class CTESTS{
  
private: 
  
public:  
  
  CTESTS( 
         CUART&, 
         CUART&, 
         CCAN&, 
         CCAN&, 
         CDAC0&, 
         CDAC_PWM&, 
         CDAC_PWM&
           );
  
  CUART& rRS485_1;
  CUART& rRS485_2;
  CCAN& rCAN_1;
  CCAN& rCAN_2;
  CDAC0& rDAC0;
  CDAC_PWM& rPWM_DAC1;
  CDAC_PWM& rPWM_DAC2;
  
  unsigned char receive_RS485_1;
  unsigned char receive_RS485_2; 
  
  unsigned short CAN1_data_rx;
  unsigned short CAN2_data_rx;
  
  void testRS485();
  void testCAN();
  void testDAC0(signed short);  
  void testDAC1_PWM();
  void testDAC2_PWM();
  
};
