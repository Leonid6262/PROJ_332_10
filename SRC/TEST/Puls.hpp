#pragma once

#include "LPC407x_8x_177x_8x.h"
#include "rem_osc.hpp"
#include "adc.hpp"

class CPULS
{ 

private:
 
  static const char pulses[];
  
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
    unsigned int current_cr;                            // Текущие данные захвата таймера
    unsigned int previous_cr;                           // Предыдущие данные захвата таймера
    unsigned short no_sync_pulses;                       // Количество пульсов отсутствия события захвата
    unsigned short sync_pulses;                          // Количество пульсов с событиями захвата
    
    static constexpr float TIC_SEC = 1000000.0;   
            
    EOperating_mode Operating_mode;    
    float sync_f; 
    
    
  } v_sync;
  
  struct RestorationState 
  {
    static constexpr char PULS_AVR = 6;                                        // Пульсов усреднения
    static constexpr float freq = 50.0f;                                       // Частота сети
    static constexpr float pi = 3.141592653589793;
    
    float u_stat[PULS_AVR];
    float i_stat[PULS_AVR];
    char ind_d_avr;
    signed short u_stator_1;
    signed short u_stator_2;
    unsigned int timing_ustator_1;
    unsigned int timing_ustator_2;
    unsigned short dT_ustator;
    
    signed short i_stator_1;
    signed short i_stator_2;
    unsigned int timing_istator_1;
    unsigned int timing_istator_2;
    unsigned short dT_istator;
  } v_restoration;
  
  static constexpr unsigned int IOCON_P1_PWM = 0x03;                         // Тип портов - PWM
  static constexpr unsigned int PWM_div_0    = 60;                           // Делитель частоты
    
  static constexpr unsigned int IOCON_P_PWM  = 0x03;                            //Тип портов - PWM
  static constexpr unsigned int IOCON_P_PORT = 0x00;                            //Тип портов - Port
  static constexpr unsigned int P1_2 = 0x02;                                    //Port1:2
  static constexpr unsigned int P1_3 = 0x03;                                    //Port1:3  
  
  static constexpr unsigned int PWM_WIDTH        = 10;                          //us
  static constexpr unsigned int PULSE_WIDTH      = 560;                         //us
  static constexpr unsigned int PULSE_PERIOD     = 3333;                        //us
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
  
public:
  
  CPULS(CADC&);
  
  CADC& rAdc;
   
  bool forcing_bridge;
  bool main_bridge;
  
  unsigned char N_Pulse;
  
  float SYNC_FREQUENCY;
  signed short U_STATORA;  
  signed short I_STATORA;
 
  void start();
  void start_main_bridge();
  void start_puls();
  void stop_puls();
  void sin_restoration();
  void control_sync();
  void conv_adc();
  



  
};
