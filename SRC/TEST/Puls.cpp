#include "Puls.hpp"
#include "system_LPC177x.h"

const char CPULS::pulses[] = {0x03, 0x06, 0x0C, 0x18, 0x30, 0x21};

CPULS::CPULS(CADC& rAdc) : rAdc(rAdc){}

void CPULS::start_puls()
{
  //Старт ИУ форсировочного моста
  if(main_bridge)   
  {
    LPC_GPIO3->CLR  = pulses[N_Pulse - 1] << FIRS_PULS_PORT;
    LPC_IOCON->P1_2 = IOCON_P_PWM;        //P1_2->PWM0:1 (SUM-1)                 
    LPC_PWM0->PCR   = PCR_PWMENA1; 
    LPC_PWM0->TCR   = COUNTER_START;      //Старт счётчик b1<-0
    LPC_PWM0->LER   = LER_012;            //Обновление MR0,MR1 и MR2 
  }
  //Старт ИУ рабочего моста
  if(forcing_bridge)      
  {
    LPC_GPIO3->CLR  = pulses[N_Pulse - 1] << FIRS_PULS_PORT;
    LPC_IOCON->P1_3 = IOCON_P_PWM;        //P1_3->PWM0:2 (SUM-2)        
    LPC_PWM0->PCR   = PCR_PWMENA2; 
    LPC_PWM0->TCR   = COUNTER_START;      //Старт счётчик b1<-0
    LPC_PWM0->LER   = LER_012;            //Обновление MR0,MR1 и MR2 
  }
  
  control_sync();     // Мониторинг события захвата
   
  switch(v_sync.Operating_mode)
  {
  case EOperating_mode::NO_SYNC:
    LPC_TIM3->MR1 = LPC_TIM3->MR0 + PULSE_WIDTH;        // Окончание текущего
    LPC_TIM3->MR0 = LPC_TIM3->MR0 + PULSE_PERIOD;       // Старт следующего    
    break;
  case EOperating_mode::RESYNC:
    LPC_TIM3->MR1 = LPC_TIM3->MR0 + PULSE_WIDTH;        // Окончание текущего
                  LPC_TIM3->MR0 = LPC_TIM3->MR0 + PULSE_PERIOD;       // Старт следующего     
    //LPC_TIM3->MR0 = v_sync.CURRENT_CR + A_Max_tic;    // Старт первого
    //A_Cur_tic = A_Max_tic;
    //N_Pulse = 1;
    
    v_sync.Operating_mode = EOperating_mode::NORMAL;  
    
    break;
  case EOperating_mode::NORMAL:    
    if(v_sync.SYNC_EVENT)
    {
      v_sync.SYNC_EVENT = false;
      LPC_TIM3->MR1 = LPC_TIM3->MR0 + PULSE_WIDTH;        // Окончание текущего
                    LPC_TIM3->MR0 = LPC_TIM3->MR0 + PULSE_PERIOD;       // Старт следующего 
      //LPC_TIM3->MR0 = v_sync.CURRENT_SYNC + A_Cur_tic + (N_Pulse * 3333);
    }
    else
    {
      LPC_TIM3->MR1 = LPC_TIM3->MR0 + PULSE_WIDTH;        // Окончание текущего
                     LPC_TIM3->MR0 = LPC_TIM3->MR0 + PULSE_PERIOD;       // Старт следующего 
      //LPC_TIM3->MR0 = v_sync.CURRENT_SYNC + A_Cur_tic + (N_Pulse * 3333);
    }
    break;
  }
  
}

void CPULS::stop_puls()
{
  LPC_IOCON->P1_2 = IOCON_P_PORT; //P1_2 - Port
  LPC_GPIO1->CLR  = 1UL << P1_2;
  LPC_IOCON->P1_3 = IOCON_P_PORT; //P1_3 - Port
  LPC_GPIO1->CLR  = 1UL << P1_3;      
  
  LPC_GPIO3->SET   = OFF_PULSES;              
  
  LPC_PWM0->TCR  = COUNTER_STOP;            //Стоп счётчик b1<-1
  LPC_PWM0->TCR  = COUNTER_RESET;
  
  N_Pulse = (N_Pulse % N_PULSES) + 1;  
}

void CPULS::conv_adc()
{
  // Измерение всех используемых (в ВТЕ) аналоговых сигналов (внешнее ADC)
  rAdc.conv_tnf
    ({
      CADC::ROTOR_CURRENT, 
      CADC::STATOR_VOLTAGE,
      CADC::STATOR_CURRENT,
      CADC::ROTOR_VOLTAGE, 
      CADC::EXTERNAL_SETTINGS,
      CADC::LEAKAGE_CURRENT,           
      CADC::LOAD_NODE_CURRENT
    });
  /* 
  Для сокращения записи аргументов здесь использована си нотация enum, вмесо типобезопасной enum class c++.
  CADC::ROTOR_CURRENT вместо static_cast<char>(CADC::EADC_NameCh::ROTOR_CURRENT) - считаю, разумный компромисс.
  Пример доступа к измеренным значениям - rADC.data[CADC::ROTOR_CURRENT] 
  */
}

void CPULS::sin_restoration() 
{  
  /*
    Восстановление сигналов произвадится по двум мгновенным значениям и углу (Theta) между ними:
    A = sqrt( (u1*u1 + u2*u2 - 2 * u1*u2 * cos(Theta)) / (sin(Theta) * sin(Theta)) );
  */
  
  // Напряжение статора
  v_restoration.u_stator_2 = rAdc.data[CADC::STATOR_VOLTAGE];
  v_restoration.timing_ustator_2 = rAdc.timings[CADC::STATOR_VOLTAGE + 1];
  
  unsigned int us1us1  =  v_restoration.u_stator_1 * v_restoration.u_stator_1;
  unsigned int us2us2  =  v_restoration.u_stator_2 * v_restoration.u_stator_2;
  signed int   us1us2  =  v_restoration.u_stator_1 * v_restoration.u_stator_2;

  v_restoration.dT_ustator = v_restoration.timing_ustator_2 - v_restoration.timing_ustator_1;
  
  v_restoration.u_stator_1 = v_restoration.u_stator_2;
  v_restoration.timing_ustator_1 = v_restoration.timing_ustator_2;
  
  float u_theta = (2.0f * v_restoration.pi * v_restoration.freq * v_restoration.dT_ustator) / 1000000.0f;
  
  float ucos = std::cos(u_theta);
  float usin = std::sin(u_theta);
   
  // Ток статора
  v_restoration.i_stator_2 = rAdc.data[CADC::STATOR_CURRENT];
  v_restoration.timing_istator_2 = rAdc.timings[CADC::STATOR_CURRENT + 1];
  
  unsigned int is1is1  =  v_restoration.i_stator_1 * v_restoration.i_stator_1;
  unsigned int is2is2  =  v_restoration.i_stator_2 * v_restoration.i_stator_2;
  signed int   is1is2  =  v_restoration.i_stator_1 * v_restoration.i_stator_2;

  v_restoration.dT_istator = v_restoration.timing_istator_2 - v_restoration.timing_istator_1;
  
  v_restoration.i_stator_1 = v_restoration.i_stator_2;
  v_restoration.timing_istator_1 = v_restoration.timing_istator_2;
  
  float i_theta = (2.0f * v_restoration.pi * v_restoration.freq * v_restoration.dT_istator) / 1000000.0f;
  
  float icos = std::cos(i_theta);
  float isin = std::sin(i_theta);
  
  // Скользящее среднее по 6-ти пульсам
  v_restoration.ind_d_avr = (v_restoration.ind_d_avr + 1) % v_restoration.PULS_AVR;
  
  v_restoration.u_stat[v_restoration.ind_d_avr] = sqrt(((us1us1 + us2us2) - (us1us2 * 2 * ucos)) / (usin * usin));  
  float uavr = (
                v_restoration.u_stat[0] + 
                v_restoration.u_stat[1] + 
                v_restoration.u_stat[2] + 
                v_restoration.u_stat[3] + 
                v_restoration.u_stat[4] + 
                v_restoration.u_stat[5]
                  ) / v_restoration.PULS_AVR;
  U_STATORA = static_cast<int>(uavr + 0.5f);
  
  v_restoration.i_stat[v_restoration.ind_d_avr] = sqrt(((is1is1 + is2is2) - (is1is2 * 2 * icos)) / (isin * isin));  
  float iavr = (
                v_restoration.i_stat[0] + 
                v_restoration.i_stat[1] + 
                v_restoration.i_stat[2] + 
                v_restoration.i_stat[3] + 
                v_restoration.i_stat[4] + 
                v_restoration.i_stat[5]
                            ) / v_restoration.PULS_AVR;
  I_STATORA = static_cast<int>(iavr + 0.5f);
  
}

void CPULS::control_sync()
{ 
  v_sync.current_cr = LPC_TIM3->CR1;
  
  if(v_sync.Operating_mode == EOperating_mode::NO_SYNC)
  {
    if(v_sync.previous_cr != v_sync.current_cr)
    {
      unsigned int dt = v_sync.current_cr - v_sync.previous_cr;
      v_sync.previous_cr = v_sync.current_cr;
      
      if (dt >= v_sync.DT_MIN && dt <= v_sync.DT_MAX)
      {
        v_sync.sync_pulses++;
        if(v_sync.sync_pulses > 200)
        {        
          v_sync.Operating_mode = EOperating_mode::RESYNC;
          v_sync.no_sync_pulses = 0;
        }
      }
      else 
      {        
        v_sync.sync_pulses = 0;
      }
    }
  }
  
  if(v_sync.Operating_mode == EOperating_mode::NORMAL)
  {
    if(v_sync.previous_cr != v_sync.current_cr)
    {
      v_sync.no_sync_pulses = 0;
      unsigned int dt = v_sync.current_cr - v_sync.previous_cr;
      v_sync.previous_cr = v_sync.current_cr;
      SYNC_FREQUENCY = v_sync.TIC_SEC / static_cast<float>(dt);
      v_sync.CURRENT_SYNC = v_sync.current_cr;
      v_sync.SYNC_EVENT = true; 
    }
    else
    {
      v_sync.no_sync_pulses++;
      if(v_sync.no_sync_pulses > 6)
      {        
        SYNC_FREQUENCY = 0;
        v_sync.sync_pulses = 0;
        v_sync.Operating_mode = EOperating_mode::NO_SYNC;
      }
    }
  }
  
}  

void CPULS::start() 
{  
  A_Cur_tic = A_Max_tic;
  
  forcing_bridge = false;
  main_bridge    = false;
  
  N_Pulse = 1;   
  v_sync.SYNC_EVENT = false;  
  v_sync.no_sync_pulses = 0;
  v_sync.sync_pulses = 0;
  v_sync.Operating_mode = EOperating_mode::NO_SYNC;
  v_restoration.ind_d_avr = 0;
  
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
  LPC_TIM3->MR0 = PULSE_PERIOD;
  LPC_TIM3->MR1 = PULSE_PERIOD + PULSE_WIDTH;
  LPC_TIM3->CCR  = TIM3_CAPTURE_RI;      //Захват T3 по фронту CAP1 без прерываний 
  LPC_TIM3->MCR  = TIM3_COMPARE_MR0;     //Compare TIM3 с MR0 - enabled
  LPC_TIM3->MCR |= TIM3_COMPARE_MR1;     //Compare TIM3 с MR1 - enabled
  
  NVIC_EnableIRQ(TIMER3_IRQn);
  
}


