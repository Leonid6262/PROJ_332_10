#include "handlers_IRQ.hpp"
#include "system_LPC177x.h"
#include "LPC407x_8x_177x_8x.h"


extern "C" 
{
  void TIMER2_IRQHandler( void )
  { 
    
    unsigned short IRQ = LPC_TIM2->IR;
    LPC_TIM2->IR &= 0xFFFFFFFF;

    if (IRQ & CPULS::IRQ_MR0)                          //Прерывание по Compare с MR0 (P->1)
    {                          
      CPULS& rCpuls = CPULS::getInstance();
      
      if(rCpuls.forcing_bridge)   
      {
        LPC_PWM0->PCR     = rCpuls.PCR_PWMENA1;          
      }
      if(rCpuls.main_bridge)      
      {
        LPC_PWM0->PCR     = rCpuls.PCR_PWMENA2;
      }
           
      LPC_PWM0->TCR       = rCpuls.COUNTER_START;      //Старт счётчик b1<-0
      LPC_PWM0->LER       = rCpuls.LER_012;            //Обновление MR0,MR1 и MR2     
     
      for(int Counter = rCpuls.DELAY_PWM; Counter > 0; Counter--){}
      
      if(rCpuls.forcing_bridge || rCpuls.main_bridge)
      {
        LPC_GPIO3->CLR  = rCpuls.pulses[rCpuls.N_Pulse - 1] << rCpuls.FIRS_PULS_PORT;
      }
      else
      {
        LPC_GPIO3->SET  = rCpuls.OFF_PULSES; 
        LPC_PWM0->TCR   = rCpuls.COUNTER_STOP;         //Стоп счётчик b1<-1
        LPC_PWM0->PCR   = 0x00;
      }            
      LPC_TIM2->MR1 = LPC_TIM2->TC + rCpuls.PULSE_WIDTH; 
      LPC_TIM2->MCR = rCpuls.TIM2_COMPARE_MR1;
    }
    
    if (IRQ & CPULS::IRQ_MR1)                          //Прерывание по Compare с MR1 (P->0)
    {            
      CPULS& rCpuls = CPULS::getInstance();
      
      LPC_GPIO3->SET  = rCpuls.OFF_PULSES;              
      LPC_TIM2->MR0   = LPC_TIM2->TC + rCpuls.PULSE_PERIOD - rCpuls.PULSE_WIDTH;
      LPC_TIM2->MCR   = rCpuls.TIM2_COMPARE_MR0;            
      
      LPC_PWM0->TCR  = rCpuls.COUNTER_STOP;            //Стоп счётчик b1<-1
      LPC_PWM0->PCR  = 0x00;
      
      rCpuls.N_Pulse++;
      if(rCpuls.N_Pulse > rCpuls.N_PULSES) 
      {
        rCpuls.N_Pulse = 1;     
      }
    }   
  }  
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
