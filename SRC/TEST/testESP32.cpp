#include "testESP32.hpp"


CTestESP32::CTestESP32(CREM_OSC& rRem_osc, CADC& rAdc) : rRem_osc(rRem_osc), rAdc(rAdc)
{
  test_var_1 = 0;
  test_var_2 = 0;
  test_var_3 = 0;
  test_var_4 = 0;
  rRem_osc.set_init.pData[0] = &test_var_1;
  rRem_osc.set_init.pData[1] = &test_var_2;
  rRem_osc.set_init.pData[2] = &test_var_3;
  rRem_osc.set_init.pData[3] = &test_var_4; 
}

void CTestESP32::test() 
{
  static unsigned int prev_TC0 = LPC_TIM0->TC;
  
  unsigned int dTrs = LPC_TIM0->TC - prev_TC0;
  if(dTrs < 33333) return;  
  prev_TC0 = LPC_TIM0->TC;
  
  //test_var_1 = rAdc.data[CADC::ROTOR_CURRENT];  
  //test_var_2 = rAdc.data[CADC::ROTOR_VOLTAGE];  
  //test_var_3 = rAdc.data[CADC::STATOR_CURRENT];  
  //test_var_4 = rAdc.data[CADC::STATOR_VOLTAGE];  
  
  // Измеренные/вычисленные значения отображаемых переменных
  test_var_1 += 10;  if(test_var_1 > 1900) test_var_1 = 0; 
  test_var_2 += 120; if(test_var_2 > 1400) test_var_2 = 0;
  test_var_3 += 60;  if(test_var_2 > 1500) test_var_3 = -150;
  test_var_4 -= 40;  if(test_var_2 < -900) test_var_4 = 0;
}
