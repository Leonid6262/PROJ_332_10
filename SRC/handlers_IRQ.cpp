#include "handlers_IRQ.hpp"
#include "system_LPC177x.h"
#include "LPC407x_8x_177x_8x.h"

CProxyHandlerTIMER123::CProxyHandlerTIMER123(){}; // Прокси для доступа к Timers IRQ Handlers

void CProxyHandlerTIMER123::set_pointers(CPULS* pPuls, CCOMPARE* pCompare, CREM_OSC* pRem_osc)  
{
  this->pPuls = pPuls;
  this->pCompare = pCompare;
  this->pRem_osc = pRem_osc;
}

extern "C" 
{
  // Compare таймера 2 используются для выдачи классической последовательности СИФУ
  void TIMER2_IRQHandler( void )
  { 
    
    unsigned short IRQ = LPC_TIM2->IR;
    LPC_TIM2->IR &= 0xFFFFFFFF;

    CProxyHandlerTIMER123& rProxy = CProxyHandlerTIMER123::getInstance();
    
    if (IRQ & rProxy.IRQ_MR0)                           //Прерывание по Compare с MR0 (P->1)
    {                          
  
      if(rProxy.pPuls->forcing_bridge)   
      {
        LPC_PWM0->PCR     = CPULS::PCR_PWMENA1;          
      }
      if(rProxy.pPuls->main_bridge)      
      {
        LPC_PWM0->PCR     = CPULS::PCR_PWMENA2;
      }
           
      LPC_PWM0->TCR       = CPULS::COUNTER_START;      //Старт счётчик b1<-0
      LPC_PWM0->LER       = CPULS::LER_012;            //Обновление MR0,MR1 и MR2     
     
      for(unsigned short Counter = CPULS::DELAY_PWM; Counter > 0; Counter--){}
      
      if(rProxy.pPuls->forcing_bridge || rProxy.pPuls->main_bridge)
      {
        LPC_GPIO3->CLR  = rProxy.pPuls->pulses[rProxy.pPuls->N_Pulse - 1] << CPULS::FIRS_PULS_PORT;
      }
      else
      {
        LPC_GPIO3->SET  = CPULS::OFF_PULSES; 
        LPC_PWM0->TCR   = CPULS::COUNTER_STOP;         //Стоп счётчик b1<-1
        LPC_PWM0->PCR   = 0x00;
      }            
      LPC_TIM2->MR1 = LPC_TIM2->TC + CPULS::PULSE_WIDTH; 
      LPC_TIM2->MCR = CPULS::TIM2_COMPARE_MR1;
     
       
      /*--- Здесь передаются отображаемые данные в ESP32 ---*/
      rProxy.pRem_osc->send_data();
      /*–-------------------------–-------------------------*/
      
    }
    
    if (IRQ & rProxy.IRQ_MR1)          //Прерывание по Compare с MR1 (P->0)
    {            
      
      LPC_GPIO3->SET  = CPULS::OFF_PULSES;              
      LPC_TIM2->MR0   = LPC_TIM2->TC + CPULS::PULSE_PERIOD - CPULS::PULSE_WIDTH;
      LPC_TIM2->MCR   = CPULS::TIM2_COMPARE_MR0;            
      
      LPC_PWM0->TCR  = CPULS::COUNTER_STOP;            //Стоп счётчик b1<-1
      LPC_PWM0->PCR  = 0x00;
      
      rProxy.pPuls->N_Pulse++;
      if(rProxy.pPuls->N_Pulse > CPULS::N_PULSES) 
      {
        rProxy.pPuls->N_Pulse = 1;     
      }
    }   
  }  
}

// По capture таймера 3 измеряется частота синхронизации
extern "C" 
{
  void TIMER3_IRQHandler( void )
  {   
    unsigned short TIMER3_IRQ = LPC_TIM3->IR;
    LPC_TIM3->IR = 0xFFFFFFFF;   
    CProxyHandlerTIMER123& rProxy = CProxyHandlerTIMER123::getInstance();
    
    if (TIMER3_IRQ & rProxy.IRQ_CAP1)       //Прерывание T3 по CAP1 (Sync)
    {            
      unsigned int time_diff = LPC_TIM3->TC - rProxy.pCompare->sync_time;      
      rProxy.pCompare->sync_f = CCOMPARE::TIC_SEC / static_cast<float>(time_diff);           
      rProxy.pCompare->sync_time = LPC_TIM3->TC;
      rProxy.pCompare->sync_f_comp = true;
    }
  }
  
  // По capture таймера 1 измеряется частота напряжения статора
  void TIMER1_IRQHandler( void )
  {   
    unsigned short TIMER1_IRQ = LPC_TIM1->IR;
    LPC_TIM1->IR = 0xFFFFFFFF;
    CProxyHandlerTIMER123& rProxy = CProxyHandlerTIMER123::getInstance();
    
    if (TIMER1_IRQ & rProxy.IRQ_CAP1)       //Прерывание T1 по CAP1 (Us)
    {            
      unsigned int time_diff = LPC_TIM1->TC - rProxy.pCompare->Us_time;
      rProxy.pCompare->Us_f = CCOMPARE::TIC_SEC / static_cast<float>(time_diff);      
      rProxy.pCompare->Us_time = LPC_TIM1->TC;
      rProxy.pCompare->Us_f_comp = true;  
    }
  }
}
