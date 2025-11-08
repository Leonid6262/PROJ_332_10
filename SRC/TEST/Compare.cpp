#include "Compare.hpp"

CCOMPARE::CCOMPARE():sync_f_comp(false), Us_f_comp(false)
{
  LPC_IOCON->P2_23 = IOCON_T3_CAP1;  // T3 CAP1    
  LPC_IOCON->P1_19 = IOCON_T1_CAP1;  // T1 CAP1   
    
  LPC_TIM3->CCR = TIM3_CAPTURE_RI;  //Захват T3 по фронту CAP1, с прерыванием без обработчика
  LPC_TIM1->CCR = TIM1_CAPTURE_RI;  //Захват T1 по фронту CAP1, с прерыванием без обработчика  
}

void CCOMPARE::start() 
{ 
  LPC_TIM3->IR = 0xFFFFFFFF;            //Очистка флагов прерываний 
  LPC_TIM1->IR = 0xFFFFFFFF;
  LPC_TIM3->TCR |= TIM3_TCR_START;      //Старт таймеров
  LPC_TIM1->TCR |= TIM1_TCR_START;
}

void CCOMPARE::test() 
{  
  //---По capture таймера 3 измеряется частота синхронизации-----
  unsigned int TIMER3_IRQ = LPC_TIM3->IR;
  LPC_TIM3->IR = 0xFFFFFFFF;
  
  if(TIMER3_IRQ & IRQ_CAP1)
  {      
    unsigned int CR1 = LPC_TIM3->CR1;
    unsigned int time_diff = CR1 - sync_time;
    sync_time = CR1;      
    sync_f = CCOMPARE::TIC_SEC / static_cast<float>(time_diff);                 
    sync_f_comp = true;
  }
  //–---–----------------------------------------------------------
  
  //---По capture таймера 1 измеряется частота напряжения статора-----
  unsigned int TIMER1_IRQ = LPC_TIM1->IR;
  LPC_TIM1->IR = 0xFFFFFFFF;
  
  if (TIMER1_IRQ & IRQ_CAP1)       
  {            
    unsigned int CR1 = LPC_TIM1->CR1;
    unsigned int time_diff = CR1 - Us_time;
    Us_time = CR1;
    Us_f = CCOMPARE::TIC_SEC / static_cast<float>(time_diff);            
    Us_f_comp = true;  
  }
  //–---–----------------------------------------------------------
 
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



  