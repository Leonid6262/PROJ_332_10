#include "Timers_init.hpp"
#include "IntPriority.hpp" 
#include "system_LPC177x.h"
#include "LPC407x_8x_177x_8x.h"
   
void Timers_Initializing()
{ 
  // Включение clock
  LPC_SC->PCONP |= CLKPWR_PCONP_PCTIM0;
  LPC_SC->PCONP |= CLKPWR_PCONP_PCTIM1;
  LPC_SC->PCONP |= CLKPWR_PCONP_PCTIM2;
  LPC_SC->PCONP |= CLKPWR_PCONP_PCTIM3;

  // Частота
  LPC_TIM0->PR  =  6 - 1; //при PR= 5,  F=60МГц/6  = 10МГц,   1тик=0.1мкс
  LPC_TIM1->PR  = 60 - 1; //при PR=59,  F=60МГц/60 =  1МГц,   1тик=1.0мкс
  LPC_TIM2->PR  = 60 - 1; //при PR=59,  F=60МГц/60 =  1МГц,   1тик=1.0мкс
  LPC_TIM3->PR  = 60 - 1; //при PR=59,  F=60МГц/60 =  1МГц,   1тик=1.0мкс
  
  // Очистка флагов прерываний
  LPC_TIM1->IR = 0xFFFFFFFF;
  LPC_TIM2->IR = 0xFFFFFFFF;
  LPC_TIM3->IR = 0xFFFFFFFF;
     
  NVIC_SetPriority( TIMER1_IRQn, Priorities::Timer1 );
  NVIC_SetPriority( TIMER2_IRQn, Priorities::Timer2 );
  NVIC_SetPriority( TIMER3_IRQn, Priorities::Timer3 );
   
  // Включение TIM0
  LPC_TIM0->TCR |= 0x1; 

}



