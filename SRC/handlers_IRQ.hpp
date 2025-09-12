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
  
  static CProxyHandlerTIMER123& getInstance() 
  {
    static CProxyHandlerTIMER123 instance;
    return instance;
  }
  
  CPULS* pPuls;
  CCOMPARE* pCompare;
  void set_pointers(CPULS*, CCOMPARE*);
  
  static constexpr unsigned int IRQ_CAP1 = 0x20;
  static constexpr unsigned int IRQ_MR0  = 0x01;
  static constexpr unsigned int IRQ_MR1  = 0x02;
  
private:
  
    CProxyHandlerTIMER123(); 
    CProxyHandlerTIMER123(const CProxyHandlerTIMER123&) = delete;
    CProxyHandlerTIMER123& operator=(const CProxyHandlerTIMER123&) = delete; 
    
};