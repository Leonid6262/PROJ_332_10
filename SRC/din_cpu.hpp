#pragma once

class CDin_cpu
{
  
private:
  
  static constexpr unsigned short N_BITS = 8;          // Количество бит в портах
  static constexpr unsigned int TIC_ms = 10000;
  
  static constexpr unsigned int B0_PORT0_IN = 15;      // 0-й бит порта. Начало Pi0 в PORT2 контроллера
  static constexpr unsigned int B0_PORT1_IN = 11;      // 0-й бит порта. Начало 1-й группы Pi1 в PORT2 контроллера
  static constexpr unsigned int MASK_G1   = 0x07;      // Маска бит 1-й группы
  static constexpr unsigned int BITS_G1      = 3;      // Битов в 1-й группе
  static constexpr unsigned int B3_PORT1_IN = 10;      // 3-й бит порта. Начало 2-й группы Pi1 в PORT0 контроллера
  static constexpr unsigned int MASK_G2   = 0xF8;      // Маска бит 2-й группы
  
  signed int integrator[N_BITS];                       //Интеграторы
  unsigned int prev_TC0;                               //Значение таймера на предыдыущем цикле
  
  // Постоянные времени интегрирования фильтра (ms первый множитель) din портов. 
  // TIC_ms = 10000 дискрет таймера на 1ms.  
  // То есть, 50*TIC_ms = 50ms, 0*TIC_ms - нет фильтрации, и т.п. 
  static constexpr unsigned int cConst_integr_Pi0[N_BITS] = 
  {
    50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms
  };
  
public:  
  
  inline bool Bl_Contact_Q1()   const {return UData_din_f_Pi0.b0;} // Блок контакт Q1
  inline bool Reg_Auto()        const {return UData_din_f_Pi0.b1;} // Режим "Автоматический"
  inline bool Reg_Drying()      const {return UData_din_f_Pi0.b2;} // Режим "Сушка"
  inline bool Setting_More()    const {return UData_din_f_Pi0.b3;} // Задание "Больше"
  inline bool Setting_Less()    const {return UData_din_f_Pi0.b4;} // Задание "Меньше"
  inline bool CU_from_testing() const {return UData_din_f_Pi0.b5;} // Сигнал "CU" от платы опробования
  inline bool Bl_HVS_NO()       const {return UData_din_f_Pi0.b6;} // NO блок контакт ВВ
  inline bool Bl_HVS_NC()       const {return UData_din_f_Pi0.b7;} // NC блок контакт ВВ
  
  inline bool NAME_PI1_B0() const {return UData_din_Pi1.b0;}
  inline bool NAME_PI1_B1() const {return UData_din_Pi1.b1;}
  inline bool NAME_PI1_B2() const {return UData_din_Pi1.b2;}
  inline bool NAME_PI1_B3() const {return UData_din_Pi1.b3;}
  inline bool NAME_PI1_B4() const {return UData_din_Pi1.b4;}
  inline bool NAME_PI1_B5() const {return UData_din_Pi1.b5;}
  inline bool NAME_PI1_B6() const {return UData_din_Pi1.b6;}
  inline bool NAME_PI1_B7() const {return UData_din_Pi1.b7;}

  CDin_cpu();
  
  unsigned char data_din_Pi0;    //Входные данные порта Pi0 (изолированного)
  unsigned char data_din_Pi1;    //Входные данные порта Pi1 (не изолированного)
  
  union
  {
    unsigned char  all;         //Выходные данные порта Pi0 после фильтра и инверсии
    struct
    {  
      unsigned char   b0 : 1;
      unsigned char   b1 : 1;
      unsigned char   b2 : 1;
      unsigned char   b3 : 1;
      unsigned char   b4 : 1;
      unsigned char   b5 : 1;
      unsigned char   b6 : 1;
      unsigned char   b7 : 1;
    };
  } UData_din_f_Pi0;
  
  union
  {
    unsigned char  all;          //Выходные данные порта Pi1 после инверсии
    struct
    {  
      unsigned char   b0 : 1;
      unsigned char   b1 : 1;
      unsigned char   b2 : 1;
      unsigned char   b3 : 1;
      unsigned char   b4 : 1;
      unsigned char   b5 : 1;
      unsigned char   b6 : 1;
      unsigned char   b7 : 1;
    };
  } UData_din_Pi1;
  
  void input_Pi0();
  void input_Pi1();
  
};

