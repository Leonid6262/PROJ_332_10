#include "rem_osc.hpp"
#include "dma_config.hpp"
#include "bool_name.hpp"
#include "system_LPC177x.h"

CREM_OSC::CREM_OSC(CDMAcontroller& r_cont_dma) : rContDMA(r_cont_dma)
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
}

// Старт передачи по DMA
void CREM_OSC::start_dma_transfer()
{
  // Старт чтения
  rContDMA.StartRxTransfer(DMA_MAP::SPI2_Rx_Channel, 
                           static_cast<unsigned int>((long long)rx_dma_buffer),                           
                           TRANSACTION_LENGTH);
  // Старт записи
  rContDMA.StartTxTransfer(DMA_MAP::SPI2_Tx_Channel, 
                           static_cast<unsigned int>((long long)tx_dma_buffer),                           
                           TRANSACTION_LENGTH);
}

/* Конфигурирование используемых каналов DMA (карту каналов с.м. в dma_config.hpp) */
/* BurstSize = 4. При EWidth::Short → 16 бит → 4 × 2 байта = 8 байт, при FIFO = 16 байт, с запасом */
void CREM_OSC::init_dma()
{
  // Конфигурация канала записи
  SChannelConfig cfg_ch_tx
  {
    DMA_MAP::SPI2_Tx_Channel,   // Номер канала из DMA_MAP
    ETransferType::TYPE_M2P,    // Тип канала из ETransferType
    GPDMA_CONN_SSP2_Tx,         // Номер периферийного подключения из Connection number definitions
    DmaBurst::SIZE_4,           // Количество единичных элементов транзакции из DmaBurst  
    EWidth::SHORT,              // Размер единичного элемента из EWidth
    OFF                         // Разрешение/запрет события окончания передачи (ON/OFF)
  };
  
  rContDMA.init_M2P2M_Channel(&cfg_ch_tx); 
  
  // Конфигурация канала чтения
  SChannelConfig cfg_ch_rx
  {
    DMA_MAP::SPI2_Rx_Channel,   // Номер канала из DMA_MAP
    ETransferType::TYPE_P2M,    // Тип канала из ETransferType
    GPDMA_CONN_SSP2_Rx,         // Номер периферийного подключения из Connection number definitions
    DmaBurst::SIZE_4,           // Количество единичных элементов транзакции из DmaBurst  
    EWidth::SHORT,              // Размер единичного элемента из EWidth
    OFF                         // Разрешение/запрет события окончания приёма (ON/OFF)
  };
  
  rContDMA.init_M2P2M_Channel(&cfg_ch_rx); 
  
}

