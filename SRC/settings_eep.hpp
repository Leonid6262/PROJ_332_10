#pragma once

#include "lpc_eeprom.h"
#include "crc16.hpp"
#include "bool_name.hpp"

// Пространство имён глобальных констант
namespace G_CONST 
{
  constexpr unsigned short NUMBER_CHANNELS = 16;     //Количество какналов АЦП
  constexpr unsigned short BYTES_RW = 3;             //Количество байт чтения/записи по SPI (один в контроллере, два на S600)
  constexpr unsigned short SSID_PS_L = 20;          // Максимальная длина имени и пароля WiFi сети 20 символов
}

class CEEPSettings {
  
private:
  // --- Структура уставок ---
  #pragma pack(push, 2) // Выравнивание по 2 байта
  struct WorkSettings 
  {
    unsigned short checkSum;                            // 0 Контрольная сумма
    unsigned short SNboard_date;                        // 1 Серийный номер платы - дата (ст.б - месяц, мл.б - год)
    unsigned short SNboard_number;                      // 2 Серийный номер платы - порядковый номер (99.99.0999)
    signed short shift_adc[G_CONST::NUMBER_CHANNELS];   // 3 Смещения АЦП 
    float incline_adc[G_CONST::NUMBER_CHANNELS];        // 4 Наклон
    signed short shift_dac0;                            // 5 Смещение DAC0
    struct Disp_c                                       // 6 Коэффициенты для отображения: p_NAME в % (дискрет/100%)
    {                                                   //                                 c_NAME в единицах СИ
      unsigned short p_ROTOR_CURRENT;
      float          c_ROTOR_CURRENT;
      unsigned short p_STATOR_CURRENT;
      float          c_STATOR_CURRENT;
      unsigned short p_ROTOR_VOLTAGE;
      float          c_ROTOR_VOLTAGE;
      unsigned short p_STATOR_VOLTAGE; 
      float          c_STATOR_VOLTAGE;
    } disp_c;
    unsigned char din_Pi0_invert;                       // 7 Признак инвертирования входов порта Pi0
    unsigned char din_Pi1_invert;                       // 8 Признак инвертирования входов порта Pi1
    unsigned char din_spi_invert[G_CONST::BYTES_RW];    // 9 Признак инвертирования SPI входов 
    unsigned char dout_spi_invert[G_CONST::BYTES_RW];   //10 Признак инвертирования SPI выходов
    unsigned char ssid[G_CONST::SSID_PS_L];             //11 Имя сети
    unsigned char password[G_CONST::SSID_PS_L];         //12 Пароль
    // Добавляя новые уставки сюда, не забывайте обновлять defaultSettings ниже!!!
  };
  #pragma pack(pop)
  //  Статические константные уставки по умолчанию (во Flash) ---
  // 'static const inline' позволяет определить ее прямо здесь, в .h файле.
  #pragma pack(push, 2) // Выравнивание по 2 байта
  static const inline WorkSettings defaultSettings 
  {
    .checkSum = 0x0000, 
    .SNboard_date = 0x6363,
    .SNboard_number = 999,
    .shift_adc = 
    {
      2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047,
      2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047
    },
    .incline_adc = 
    {
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    },
    .shift_dac0 = 0,
    .disp_c = 
    {
      .p_ROTOR_CURRENT  = 2047,
      .c_ROTOR_CURRENT  = 0.5f,
      .p_STATOR_CURRENT = 2047,
      .c_STATOR_CURRENT = 0.5f,
      .p_ROTOR_VOLTAGE  = 2047,
      .c_ROTOR_VOLTAGE  = 0.5f,
      .p_STATOR_VOLTAGE = 2047,
      .c_STATOR_VOLTAGE = 0.5f  
    },
    .din_Pi0_invert = 0,
    .din_Pi1_invert = 0,
    .din_spi_invert =  { 0,0,0 },
    .dout_spi_invert = { 0,0,0 },
    .ssid = "NetName",
    .password = "Password"
  };
  #pragma pack(pop)
  
  // Текущий набор уставок, хранящийся в RAM ---
  WorkSettings settings;
  
  // Механизмы Singleton ---
  CEEPSettings(); // Приватный конструктор
  CEEPSettings(const CEEPSettings&) = delete; // Запрещаем копирование
  CEEPSettings& operator=(const CEEPSettings&) = delete; // Запрещаем присваивание

  // Приватные методы для работы с EEP  ---
  StatusRet readFromEEPInternal(WorkSettings& outSettings);      // Чтение в WorkSettings
  void writeToEEPInternal(WorkSettings& inSettings);             // Запись из WorkSettings
  
  CCRC16* pCRC16;

  void EEP_init(void);
  void EEPr(uint16_t page_offset, uint16_t page_address, void* data, EEPROM_Mode_Type mode, uint32_t count);
  void EEPw(uint16_t page_offset, uint16_t page_address, void* data, EEPROM_Mode_Type mode, uint32_t count);

public:
    
  // Публичный метод для получения единственного экземпляра ---
  static CEEPSettings& getInstance();
  
  // Загрузки/сохранения уставок ---
  StatusRet loadSettings();     // Загружает из EEP. Если ошибка CRC - остаются дефолтные.
  void saveSettings();          // Сохраняет текущие уставки в EEP.
  
  // Прочитать/Изменить уставку ---
  inline WorkSettings& getSettings() 
  {
    return settings;
  }
  
  void init_EEP(CCRC16*);   
  
};
