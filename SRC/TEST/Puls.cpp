#include "Puls.hpp"
#include "system_LPC177x.h"

const char CPULS::pulses[] = {0x03, 0x06, 0x0C, 0x18, 0x30, 0x21};

CPULS::CPULS()
{
  LPC_SC->PCONP   |= CLKPWR_PCONP_PCPWM0;       //PWM0 power/clock control bit.
  LPC_PWM0->PR     = PWM_div_0 - 1;             //при PWM_div=60, F=60МГц/60=1МГц, 1тик=1мкс       
  
  LPC_PWM0->TCR    = COUNTER_CLR;               //Сброс регистра таймера
  LPC_PWM0->TCR    = COUNTER_RESET;             //Сброс таймера 
}

void CPULS::start() 
{  
  forcing_bridge = false;
  main_bridge    = false;
  
  N_Pulse = 1;
  
  LPC_TIM2->MCR  = 0x00000000;          //Compare TIM3 с MR0 и MR1, с прерываниями (disabled)
  LPC_TIM2->IR   = 0xFFFFFFFF;          //Очистка флагов прерываний  
  LPC_TIM2->TCR |= TIM2_TCR_START;      //Старт таймера TIM2
  
  LPC_IOCON->P1_2     = IOCON_P1_PWM;   //P1_2->PWM0:1 (SUM-1) 
  LPC_IOCON->P1_3     = IOCON_P1_PWM;   //P1_3->PWM0:2 (SUM-2) 
  LPC_PWM0->PCR       = 0x00;           //Отключение PWM0 
  LPC_PWM0->MR0       = PWM_WIDTH * 2;  //Период ШИМ. MR0 - включение
  LPC_PWM0->MR1       = PWM_WIDTH;      //Выключение PWM0:1 по MR1
  LPC_PWM0->MR2       = PWM_WIDTH;      //Выключение PWM0:2 по MR2
  
  LPC_PWM0->LER       = LER_012;        //Обновление MR0,MR1 и MR2
  LPC_PWM0->TCR       = COUNTER_STOP;   //Включение PWM. Счётчик - стоп
  
  LPC_TIM2->MR1 = LPC_TIM2->TC + PULSE_WIDTH;    
  LPC_TIM2->MCR = TIM2_COMPARE_MR1;     //Compare TIM2 с MR1- enabled
  
  NVIC_EnableIRQ(TIMER2_IRQn);
  
}


