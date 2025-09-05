#include "din_cpu.hpp"
#include "settings_eep.hpp"

// Постоянные времени интегрирования фильтра (ms первый множитель) din портов. 
// TIC_ms = 10000 дискрет таймера на 1ms.  
// То есть, 50*TIC_ms = 50ms, 0*TIC_ms - нет фильтрации, и т.п. 
const unsigned int CDin_cpu::cConst_integr_Pi0[N_BITS] = 
{
  50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms
};

CDin_cpu::CDin_cpu()
{
  //Инициализация фильтра
  for(char b = 0; b < N_BITS; b++) 
  {
    integrator[b] = 1;     
  }  
  prev_TC0 = LPC_TIM0->TC;
}

void CDin_cpu::input_Pi0() 
{
  
  data_din_Pi0 = ~static_cast<unsigned char>(LPC_GPIO2->PIN >> B0_PORT0_IN);
  
  //--Фильтр (интегратор входного сигнала)--
  unsigned int dT = LPC_TIM0->TC - prev_TC0; //Текущая дельта [0.1*mks]
  prev_TC0 = LPC_TIM0->TC;
  
  //Входные данные порта  с учётом инверсии
  unsigned char data_din_invert = data_din_Pi0 ^ CEEPSettings::getInstance().getSettings().din_Pi0_invert;
  //Фильтр 8-ми бит
  for(char b = 0; b < N_BITS; b++)
  {    
    if(data_din_invert & (1UL << b))
    {           
      //Если на входе 1
      integrator[b] += dT;
      if(integrator[b] >= cConst_integr_Pi0[b])
      { 
        //Если интегратор в насыщении, на выходе 1
        integrator[b] = cConst_integr_Pi0[b];
        UData_din_f_Pi0.all |= (1UL << b); 
      }
    }
    else
    {
      //Если на входе 0
      integrator[b] -= dT;
      if(integrator[b] <= 0)
      { 
        //Если интегратор в нуле, на выходе 0
        integrator[b] = 0;
        UData_din_f_Pi0.all &= ~(1UL << b);        
      }
    }     
  } 
}

// Порт Pi1, по сути, набор входов прерываний. Не фильтруется как Pi0
// data_inp_Pi1.all - нужен, только для визуального контроля на ПТ
void CDin_cpu::input_Pi1() 
{
  data_din_Pi1 = static_cast<unsigned char>(
                        ((LPC_GPIO2->PIN >> B0_PORT1_IN) & MASK_G1) | 
                        ((LPC_GPIO0->PIN >> (B3_PORT1_IN - BITS_G1)) & MASK_G2)
                         );
  UData_din_Pi1.all = data_din_Pi1 ^ CEEPSettings::getInstance().getSettings().din_Pi1_invert;
}


