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


// --- Singleton Майерса ---
CPULS& CPULS::getInstance() 
{
  // Инициализация в момент первого вызова
  static CPULS instance; 
  return instance;
}

void CPULS::start() 
{  
  forcing_bridge = false;
  main_bridge    = false;
  
  N_Pulse = 1;
  
  LPC_TIM2->MCR  = 0x00000000;          //Compare TIM3 с MR0 и MR1, с прерываниями (disabled)
  LPC_TIM2->IR   = 0xFFFFFFFF;          //Очистка флагов прерываний  
  LPC_TIM2->TCR |= TIM2_TCR_START;      //Старт таймера TIM2
  NVIC_EnableIRQ(TIMER2_IRQn);
  
  LPC_IOCON->P1_2     = IOCON_P1_PWM;   //P1_2->PWM0:1 (SUM-1) 
  LPC_IOCON->P1_3     = IOCON_P1_PWM;   //P1_3->PWM0:2 (SUM-2) 
  LPC_PWM0->PCR       = 0x00;           //Отключение PWM0 
  LPC_PWM0->MR0       = PWM_WIDTH * 2;  //Период ШИМ. MR0 - включение
  LPC_PWM0->MR1       = PWM_WIDTH;      //Выключение PWM0:1 по MR1
  LPC_PWM0->MR2       = PWM_WIDTH;      //Выключение PWM0:2 по MR2
  
  LPC_PWM0->LER       = LER_012;        //Обновление MR0,MR1 и MR2
  LPC_PWM0->TCR       = COUNTER_STOP;   //Включение PWM. Счётчик - стоп
  
  LPC_TIM2->MR1 = LPC_TIM2->TC + PULSE_WIDTH;    
  LPC_TIM2->MCR = TIM2_COMPARE_MR1;     //Compare TIM3 с MR1- enabled
}

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


  