#include "handlers_IRQ.hpp"
#include "system_LPC177x.h"
#include <cmath>
#include "LPC407x_8x_177x_8x.h"

CProxyHandlerTIMER::CProxyHandlerTIMER(){}; // Прокси для доступа к Timers IRQ Handlers

void CProxyHandlerTIMER::set_pointers(CPULS* pPuls, CREM_OSC* pRem_osc, CADC* pAdc)  
{
  this->pPuls = pPuls;
  this->pRem_osc = pRem_osc;
  this->pAdc = pAdc;
}

extern "C" 
{
  // Compare таймера 2 используются для выдачи классической последовательности СИФУ
  void TIMER2_IRQHandler( void )
  { 
  
    CProxyHandlerTIMER& rProxy = CProxyHandlerTIMER::getInstance();
    
    unsigned int IRQ = LPC_TIM2->IR;
    LPC_TIM2->IR = 0xFFFFFFFF;

    // Rising puls
    if (IRQ & rProxy.IRQ_MR0)       //Прерывание по Compare с MR0 (P->1)
    {                          
      
      rProxy.pPuls->start_puls();   //Старт ИУ   
 
      // Измерение всех используемых (в ВТЕ) аналоговых сигналов (внешнее ADC)
      rProxy.pAdc->conv_tnf
        ({
          CADC::ROTOR_CURRENT, 
          CADC::STATOR_VOLTAGE, 
          CADC::ROTOR_VOLTAGE, 
          CADC::EXTERNAL_SETTINGS,
          CADC::LEAKAGE_CURRENT, 
          CADC::STATOR_CURRENT, 
          CADC::LOAD_NODE_CURRENT
        });
      /* 
      Для сокращения записи аргументов здесь использована си нотация enum, вмесо типобезопасной enum class c++.
      CADC::ROTOR_CURRENT вместо static_cast<char>(CADC::EADC_NameCh::ROTOR_CURRENT) - считаю, разумный компромисс.
      Пример доступа к измеренным значениям - rADC.data[CADC::ROTOR_CURRENT] 
      */
      
      rProxy.pPuls->sin_restoration();  // Восстанавление сигналов напряжения и тока статора
      rProxy.pRem_osc->send_data();     // Передача отображаемых данных в ESP32

    }
    
    // Faling puls
    if (IRQ & rProxy.IRQ_MR1)           //Прерывание по Compare с MR1 (P->0)
    {                       
      rProxy.pPuls->stop_puls();        //Стоп ИУ       
    }  
    
  }  
}

