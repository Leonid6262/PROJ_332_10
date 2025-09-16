#pragma once

#include "settings_eep.hpp"
#include "spi_init.hpp"

class CADC{ 
  
private:
  
  static constexpr unsigned int D_MODE_PULLUP   = 0x02 <<3 ;
  static constexpr unsigned int IOCON_SPI       = 0x03;
  static constexpr unsigned int Hz_SPI          = 1000000;
  static constexpr unsigned int bits_tr         = 16;
  
  // Данные для запуска конвертации (формат - см. док.)
  static const unsigned short cN_CH[G_CONST::NUMBER_CHANNELS];
  //#pragma inline = forced
  inline unsigned short setChannel(unsigned short n_ch)          // Запуск текущей и считывание результата предыдущих конвертаций
  {
    LPC_SSP1->DR = cN_CH[n_ch];
    while (LPC_SSP1->SR & SPI_Config::SR_BSY);
    return LPC_SSP1->DR;
  }
  //#pragma inline = forced
  inline void getData(unsigned short raw_adc_data)               // Обработка и фиксация полученных данных измерения
  {
    unsigned short tmp_Nch = (raw_adc_data & 0xF000) >> 12;
    if( tmp_Nch < G_CONST::NUMBER_CHANNELS ) 
    {
      data[tmp_Nch] = 
        ((raw_adc_data & 0x0FFF) - CEEPSettings::getInstance().getSettings().shift_adc[tmp_Nch]) * 
          (1.0f + CEEPSettings::getInstance().getSettings().incline_adc[tmp_Nch]);
    }
  }

public:
  
  CADC();
  
  // Список имён каналов
  enum EADC_NameCh{
    ROTOR_CURRENT       = 0,
    STATOR_VOLTAGE      = 1,
    ROTOR_VOLTAGE       = 2,
    LEAKAGE_CURRENT     = 3,    
    STATOR_CURRENT      = 4,             
    LOAD_NODE_CURRENT   = 5,                                       
    EXTERNAL_SETTINGS   = 6,             
    Name_ch07,            
    Name_ch08,            
    Name_ch09,            
    Name_ch10,            
    Name_ch11,            
    Name_ch12,           
    Name_ch13,           
    Name_ch14,            
    ch_HRf              = 15     // Промежуточный канал (0.5 Ref)
  };
  
  // Массив измеренных значений.
  // Пример доступа - adc.data[CADC::Rotor_current] 
  signed short data[G_CONST::NUMBER_CHANNELS];
  
  // Тип параметров short вмесо ожидаемого EADC_NameCh,
  // обеспечивает любой порядок аргументов при вызове любого метода
  void conv(short);
  void conv(short, short);
  void conv(short, short, short);
  void conv(short, short, short, short); 
  void conv(short, short, short, short, short);
  void conv(short, short, short, short, short, short); 
  void conv(short, short, short, short, short, short, short); 
//    .
//    .
//    .
//  void conv(short, short, short, short, short, ... short); // До 16 перегружаемых методов
  
};
