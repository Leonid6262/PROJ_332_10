#pragma once

#include "lpc407x_8x_177x_8x.h"
#include "lpc177x_8x_gpdma.h"
#include "lpc177x_8x_clkpwr.h"
#include "dma_config.hpp"

class CDMAcontroller {
  
private:
  
  static constexpr unsigned char MaxChannels = 8;
  static constexpr unsigned int TransferSize_MASK = 0x00000FFF;

  static constexpr LPC_GPDMACH_TypeDef* const DmaChannels[MaxChannels] = {
    LPC_GPDMACH0,
    LPC_GPDMACH1,
    LPC_GPDMACH2,
    LPC_GPDMACH3,
    LPC_GPDMACH4,
    LPC_GPDMACH5,
    LPC_GPDMACH6,
    LPC_GPDMACH7
  };
  
  static constexpr const volatile void* LUTPerAddr[] = {
    nullptr,                            // Reserved
    &LPC_MCI->FIFO,                     // SD Card
    &LPC_SSP0->DR,                      // SSP0 Tx
    &LPC_SSP0->DR,                      // SSP0 Rx
    &LPC_SSP1->DR,                      // SSP1 Tx
    &LPC_SSP1->DR,                      // SSP1 Rx
    &LPC_SSP2->DR,                      // SSP2 Tx
    &LPC_SSP2->DR,                      // SSP2 Rx
    &LPC_ADC->GDR,                      // ADC
    &LPC_DAC->CNR,                      // DAC
    &LPC_UART0->THR,                    // UART0 Tx
    &LPC_UART0->RBR,                    // UART0 Rx
    &LPC_UART1->THR,                    // UART1 Tx
    &LPC_UART1->RBR,                    // UART1 Rx
    &LPC_UART2->THR,                    // UART2 Tx
    &LPC_UART2->RBR,                    // UART2 Rx
    &LPC_TIM0->MR0,                     // MAT0.0
    &LPC_TIM0->MR1,                     // MAT0.1
    &LPC_TIM1->MR0,                     // MAT1.0
    &LPC_TIM1->MR1,                     // MAT1.1
    &LPC_TIM2->MR0,                     // MAT2.0
    &LPC_TIM2->MR1,                     // MAT2.1
    &LPC_I2S->TXFIFO,                   // I2S Tx
    &LPC_I2S->RXFIFO,                   // I2S Rx
    nullptr,                            // Reserved
    nullptr,                            // Reserved
    &LPC_UART3->THR,                    // UART3 Tx
    &LPC_UART3->RBR,                    // UART3 Rx
    &LPC_UART4->THR,                    // UART4 Tx
    &LPC_UART4->RBR,                    // UART4 Rx
    &LPC_TIM3->MR0,                     // MAT3.0
    &LPC_TIM3->MR1                      // MAT3.1
  };
  
public:

  CDMAcontroller();
  
  void init_M2P2M_Channel(const SChannelConfig*);
 
  
  void enableIrq();
  void disableIrq();

  void StartTxTransfer(unsigned char, unsigned int, unsigned int);
  void StartRxTransfer(unsigned char, unsigned int, unsigned int);

};


