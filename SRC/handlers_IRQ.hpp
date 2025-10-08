#pragma once

#include "Puls.hpp"
#include "Compare.hpp"

extern "C" 
{
  void TIMER1_IRQHandler(void);
  void TIMER2_IRQHandler(void);
  void TIMER3_IRQHandler(void);
}

/*
  Синглтон-прокси для доступа к TIMER1_IRQHandler(), TIMER2_IRQHandler() и TIMER3_IRQHandler()
*/

class CProxyHandlerTIMER123 
{
public:
  
  static constexpr unsigned int IOCON_P_PWM  = 0x03;                            //Тип портов - PWM
  static constexpr unsigned int IOCON_P_PORT = 0x00;                            //Тип портов - Port
  static constexpr unsigned int P1_2 = 0x02;                                    //Port1:2
  static constexpr unsigned int P1_3 = 0x03;                                    //Port1:3
  
  static CProxyHandlerTIMER123& getInstance() 
  {
    static CProxyHandlerTIMER123 instance;
    return instance;
  }
  
  CPULS* pPuls;
  CCOMPARE* pCompare;
  CREM_OSC* pRem_osc;
  void set_pointers(CPULS*, CCOMPARE*, CREM_OSC*);
  
  const unsigned int IRQ_CAP1 = 0x20;
  const unsigned int IRQ_MR0  = 0x01;
  const unsigned int IRQ_MR1  = 0x02;
  
private:
  
    
  
    CProxyHandlerTIMER123(); 
    CProxyHandlerTIMER123(const CProxyHandlerTIMER123&) = delete;
    CProxyHandlerTIMER123& operator=(const CProxyHandlerTIMER123&) = delete; 
    
};