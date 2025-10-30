#pragma once

#include "settings_eep.hpp"

class CSPI_ports{
  
public:  
  
  //--- Входы контроллера ---
  inline bool DinControllerB0() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_DIN_CPU)].b0;}
  inline bool Stator_Key()      const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_DIN_CPU)].b1;}
  inline bool DinControllerB2() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_DIN_CPU)].b2;}
  inline bool DinControllerB3() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_DIN_CPU)].b3;}
  inline bool IN1()             const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_DIN_CPU)].b4;}
  inline bool IN2()             const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_DIN_CPU)].b5;}
  inline bool IN3()             const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_DIN_CPU)].b6;}
  inline bool DinControllerB7() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_DIN_CPU)].b7;}
  
  //--- Выходы контроллера ---
  inline void Lamp_REDY         (bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_DOUT_CPU)].b0 = state;}
  inline void System_FAILURE    (bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_DOUT_CPU)].b1 = state;}
  inline void Excitation_Applied(bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_DOUT_CPU)].b2 = state;}
  inline void START_Premission  (bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_DOUT_CPU)].b3 = state;}
  inline void ON_Shunt_HVS      (bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_DOUT_CPU)].b4 = state;}
  inline void DoutControllerB5  (bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_DOUT_CPU)].b5 = state;}
  inline void DoutControllerB6  (bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_DOUT_CPU)].b6 = state;}
  inline void DoutControllerB7  (bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_DOUT_CPU)].b7 = state;}

  //--- Входы S600 ---
  inline bool DinEx1B0b0() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_0_DIN_EX1)].b0;}
  inline bool DinEx1B0b1() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_0_DIN_EX1)].b1;}
  inline bool DinEx1B0b2() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_0_DIN_EX1)].b2;}
  inline bool DinEx1B0b3() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_0_DIN_EX1)].b3;}
  inline bool DinEx1B0b4() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_0_DIN_EX1)].b4;}
  inline bool DinEx1B0b5() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_0_DIN_EX1)].b5;}
  inline bool DinEx1B0b6() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_0_DIN_EX1)].b6;}
  inline bool DinEx1B0b7() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_0_DIN_EX1)].b7;}
  
  inline bool DinEx1B2b0() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_1_DIN_EX1)].b0;}
  inline bool DinEx1B2b1() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_1_DIN_EX1)].b1;}
  inline bool DinEx1B2b2() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_1_DIN_EX1)].b2;}
  inline bool DinEx1B2b3() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_1_DIN_EX1)].b3;}
  inline bool DinEx1B2b4() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_1_DIN_EX1)].b4;}
  inline bool DinEx1B2b5() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_1_DIN_EX1)].b5;}
  inline bool DinEx1B2b6() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_1_DIN_EX1)].b6;}
  inline bool DinEx1B2b7() const {return UData_din_f[static_cast<char>(EBytesDinNumber::BYTE_1_DIN_EX1)].b7;}
  
  //--- Выходы S600 ---
  inline void DoutEx1b0(bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_0_DOUT_EX1)].b0 = state;}
  inline void DoutEx1b1(bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_0_DOUT_EX1)].b1 = state;}
  inline void DoutEx1b2(bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_0_DOUT_EX1)].b2 = state;}
  inline void DoutEx1b3(bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_0_DOUT_EX1)].b3 = state;}
  inline void DoutEx1b4(bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_0_DOUT_EX1)].b4 = state;}
  inline void DoutEx1b5(bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_0_DOUT_EX1)].b5 = state;}
  inline void DoutEx1b6(bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_0_DOUT_EX1)].b6 = state;}
  inline void DoutEx1b7(bool state) {UData_dout[static_cast<char>(EBytesDoutNumber::BYTE_0_DOUT_EX1)].b7 = state;}
  
  CSPI_ports();
  
  //Номера считываемых байтов последовательности
  enum class EBytesDinNumber
  {
    BYTE_DIN_CPU   = 0,     // Номер байта входов контроллера (всегда ноль, так как, первый в последовательности) 
    BYTE_0_DIN_EX1 = 1,     // Нулевой байт входов 1-й платы расширения  
    BYTE_1_DIN_EX1 = 2      // Первый байт входов 1-й платы расширения
  };
  
  //Номера записываемых байтов последовательности
  enum class EBytesDoutNumber
  {
    BYTE_1_DOUT_EX1 = 0,    // Первый байт выходов 1-й платы расширения (в S600 2/1 - отсутствует)
    BYTE_0_DOUT_EX1 = 1,    // Нулевой байт выходов 1-й платы расширения
    BYTE_DOUT_CPU   = 2     // Номер байта выходов контроллера (всегда (_BYTES_RW-1)), так как, замыкает последовательность)
  };
  
  /*                    Последовательность цикла:
                        
        MISO: <- byte_din_cpu  <- byte_0_din_ex1  <- byte_1_din_ex1
                      0                 1                  2
        MOSI: -> byte_dout_cpu -> byte_0_dout_ex1 -> byte_1_dout_ex1(*)
                      2                 1                  0

        (*) При использовании S600 2/1, byte_1_dout_ex1 - выходит в эфир     
  */
  
  union
  {
    unsigned char  all;               
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
  }UData_dout[G_CONST::BYTES_RW_MAX];               //Данные для записи в д.выходы

  unsigned char data_din[G_CONST::BYTES_RW_MAX];    //Входные данные din портов 
  
  union
  {
    unsigned char  all;
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
  }UData_din_f[G_CONST::BYTES_RW_MAX];               //Данные din портов после фильтра
  
  void rw();
  
private: 
  
  static constexpr unsigned short N_BITS = 8;                                   // Количество бит в портах
  static const unsigned int const_integr_spi[G_CONST::BYTES_RW_MAX][N_BITS];    // Постоянные интегрирования
  signed int integrator[G_CONST::BYTES_RW_MAX][N_BITS];                         // Интегратор   
  unsigned int prev_TC0;                                                        // Значение таймера на предыдыущем цикле
   
  static constexpr unsigned int IOCON_SPI = 0x02;
  static constexpr unsigned int Hz_SPI    = 800000;
  static constexpr unsigned int bits_tr   = 8;
  
  static constexpr unsigned int CR0_CPOL_HI     = 1UL << 6;
  static constexpr unsigned int CR0_CPHA_SECOND = 1UL << 7;
  static constexpr unsigned int TIC_ms = 10000;
  static constexpr unsigned int OUT_E = 1UL << 8;
  static constexpr unsigned int HOLD  = 1UL << 7;

};


