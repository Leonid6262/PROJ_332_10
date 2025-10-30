#pragma once

#include "rem_osc.hpp"
#include "adc.hpp"

class CTestESP32{ 
  

public:
  
  CREM_OSC& rRem_osc;
  CADC& rAdc;
  CTestESP32(CREM_OSC&, CADC&);
  
  void test();
   
private:
  
  signed short test_var_1;
  signed short test_var_2;
  signed short test_var_3;
  signed short test_var_4;
   
};