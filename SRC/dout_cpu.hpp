#pragma once

#include "LPC407x_8x_177x_8x.h"

class CDout_cpu
{
private:
  
  static constexpr unsigned short B_ULED       = 9;   // Бит U-LED
  static constexpr unsigned short B0_PORT_OUT = 24;   // 1-й бит порта
  
public:
  
  static inline void UserLedOn() 
  {
    LPC_GPIO0->CLR = (1UL << B_ULED);
  }
  
  static inline void UserLedOff() 
  {
    LPC_GPIO0->SET = (1UL << B_ULED);
  }
  
  inline void bNamePo0B0(bool state)    {edit_bit(0, state);}
  inline void REL_LEAKAGE_P(bool state) {edit_bit(1, state);} // Реле контроля изоляции К1
  inline void REL_LEAKAGE_N(bool state) {edit_bit(2, state);} // Реле контроля изоляции К2
  inline void bNamePo0B3(bool state)    {edit_bit(3, state);}
  inline void bNamePo0B4(bool state)    {edit_bit(4, state);}
  inline void bNamePo0B5(bool state)    {edit_bit(5, state);}
  inline void bNamePo0B6(bool state)    {edit_bit(6, state);}
  inline void bNamePo0B7(bool state)    {edit_bit(7, state);}
  
  void edit_bit(char, bool); 
  
};
