#include "SIFU.hpp"
#include <algorithm>
#include "system_LPC177x.h"

const unsigned char CSIFU::pulses[]  = {0x00, 0x21, 0x03, 0x06, 0x0C, 0x18, 0x30}; // Индекс 0 не используется
const signed short  CSIFU::offsets[] = 
{
 0, 
   SyncState::_60gr, 
   SyncState::_120gr, 
   SyncState::_180gr, 
  -SyncState::_120gr, 
  -SyncState::_60gr, 
   SyncState::_0gr
}; // Индекс 0 не используется

CSIFU::CSIFU(CPULSCALC& rPulsCalc) : rPulsCalc(rPulsCalc){}

void CSIFU::rising_puls()
{
  
  N_Pulse = (N_Pulse % N_PULSES) + 1; 
  
  //Старт ИУ форсировочного моста
  if(main_bridge)   
  {
    LPC_IOCON->P1_2 = IOCON_P_PWM;        //P1_2->PWM0:1 (SUM-1)                 
    LPC_PWM0->PCR   = PCR_PWMENA1; 
    LPC_PWM0->TCR   = COUNTER_START;      //Старт счётчик b1<-0
    LPC_PWM0->LER   = LER_012;            //Обновление MR0,MR1 и MR2 
    LPC_GPIO3->CLR  = pulses[N_Pulse] << FIRS_PULS_PORT;
  }
  //Старт ИУ рабочего моста
  if(forcing_bridge)      
  {    
    LPC_IOCON->P1_3 = IOCON_P_PWM;        //P1_3->PWM0:2 (SUM-2)        
    LPC_PWM0->PCR   = PCR_PWMENA2; 
    LPC_PWM0->TCR   = COUNTER_START;      //Старт счётчик b1<-0
    LPC_PWM0->LER   = LER_012;            //Обновление MR0,MR1 и MR2
    LPC_GPIO3->CLR  = pulses[N_Pulse] << FIRS_PULS_PORT;
  }
  
  control_sync();     // Мониторинг события захвата синхроимпульса
  
  LPC_TIM3->MR1 = LPC_TIM3->MR0 + PULSE_WIDTH;          // Окончание текущего 
  
  switch(v_sync.Operating_mode)
  {
  case EOperating_mode::NO_SYNC:        
    LPC_TIM3->MR0 = LPC_TIM3->MR0 + v_sync._60gr;       // Старт следующего через 60 градусов
    break;  
  case EOperating_mode::RESYNC:           
    v_sync.Operating_mode = EOperating_mode::NORMAL;    // Синхронизация с 1-го в Alpha_max
    A_Task_tick = A_Max_tick;
    A_Cur_tick = A_Max_tick; 
    A_Prev_tick = A_Max_tick; 
    d_Alpha = 0;
    LPC_TIM3->MR0 = v_sync.CURRENT_SYNC + A_Cur_tick;   // 1-2-3-4-sync-5->6-1-2-3-4-sync-5->6-1-2....
    N_Pulse = 6;    
    break;                                              
  case EOperating_mode::NORMAL:           
    // Классические ограничения
    if(A_Task_tick > A_Max_tick) A_Task_tick = A_Max_tick;
    if(A_Task_tick < A_Min_tick) A_Task_tick = A_Min_tick;   
    
    d_Alpha = A_Task_tick - A_Prev_tick;
    
    if (abs(d_Alpha) < d_A_Max_tick) A_Cur_tick = A_Task_tick; 
    
    else 
    {
      //d_Alpha = (d_Alpha > 0 ? d_A_Max_tick : -d_A_Max_tick);
      //A_Cur_tick += d_Alpha;
      
      if(d_Alpha > 0) 
      {
        A_Cur_tick += d_A_Max_tick;
        d_Alpha = d_A_Max_tick;
      }
      else 
      {
        A_Cur_tick -= d_A_Max_tick;
        d_Alpha = -d_A_Max_tick;
      }
    }

    A_Prev_tick = A_Cur_tick;
    
    // Расчёт тайминга для следующего импульса
    if(v_sync.SYNC_EVENT)
    {
      v_sync.SYNC_EVENT = false;

      //v_sync.sync_timing = v_sync.CURRENT_SYNC + offsets[N_Pulse];
      
      switch(N_Pulse)
      {
      case 4: // Диапазон 180...240
        v_sync.sync_timing = v_sync.CURRENT_SYNC - v_sync._120gr;// для 5
        break;  
      case 5: // Диапазон 120...180
        v_sync.sync_timing = v_sync.CURRENT_SYNC - v_sync._60gr;// для 6
        break;
      case 6: // Диапазон 60...120  
        v_sync.sync_timing = v_sync.CURRENT_SYNC + v_sync._0gr; // для 1
        break;
      case 1: // Диапазон 0...60 
        v_sync.sync_timing = v_sync.CURRENT_SYNC + v_sync._60gr;// для 2
        break;
      case 2: // Диапазон -60...0
        v_sync.sync_timing = v_sync.CURRENT_SYNC + v_sync._120gr;// для 3
        break;
      case 3: // Диапазон -120...-60
        v_sync.sync_timing = v_sync.CURRENT_SYNC + v_sync._180gr;// для 4
        break;
      }      
      LPC_TIM3->MR0 = v_sync.sync_timing + A_Cur_tick; 
    }
    else
    {
      LPC_TIM3->MR0 = LPC_TIM3->MR0 + v_sync._60gr + d_Alpha;
    }    
    break;
  }    
   // Измерение всех используемых (в ВТЕ) аналоговых сигналов (внешнее ADC)      
   // Восстанавление сигналов напряжения и тока статора
   rPulsCalc.conv_and_calc();
}

void CSIFU::faling_puls()
{
  LPC_IOCON->P1_2 = IOCON_P_PORT; //P1_2 - Port
  LPC_GPIO1->CLR  = 1UL << P1_2;
  LPC_IOCON->P1_3 = IOCON_P_PORT; //P1_3 - Port
  LPC_GPIO1->CLR  = 1UL << P1_3;      
  
  LPC_GPIO3->SET   = OFF_PULSES;              
  
  LPC_PWM0->TCR  = COUNTER_STOP;  //Стоп счётчик b1<-1
  LPC_PWM0->TCR  = COUNTER_RESET;  
}

void CSIFU::control_sync()
{ 
  v_sync.current_cr = LPC_TIM3->CR1;
  
  if((v_sync.Operating_mode == EOperating_mode::NO_SYNC) && (v_sync.previous_cr != v_sync.current_cr))
  {
    unsigned int dt = v_sync.current_cr - v_sync.previous_cr;
    v_sync.previous_cr = v_sync.current_cr;   
    if (dt >= v_sync.DT_MIN && dt <= v_sync.DT_MAX)
    {
      v_sync.sync_pulses++;
      if(v_sync.sync_pulses > 100)
      {        
        v_sync.Operating_mode = EOperating_mode::RESYNC;
        v_sync.no_sync_pulses = 0;
        v_sync.CURRENT_SYNC = v_sync.current_cr;
      }
    }
    else 
    {        
      v_sync.sync_pulses = 0;
    }
    return;
  }

  if(v_sync.Operating_mode == EOperating_mode::NORMAL)
  {
    if(v_sync.previous_cr != v_sync.current_cr)
    {
      unsigned int dt = v_sync.current_cr - v_sync.previous_cr;
      v_sync.previous_cr = v_sync.current_cr;
      if (dt >= v_sync.DT_MIN && dt <= v_sync.DT_MAX)
      {
        SYNC_FREQUENCY = v_sync.TIC_SEC / static_cast<float>(dt);
        v_sync.CURRENT_SYNC = v_sync.current_cr;
        v_sync.no_sync_pulses = 0;
        v_sync.SYNC_EVENT = true; 
      }     
      else
      {
        SYNC_FREQUENCY = 0;
        v_sync.sync_pulses = 0;
        v_sync.Operating_mode = EOperating_mode::NO_SYNC;
      }      
    }
    else
    {
      v_sync.no_sync_pulses++;
      if(v_sync.no_sync_pulses > (N_PULSES * 4))
      {        
        SYNC_FREQUENCY = 0;
        v_sync.sync_pulses = 0;
        v_sync.Operating_mode = EOperating_mode::NO_SYNC;
      }
    }
  } 
}  

void  CSIFU::start_forcing_bridge()
{
  forcing_bridge = true;
}
void  CSIFU::start_main_bridge()
{
  main_bridge = true;
}
void  CSIFU::stop()
{
  forcing_bridge = false;
  main_bridge = false;
}

void CSIFU::init_and_start()
{      
  forcing_bridge = false;
  main_bridge    = false;
  
  N_Pulse = 1;   
  v_sync.SYNC_EVENT = false;  
  v_sync.no_sync_pulses = 0;
  v_sync.sync_pulses = 0;
  v_sync.Operating_mode = EOperating_mode::NO_SYNC;
  
  LPC_SC->PCONP   |= CLKPWR_PCONP_PCPWM0;       //PWM0 power/clock control bit.
  LPC_PWM0->PR     = PWM_div_0 - 1;             //при PWM_div=60, F=60МГц/60=1МГц, 1тик=1мкс       
  
  LPC_PWM0->TCR    = COUNTER_CLR;               //Сброс регистра таймера
  LPC_PWM0->TCR    = COUNTER_RESET;             //Сброс таймера 
  
  LPC_PWM0->PCR       = 0x00;           //Отключение PWM0 
  LPC_PWM0->MR0       = PWM_WIDTH * 2;  //Период ШИМ. MR0 - включение
  LPC_PWM0->MR1       = PWM_WIDTH;      //Выключение PWM0:1 по MR1
  LPC_PWM0->MR2       = PWM_WIDTH;      //Выключение PWM0:2 по MR2
  
  LPC_PWM0->LER       = LER_012;        //Обновление MR0,MR1 и MR2
  LPC_PWM0->TCR       = COUNTER_STOP;   //Включение PWM. Счётчик - стоп
  LPC_PWM0->TCR       = COUNTER_RESET;
  
  LPC_IOCON->P2_23 = IOCON_T3_CAP1;     //T3 CAP1   
  LPC_TIM3->MCR  = 0x00000000;          //Compare TIM3 с MR0 и MR1, с прерываниями (disabled)
  LPC_TIM3->IR   = 0xFFFFFFFF;          //Очистка флагов прерываний  
  LPC_TIM3->TCR |= TIM3_TCR_START;      //Старт таймера TIM3
  
  LPC_TIM3->TC = 0;
  LPC_TIM3->MR0 = v_sync._60gr;
  LPC_TIM3->MR1 = v_sync._60gr + PULSE_WIDTH;
  LPC_TIM3->CCR  = TIM3_CAPTURE_RI;      //Захват T3 по фронту CAP1 без прерываний 
  LPC_TIM3->MCR  = TIM3_COMPARE_MR0;     //Compare TIM3 с MR0 - enabled
  LPC_TIM3->MCR |= TIM3_COMPARE_MR1;     //Compare TIM3 с MR1 - enabled
  
  NVIC_EnableIRQ(TIMER3_IRQn);
  
}


