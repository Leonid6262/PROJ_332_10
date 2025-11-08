#pragma once

#include "LPC407x_8x_177x_8x.h"

class CCOMPARE
{ 

private:
  
  static constexpr unsigned int IOCON_T3_CAP1 = 0x23;
  static constexpr unsigned int IOCON_T1_CAP1 = 0x23;
  
  static constexpr unsigned int TIM3_TCR_START = 0x01;
  static constexpr unsigned int TIM1_TCR_START = 0x01;
  
  static constexpr unsigned int TIM3_CAPTURE_RI = 0x28;
  static constexpr unsigned int TIM1_CAPTURE_RI = 0x28;
  
  static constexpr unsigned int IRQ_CAP1 = 0x20;
  
  static constexpr unsigned int _100ms = 1000000;
   
public:
  
  CCOMPARE();

  float SYNC_FREQUENCY;
  float sync_f;
  bool sync_f_comp;
  unsigned int sync_time;
  
  float STATOR_FREQUENCY;
  float Us_f;
  bool Us_f_comp;
  unsigned int Us_time;
  
  void test();
  void start();
  
  static constexpr float TIC_SEC = 1000000.0;

};
