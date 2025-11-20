#pragma once

#include "lpc_eeprom.h"
#include "crc16.hpp"
#include "bool_name.hpp"

// Пространство имён глобальных констант
namespace G_CONST 
{
  constexpr unsigned short NUMBER_CHANNELS = 16;     // Количество какналов АЦП
  constexpr unsigned short SSID_PS_L = 20;           // Максимальная длина имени и пароля WiFi сети 20 символов
  constexpr unsigned short BYTES_RW_MAX = 3;         // Макс. количество байт чтения/записи по SPI (кол. структур в CSPI_ports)
  constexpr unsigned short BYTES_RW_REAL = 1;        // Фактическое количество байт чтения/записи по SPI (определяется схемой)
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
      unsigned short p_var1;
      float          c_var1;
      unsigned short p_var2;
      float          c_var2;
      unsigned short p_var3;
      float          c_var3;
      unsigned short p_var4;
      float          c_var4;
      unsigned short p_var5;
      float          c_var5;
      unsigned short p_var6;
      float          c_var6;
      unsigned short p_var7;
      float          c_var7;
      unsigned short p_var8;
      float          c_var8;
      unsigned short p_var9;
      float          c_var9;
      unsigned short p_var10;
      float          c_var10;
    } disp_c;
    unsigned char din_Pi0_invert;                               // 7 Признак инвертирования входов порта Pi0
    unsigned char din_Pi1_invert;                               // 8 Признак инвертирования входов порта Pi1
    unsigned char din_spi_invert[G_CONST::BYTES_RW_MAX];        // 9 Признак инвертирования SPI входов 
    unsigned char dout_spi_invert[G_CONST::BYTES_RW_MAX];       //10 Признак инвертирования SPI выходов
    signed short  power_shift;                                  //11 Сдвиг силового напряжения
    unsigned char ssid[G_CONST::SSID_PS_L];                     //12 Имя сети
    unsigned char password[G_CONST::SSID_PS_L];                 //13 Пароль
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
      .p_var1 = 100,
      .c_var1 = 0.5f,
      .p_var2 = 1380,
      .c_var2 = 13.8f,
      .p_var3 = 1086,
      .c_var3 = 0.5f,
      .p_var4 = 100,
      .c_var4 = 0.5f,  
      .p_var5 = 100,
      .c_var5 = 0.5f,
      .p_var6 = 100,
      .c_var6 = 0.5f,
      .p_var7 = 100,
      .c_var7 = 0.5f,
      .p_var8 = 100,
      .c_var8 = 0.5f,  
      .p_var9 = 100,
      .c_var9 = 0.5f,
      .p_var10 = 100,
      .c_var10 = 0.5f  
    },
    .din_Pi0_invert = 0,
    .din_Pi1_invert = 0,
    .din_spi_invert =  { 0,0,0 },
    .dout_spi_invert = { 0,0,0 },
    .power_shift = 0,
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
        