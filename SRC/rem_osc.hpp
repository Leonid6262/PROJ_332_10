#pragma once

#include "spi_init.hpp"
#include "controllerDMA.hpp"
#include <vector>

class CREM_OSC
{ 
  
private: 
  
  CDMAcontroller& rContDMA;
  void init_dma();
  
  static constexpr unsigned int Hz_SPI  = 1000000;
  static constexpr unsigned int bits_tr = 16;

  static constexpr unsigned int IOCON_SPI  = 0x02;
  static constexpr unsigned int IOCON_PORT = 0x00;
  static constexpr unsigned int PROG_CS    = 1UL << 3;
  static constexpr unsigned int RXDMAE     = 1UL << 0;
  static constexpr unsigned int TXDMAE     = 1UL << 1;
  static constexpr unsigned int P1_27      = 1UL << 27;
  
  static constexpr unsigned short TRANSACTION_LENGTH = 11; 
  static constexpr unsigned char NUMBER_TRACKS = 10;
  static constexpr unsigned char NAME_LENGTH = 5+1;            //максимальная длина имени трека
  
public:
  
  struct SSET_init {
    signed short* pData[NUMBER_TRACKS];
    const char Names[NUMBER_TRACKS][NAME_LENGTH];
    unsigned short d_100p[NUMBER_TRACKS];
    unsigned short SNboard_number;
  };
  const SSET_init& set_init;
  
  CREM_OSC(CDMAcontroller&, SSET_init&);
  
  static signed short tx_dma_buffer[TRANSACTION_LENGTH];
  static signed short rx_dma_buffer[TRANSACTION_LENGTH];
  
  void start_dma_transfer();
};

