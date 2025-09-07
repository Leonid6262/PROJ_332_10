#include "rem_osc.hpp"
#include "dma_config.hpp"
#include "bool_name.hpp"
#include "system_LPC177x.h"

// Старт передачи по DMA
void CREM_OSC::start_dma_transfer()
{
  // Старт чтения
  rContDMA.StartRxTransfer(CDMAcontroller::ChannelMap::SPI2_Rx_Channel, 
                           static_cast<unsigned int>((long long)rx_dma_buffer),                           
                           TRANSACTION_LENGTH);
  // Старт записи
  rContDMA.StartTxTransfer(CDMAcontroller::ChannelMap::SPI2_Tx_Channel, 
                           static_cast<unsigned int>((long long)tx_dma_buffer),                           
                           TRANSACTION_LENGTH);
}

/* Конфигурирование используемых каналов DMA (карту каналов с.м. в controllerDMA.hpp) */
/* BurstSize = 4. При EWidth::Short → 16 бит → 4 × 2 байта = 8 байт, при FIFO = 16 байт, с запасом */
void CREM_OSC::init_dma()
{
  // Конфигурация канала записи
  CDMAcontroller::SChannelConfig cfg_ch_tx
  {
    CDMAcontroller::ChannelMap::SPI2_Tx_Channel,  // Номер канала
    CDMAcontroller::ETransferType::TYPE_M2P,      // Тип канала
    CDMAcontroller::EConnNumber::SSP2_Tx,         // Номер периферийного подключения
    CDMAcontroller::DmaBurst::SIZE_4,             // Количество единичных элементов транзакции 
    CDMAcontroller::EWidth::SHORT,                // Размер единичного элемента
    OFF                                           // Разрешение/запрет события окончания передачи (ON/OFF)
  };
  
  rContDMA.init_M2P2M_Channel(&cfg_ch_tx); 
  
  // Конфигурация канала чтения
  CDMAcontroller::SChannelConfig cfg_ch_rx
  {
    CDMAcontroller::ChannelMap::SPI2_Rx_Channel,  // Номер канала
    CDMAcontroller::ETransferType::TYPE_P2M,      // Тип канала
    CDMAcontroller::EConnNumber::SSP2_Rx,         // Номер периферийного подключения
    CDMAcontroller::DmaBurst::SIZE_4,             // Количество единичных элементов транзакции
    CDMAcontroller::EWidth::SHORT,                // Размер единичного элемента
    OFF                                           // Разрешение/запрет события окончания приёма (ON/OFF)
  };
  
  rContDMA.init_M2P2M_Channel(&cfg_ch_rx); 
  
}

CREM_OSC::CREM_OSC(CDMAcontroller& rContDMA) : rContDMA(rContDMA)
{   
  LPC_IOCON->P5_0 = IOCON_SPI;       //MOSI
  LPC_IOCON->P5_1 = IOCON_SPI;       //MISO
  LPC_IOCON->P5_2 = IOCON_SPI;       //SCK   
  
  LPC_IOCON->P5_3 = IOCON_PORT;      //Програмный CS  
  LPC_GPIO5->DIR |= PROG_CS;
  LPC_GPIO5->CLR  = PROG_CS;
  
  // Используется SPI-2
  LPC_SC->PCONP |= CLKPWR_PCONP_PCSSP2;
  LPC_SSP2->CR0 = 0;
  LPC_SSP2->CR0 = SPI_Config::CR0_DSS(bits_tr);// | CR0_FRF_SPI;  | CR0_CPOL_HI; | CR0_CPHA_SECOND; 
  LPC_SSP2->CR1 = 0;
  SPI_Config::set_spi_clock(LPC_SSP2, Hz_SPI, PeripheralClock );
  LPC_SSP2->CR1 |= SPI_Config::CR1_SSP_EN ;
  LPC_SSP2->DMACR = TXDMAE | RXDMAE; // Приём и передача по DMA
  
  init_dma();
}
