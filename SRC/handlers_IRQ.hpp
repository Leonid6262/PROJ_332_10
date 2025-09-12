#pragma once

#include "Puls.hpp"
#include "Compare.hpp"

extern "C" {
  void TIMER2_IRQHandler(void);
}

/*
  Синглтон-прокси для доступа к void TIMER2_IRQHandler( void ) из CPULS
*/

class CProxy_Handler_TIMER2 {
public:
    static CProxy_Handler_TIMER2& getInstance() {
        static CProxy_Handler_TIMER2 instance;
        return instance;
    }

    void setTarget(CPULS* pPuls) {
        this->pPuls = pPuls;
    }

private:

    CProxy_Handler_TIMER2(); 
    CProxy_Handler_TIMER2(const CProxy_Handler_TIMER2&) = delete;
    CProxy_Handler_TIMER2& operator=(const CProxy_Handler_TIMER2&) = delete; // Delete assignment operator

    CPULS* pPuls;
};