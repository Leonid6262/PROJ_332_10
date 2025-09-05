#pragma once

#include "spi_init.hpp"
#include "controllerDMA.hpp"

class CREM_OSC
{ 
  
private: 
  
  CDMAcontroller& rContDMA;
  
  static constexpr unsigned int Hz_SPI  = 1000000;
  static constexpr unsigned int bits_tr = 16;
  
  static constexpr unsigned int IOCON_SPI  = 0x02;
  static constexpr unsigned int IOCON_PORT = 0x00;
  static constexpr unsigned int PROG_CS    = 1UL << 3;
  static constexpr unsigned int RXDMAE     = 1UL << 0;
  static constexpr unsigned int TXDMAE     = 1UL << 1;
  
  static constexpr unsigned short TRANSACTION_LENGTH = 11; 
 
public:
  
  CREM_OSC(CDMAcontroller&);
  
  static signed short tx_dma_buffer[TRANSACTION_LENGTH];
  static signed short rx_dma_buffer[TRANSACTION_LENGTH];
  
  void init_dma();
  void start_dma_transfer();
};

