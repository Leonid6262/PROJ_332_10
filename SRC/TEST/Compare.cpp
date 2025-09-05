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

// --- Singleton Майерса ---
CCOMPARE& CCOMPARE::getInstance() 
{
  // Инициализация в момент первого вызова
  static CCOMPARE instance; 
  return instance;
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
  }
  if(Us_f_comp) 
  {
    STATOR_FREQUENCY = Us_f;
  }
  else
  {
    STATOR_FREQUENCY = 0;
  }
  sync_f_comp = false;
  Us_f_comp = false;  
}

extern "C" 
{
  void TIMER3_IRQHandler( void )
  {   
    unsigned short TIMER3_IRQ = LPC_TIM3->IR;
    LPC_TIM3->IR = 0xFFFFFFFF;
    
    if (TIMER3_IRQ & CCOMPARE::IRQ_CAP1)       //Прерывание T3 по CAP1 (Sync)
    {            
      CCOMPARE& rCompare = CCOMPARE::getInstance();
      unsigned int time_diff = LPC_TIM3->TC - rCompare.sync_time;      
      rCompare.sync_f = CCOMPARE::TIC_SEC / static_cast<float>(time_diff);           
      rCompare.sync_time = LPC_TIM3->TC;
      rCompare.sync_f_comp = true;
    }
  }
  
  void TIMER1_IRQHandler( void )
  {   
    unsigned short TIMER1_IRQ = LPC_TIM1->IR;
    LPC_TIM1->IR = 0xFFFFFFFF;
    
    if (TIMER1_IRQ & CCOMPARE::IRQ_CAP1)       //Прерывание T1 по CAP1 (Us)
    {            
      CCOMPARE& rCompare = CCOMPARE::getInstance();
      unsigned int time_diff = LPC_TIM1->TC - rCompare.Us_time;
      rCompare.Us_f = CCOMPARE::TIC_SEC / static_cast<float>(time_diff);      
      rCompare.Us_time = LPC_TIM1->TC;
      rCompare.Us_f_comp = true;  
    }
  }
}


  