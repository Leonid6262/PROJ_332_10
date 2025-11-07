#pragma once

#include "LPC407x_8x_177x_8x.h"
#include "rem_osc.hpp"
#include "adc.hpp"

class CPULS
{ 

private:
 
  static constexpr unsigned int IOCON_P1_PWM = 0x03;                         //Тип портов - PWM
  static constexpr unsigned int PWM_div_0    = 60;                           //Делитель частоты
  static constexpr float pi = 3.141592653589793;
  
public:
  
  CPULS();

  static const char pulses[];
  
  bool forcing_bridge;
  bool main_bridge;
  
  unsigned char N_Pulse;
  
  signed short u_stator_1;
  signed short u_stator_2;
  unsigned int timing_stator_1;
  unsigned int timing_stator_2;
  unsigned short dTrs;
    
  char ind_d_avr;
  float u_stat[6]; 
  signed short u_stat_avr;
  
  void start();
  void calc(CADC*);

  static constexpr unsigned int PWM_WIDTH        = 10;                          //us
  static constexpr unsigned int PULSE_WIDTH      = 560;                         //us
  static constexpr unsigned int PULSE_PERIOD     = 3333;                        //us
  static constexpr unsigned int N_PULSES         = 6;
  static constexpr unsigned int OFF_PULSES       = 0x003F0000;                   //Импульсы в порту
  static constexpr unsigned int FIRS_PULS_PORT   = 16;                           //1-й импульс в порту 
  
  static constexpr unsigned int PCR_PWMENA1      = 0x200;
  static constexpr unsigned int PCR_PWMENA2      = 0x400;
  static constexpr unsigned int LER_012          = 0x07;
  static constexpr unsigned int COUNTER_CLR      = 0x00;
  static constexpr unsigned int COUNTER_RESET    = 0x02;
  static constexpr unsigned int COUNTER_STOP     = 0x0B;
  static constexpr unsigned int COUNTER_START    = 0x09;
  
  static constexpr unsigned int TIM2_TCR_START   = 0x01;

  static constexpr unsigned int TIM2_COMPARE_MR0 = 0x01;
  static constexpr unsigned int TIM2_COMPARE_MR1 = 0x08;
  
};
