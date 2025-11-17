#pragma once

#include "LPC407x_8x_177x_8x.h"
#include "rem_osc.hpp"
#include "conv_and_calc.hpp"

class CSIFU
{ 
public:
  
  CSIFU(CPULSCALC&);
  
  CPULSCALC& rPulsCalc;
  
  float SYNC_FREQUENCY;
  
  signed short A_Task_tick;

  void init_and_start();
  void rising_puls();
  void faling_puls();

  void start_forcing_bridge();
  void start_main_bridge();
  void stop();
 
private:
 
  static const unsigned char pulses[];
  static const signed  short offsets[];
  
  bool forcing_bridge;
  bool main_bridge;
  
  unsigned char N_Pulse;
    
  signed short A_Cur_tick; 
  signed short A_Prev_tick;
  signed short d_Alpha;
  
  static constexpr signed short A_Max_tick   = 8333;
  static constexpr signed short A_Min_tick   = 1667;
  static constexpr signed short d_A_Max_tick = 5;//278;
  
  void control_sync();
  
  enum class EOperating_mode {
    NO_SYNC,           
    RESYNC,          
    NORMAL            
  };
  
  struct SyncState 
  {
    static constexpr float SYNC_F_MIN = 49.0f;
    static constexpr float SYNC_F_MAX = 51.0f;
    static constexpr float DT_MIN = 19608;
    static constexpr float DT_MAX = 20408;
    static constexpr signed short   _0gr = 0;
    static constexpr signed short  _60gr = 3333;
    static constexpr signed short _120gr = 6667;
    static constexpr signed short _180gr = 10000;
    unsigned int sync_timing;
    unsigned int CURRENT_SYNC;
    unsigned int current_cr;                            // Текущие данные захвата таймера
    unsigned int previous_cr;                           // Предыдущие данные захвата таймера
    unsigned short no_sync_pulses;                      // Количество пульсов отсутствия события захвата
    unsigned short sync_pulses;                         // Количество пульсов с событиями захвата
    
    static constexpr float TIC_SEC = 1000000.0;               
    EOperating_mode Operating_mode;    
     
    bool SYNC_EVENT;
    
  } v_sync;
  
  static constexpr unsigned int IOCON_P1_PWM = 0x03;                         // Тип портов - PWM
  static constexpr unsigned int PWM_div_0    = 60;                           // Делитель частоты
    
  static constexpr unsigned int IOCON_P_PWM  = 0x03;                         //Тип портов - PWM
  static constexpr unsigned int IOCON_P_PORT = 0x00;                         //Тип портов - Port
  static constexpr unsigned int P1_2 = 0x02;                                 //Port1:2
  static constexpr unsigned int P1_3 = 0x03;                                 //Port1:3  
  
  static constexpr unsigned int PWM_WIDTH        = 10;                          //us
  static constexpr unsigned int PULSE_WIDTH      = 550;//560;                         //us
  static constexpr unsigned int N_PULSES         = 6;
  static constexpr unsigned int OFF_PULSES       = 0x003F0000;                   //Импульсы в порту
  static constexpr unsigned int FIRS_PULS_PORT   = 16;                           //1-й импульс в порту 
  
  static constexpr unsigned int PCR_PWMENA1      = 0x200;
  static constexpr unsigned int PCR_PWMENA2      = 0x400;
  static constexpr unsigned int LER_012          = 0x07;
  static constexpr unsigned int COUNTER_CLR      = 0x00;
  static constexpr unsigned int COUNTER_RESET    = 0x02;
  static constexpr unsigned int COUNTER_STOP     = 0x0B;
  static constexpr unsigned int COUNTER_START    = 0x09;
  
  static constexpr unsigned int TIM3_TCR_START   = 0x01;

  static constexpr unsigned int TIM3_COMPARE_MR0 = 0x01;
  static constexpr unsigned int TIM3_COMPARE_MR1 = 0x08;
  static constexpr unsigned int TIM3_CAPTURE_RI  = 0x08;
  static constexpr unsigned int IOCON_T3_CAP1    = 0x23;
  
};
