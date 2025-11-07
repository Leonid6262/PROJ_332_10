#include "Puls.hpp"
#include <cmath>
#include "system_LPC177x.h"

const char CPULS::pulses[] = {0x03, 0x06, 0x0C, 0x18, 0x30, 0x21};

CPULS::CPULS()
{  
  LPC_SC->PCONP   |= CLKPWR_PCONP_PCPWM0;       //PWM0 power/clock control bit.
  LPC_PWM0->PR     = PWM_div_0 - 1;             //при PWM_div=60, F=60МГц/60=1МГц, 1тик=1мкс       
  
  LPC_PWM0->TCR    = COUNTER_CLR;               //Сброс регистра таймера
  LPC_PWM0->TCR    = COUNTER_RESET;             //Сброс таймера 
  
  ind_d_avr = 0;
}

void CPULS::calc(CADC* pAdc) 
{  
  u_stator_2 = pAdc->data[CADC::STATOR_VOLTAGE];
  timing_stator_2 = pAdc->timings[CADC::STATOR_VOLTAGE + 1];
  
  unsigned int us1us1  =  u_stator_1 * u_stator_1;
  unsigned int us2us2  =  u_stator_2 * u_stator_2;
  signed int   us1us2  =  u_stator_1 * u_stator_2;

  dTrs = timing_stator_2 - timing_stator_1;
  
  u_stator_1 = u_stator_2;
  timing_stator_1 = timing_stator_2;
  
  float theta = (2.0f * pi * 50.0 * dTrs) / 1000000.0f;
  
  float cos = std::cos(theta);
  float sin = std::sin(theta);
  
  ind_d_avr++;
  if(ind_d_avr > 5) ind_d_avr = 0;
  u_stat[ind_d_avr] = sqrt(((us1us1 + us2us2) - (us1us2 * 2 * cos)) / (sin * sin));
  
  u_stat_avr = round((u_stat[0] + u_stat[1] + u_stat[2] + u_stat[3] + u_stat[4] + u_stat[5]) / 6.0f);

}

void CPULS::start() 
{  
  forcing_bridge = false;
  main_bridge    = false;
  
  N_Pulse = 1;

  LPC_PWM0->PCR       = 0x00;           //Отключение PWM0 
  LPC_PWM0->MR0       = PWM_WIDTH * 2;  //Период ШИМ. MR0 - включение
  LPC_PWM0->MR1       = PWM_WIDTH;      //Выключение PWM0:1 по MR1
  LPC_PWM0->MR2       = PWM_WIDTH;      //Выключение PWM0:2 по MR2
  
  LPC_PWM0->LER       = LER_012;        //Обновление MR0,MR1 и MR2
  LPC_PWM0->TCR       = COUNTER_STOP;   //Включение PWM. Счётчик - стоп
  LPC_PWM0->TCR       = COUNTER_RESET;
  
  LPC_TIM2->MCR  = 0x00000000;          //Compare TIM3 с MR0 и MR1, с прерываниями (disabled)
  LPC_TIM2->IR   = 0xFFFFFFFF;          //Очистка флагов прерываний  
  LPC_TIM2->TCR |= TIM2_TCR_START;      //Старт таймера TIM2
  LPC_TIM2->MR0 = LPC_TIM2->TC + PULSE_PERIOD;
  LPC_TIM2->MR1 = LPC_TIM2->TC + PULSE_WIDTH;    
  LPC_TIM2->MCR = TIM2_COMPARE_MR1;     //Compare TIM2 с MR1- enabled
  
  NVIC_EnableIRQ(TIMER2_IRQn);
  
}


