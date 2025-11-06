#include "handlers_IRQ.hpp"
#include "system_LPC177x.h"
#include "dout_cpu.hpp"
#include "LPC407x_8x_177x_8x.h"

CProxyHandlerTIMER123::CProxyHandlerTIMER123(){}; // Прокси для доступа к Timers IRQ Handlers

void CProxyHandlerTIMER123::set_pointers(CPULS* pPuls, CCOMPARE* pCompare, CREM_OSC* pRem_osc, CADC* pAdc)  
{
  this->pPuls = pPuls;
  this->pCompare = pCompare;
  this->pRem_osc = pRem_osc;
  this->pAdc = pAdc;
}

extern "C" 
{
  // Compare таймера 2 используются для выдачи классической последовательности СИФУ
  void TIMER2_IRQHandler( void )
  { 
  
    CProxyHandlerTIMER123& rProxy = CProxyHandlerTIMER123::getInstance();
    
    unsigned int IRQ = LPC_TIM2->IR;
    LPC_TIM2->IR = 0xFFFFFFFF;

    if (IRQ & rProxy.IRQ_MR0)                           //Прерывание по Compare с MR0 (P->1)
    {                          
      //Старт ИУ форсировочного моста
      if(rProxy.pPuls->main_bridge)   
      {
        LPC_GPIO3->CLR  = rProxy.pPuls->pulses[rProxy.pPuls->N_Pulse - 1] << CPULS::FIRS_PULS_PORT;
        LPC_IOCON->P1_2 = CProxyHandlerTIMER123::IOCON_P_PWM;   //P1_2->PWM0:1 (SUM-1)                 
        LPC_PWM0->PCR   = CPULS::PCR_PWMENA1; 
        LPC_PWM0->TCR   = CPULS::COUNTER_START;      //Старт счётчик b1<-0
        LPC_PWM0->LER   = CPULS::LER_012;            //Обновление MR0,MR1 и MR2 
      }
      //Старт ИУ рабочего моста
      if(rProxy.pPuls->forcing_bridge)      
      {
        LPC_GPIO3->CLR  = rProxy.pPuls->pulses[rProxy.pPuls->N_Pulse - 1] << CPULS::FIRS_PULS_PORT;
        LPC_IOCON->P1_3 = CProxyHandlerTIMER123::IOCON_P_PWM;   //P1_3->PWM0:2 (SUM-2)        
        LPC_PWM0->PCR   = CPULS::PCR_PWMENA2; 
        LPC_PWM0->TCR   = CPULS::COUNTER_START;      //Старт счётчик b1<-0
        LPC_PWM0->LER   = CPULS::LER_012;            //Обновление MR0,MR1 и MR2 
      }
      
      LPC_TIM2->MR1 = LPC_TIM2->TC + CPULS::PULSE_WIDTH; 
      LPC_TIM2->MCR = CPULS::TIM2_COMPARE_MR1;
      
      // Измерение всех используемых (в ВТЕ) аналоговых сигналов (внешнее ADC)
      CDout_cpu::UserLedOn();
      rProxy.pAdc->conv
        ({
          CADC::ROTOR_CURRENT, 
          CADC::STATOR_VOLTAGE, 
          CADC::ROTOR_VOLTAGE, 
          CADC::EXTERNAL_SETTINGS
        });
      rProxy.pAdc->conv
        ({
          CADC::LEAKAGE_CURRENT, 
          CADC::STATOR_CURRENT, 
          CADC::LOAD_NODE_CURRENT
        });
      CDout_cpu::UserLedOff();
    /* 
      Для сокращения записи аргументов здесь использована си нотация enum, вмесо типобезопасной enum class c++.
      CADC::ROTOR_CURRENT вместо static_cast<char>(CADC::EADC_NameCh::ROTOR_CURRENT) - считаю, разумный компромисс.
      Пример доступа к измеренным значениям - rADC.data[CADC::ROTOR_CURRENT] 
    */
      //----------------------------------------------------------------------
      
      /*--- Здесь передаются отображаемые данные в ESP32 ---*/
      rProxy.pRem_osc->send_data();
      /*–-------------------------–-------------------------*/
      
      // По capture таймера 3 измеряется частота синхронизации
      unsigned int TIMER3_IRQ = LPC_TIM3->IR;
      LPC_TIM3->IR = 0xFFFFFFFF;
      
      if(TIMER3_IRQ & rProxy.IRQ_CAP1)
      {      
        unsigned int CR1 = LPC_TIM3->CR1;
        unsigned int time_diff = CR1 - rProxy.pCompare->sync_time;
        rProxy.pCompare->sync_time = CR1;      
        rProxy.pCompare->sync_f = CCOMPARE::TIC_SEC / static_cast<float>(time_diff);                 
        rProxy.pCompare->sync_f_comp = true;
      }
      
    }
    
    if (IRQ & rProxy.IRQ_MR1)          //Прерывание по Compare с MR1 (P->0)
    {                 
      LPC_IOCON->P1_2 = CProxyHandlerTIMER123::IOCON_P_PORT; //P1_2 - Port
      LPC_GPIO1->CLR  = 1UL << CProxyHandlerTIMER123::P1_2;
      LPC_IOCON->P1_3 = CProxyHandlerTIMER123::IOCON_P_PORT; //P1_3 - Port
      LPC_GPIO1->CLR  = 1UL << CProxyHandlerTIMER123::P1_3;      
      
      LPC_GPIO3->SET   = CPULS::OFF_PULSES;              
      LPC_TIM2->MR0   += CPULS::PULSE_PERIOD;
      LPC_TIM2->MCR    = CPULS::TIM2_COMPARE_MR0;            
      
      LPC_PWM0->TCR  = CPULS::COUNTER_STOP;            //Стоп счётчик b1<-1
      LPC_PWM0->TCR  = CPULS::COUNTER_RESET;
      
      rProxy.pPuls->N_Pulse++;
      if(rProxy.pPuls->N_Pulse > CPULS::N_PULSES) 
      {
        rProxy.pPuls->N_Pulse = 1;     
      }
    }   
  }  
}

// По capture таймера 1 измеряется частота напряжения статора
extern "C" 
{
  void TIMER1_IRQHandler( void )
  {   
    unsigned int TIMER1_IRQ = LPC_TIM1->IR;
    LPC_TIM1->IR = 0xFFFFFFFF;
    
    CProxyHandlerTIMER123& rProxy = CProxyHandlerTIMER123::getInstance();
    
    if (TIMER1_IRQ & rProxy.IRQ_CAP1)       //Прерывание T1 по CAP1 (Us)
    {            
      unsigned int CR1 = LPC_TIM1->CR1;
      unsigned int time_diff = CR1 - rProxy.pCompare->Us_time;
      rProxy.pCompare->Us_time = CR1;
      rProxy.pCompare->Us_f = CCOMPARE::TIC_SEC / static_cast<float>(time_diff);            
      rProxy.pCompare->Us_f_comp = true;  
    }
  }
}
