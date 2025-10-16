#include "Compare.hpp"

CCOMPARE::CCOMPARE():sync_f_comp(false), Us_f_comp(false)
{
  LPC_IOCON->P2_23 = IOCON_T3_CAP1;  // T3 CAP1    
  LPC_IOCON->P1_19 = IOCON_T1_CAP1;  // T1 CAP1   
    
  LPC_TIM3->CCR = TIM3_CAPTURE_R; //Захват T3 по фронту CAP1, с прерыванием
  LPC_TIM1->CCR = TIM1_CAPTURE_R; //Захват T1 по фронту CAP1, с прерыванием  
}

void CCOMPARE::start() 
{ 
  LPC_TIM3->IR = 0xFFFFFFFF;            //Очистка флагов прерываний 
  LPC_TIM1->IR = 0xFFFFFFFF;
  LPC_TIM3->TCR |= TIM3_TCR_START;      //Старт таймеров
  LPC_TIM1->TCR |= TIM1_TCR_START;
  NVIC_EnableIRQ( TIMER3_IRQn );
  NVIC_EnableIRQ( TIMER1_IRQn );
}

void CCOMPARE::test() 
{  
  static unsigned int prev_TC0;
  
  unsigned int dTrs = LPC_TIM0->TC - prev_TC0; //Текущая дельта [0.1*mks]
  if(dTrs < _100ms) return;  
  prev_TC0 = LPC_TIM0->TC;
  if(sync_f_comp) 
  {
    SYNC_FREQUENCY = sync_f;
  }
  else
  {
    SYNC_FREQUENCY = 0;
    sync_f_comp = false;
  }
  if(Us_f_comp) 
  {
    STATOR_FREQUENCY = Us_f;
    Us_f_comp = false;  
  }
  else
  {
    STATOR_FREQUENCY = 0;
  } 
}



  