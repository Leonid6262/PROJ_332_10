#pragma once

#include "spi_init.hpp"
#include "controllerDMA.hpp"
#include "bool_name.hpp"
#include <vector>

class CREM_OSC
{ 
  
private: 
  
  CDMAcontroller& rContDMA;
    
  unsigned char number_actual_tracks;
  
  void init_SPI();
  void init_dma();
  unsigned char get_actual_number();
  void transfer_disp_c();
  void transfer_name();
  void transfer_mode();
  StatusRet transfer_SN_ID();
  StatusRet transfer_SSID();
  StatusRet transfer_Password();
    
  static constexpr unsigned short TRANSACTION_LENGTH = 11;                // Слово управления + 10 треков
  static constexpr unsigned char  NUMBER_TRACKS = TRANSACTION_LENGTH - 1; // Максимальное количество треков
  static constexpr unsigned char  NAME_LENGTH = 5 + 1;                    // Максимальная длина имени трека 5 символов
  static constexpr unsigned char  SSID_PS_L   = 20;                       // Максимальная длина имени и пароля WiFi сети 20 символов
  
  //Директивы передачи
  static constexpr unsigned short send_SSID  = 0x8000; //Передача SS_ID
  static constexpr unsigned short send_PASS  = 0x8001; //Передача Password
  static constexpr unsigned short send_SNID  = 0x8002; //Передача SN_ID
  static constexpr unsigned short send_CIND  = 0x8003; //Передача коэффициентов отображения
  
  static constexpr unsigned short NAME_CODES[NUMBER_TRACKS / 2] = {
    0x8004,  //Передача имён 0,1
    0x8005,  //Передача имён 2,3
    0x8006,  //Передача имён 4,5
    0x8007,  //Передача имён 6,7
    0x8008   //Передача имён 8,9
  };
  
  static constexpr unsigned short send_TRACKS = 0x8F00; //Передача данных треков
  
  static constexpr unsigned int Hz_SPI  = 1000000;
  static constexpr unsigned int bits_tr = 16;

  static constexpr unsigned int IOCON_SPI  = 0x02;
  static constexpr unsigned int IOCON_PORT = 0x00;
  static constexpr unsigned int PROG_CS    = 1UL << 3;
  static constexpr unsigned int RXDMAE     = 1UL << 0;
  static constexpr unsigned int TXDMAE     = 1UL << 1;
  static constexpr unsigned int P1_27      = 1UL << 27;
 
public:
  
  bool StatusESP32;
  
  // Режим работы
  enum class Operating_mode : unsigned char 
  {
    Access_point,
    Station       
  };
  // Структура инициализирующих значений
  struct SSET_init {
    signed short* pData[NUMBER_TRACKS];
    const char Names[NUMBER_TRACKS][NAME_LENGTH];
    unsigned short d_100p[NUMBER_TRACKS];
    Operating_mode mode;
    unsigned short SNboard_number;
    const char* pSSID;
    const char* pPassword;
  };
  const SSET_init& set_init;
  
  CREM_OSC(CDMAcontroller&, SSET_init&);
  
  static signed short tx_dma_buffer[TRANSACTION_LENGTH];
  static signed short rx_dma_buffer[TRANSACTION_LENGTH];
  
  void start_dma_transfer();
  
  // Как задавать ssid и password пока не ясно. При отсутствии панели оператора
  // и сетевых интерфейсов, возможно с ноутбука. Задавать с ПТ удовольствие так себе.
  static constexpr const char* ssid     = "SectorSoftware";
  static constexpr const char* password = "SoftwareSector";
};

