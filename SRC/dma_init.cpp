#include "dma_init.h"
#include "lpc177x_8x_gpdma.h"
#include "lpc177x_8x_clkpwr.h"
#include "IntPriority.h"

void DMA67_Init(){
  
  CLKPWR_ConfigPPWR (CLKPWR_PCONP_PCGPDMA, ENABLE);
  LPC_GPDMACH1->CConfig = 0;
  LPC_GPDMACH2->CConfig = 0;
  LPC_GPDMA->IntTCClear = 0xFF;
  LPC_GPDMA->IntErrClr = 0xFF;

  NVIC_DisableIRQ (DMA_IRQn);
  NVIC_SetPriority(DMA_IRQn, _DMA_IRQn_PRIORITY);
    
  GPDMA_Channel_CFG_Type GPDMACfg_6;
  GPDMA_Channel_CFG_Type GPDMACfg_7;
  
  GPDMACfg_6.ChannelNum = 6; // channel 6. Memory to Tx SPI
  GPDMACfg_6.TransferType = GPDMA_TRANSFERTYPE_M2P;
  //GPDMACfg1.SrcConn = 0;
  GPDMACfg_6.DstConn = GPDMA_CONN_SSP2_Tx;
  GPDMACfg_6.DMALLI = 0;
  GPDMA_Setup(&GPDMACfg_6);

  LPC_GPDMACH6->CControl |= GPDMA_DMACCxControl_SWidth(1); //2-byte
  LPC_GPDMACH6->CControl |= GPDMA_DMACCxControl_DWidth(1); //2-byte
  
  LPC_GPDMACH6->CConfig &= ~GPDMA_DMACCxConfig_IE;
  LPC_GPDMACH6->CConfig &= ~GPDMA_DMACCxConfig_ITC;
  
  GPDMACfg_7.ChannelNum = 7; // channel 7. Rx SPI to Memory
  GPDMACfg_7.TransferType = GPDMA_TRANSFERTYPE_P2M;
  GPDMACfg_7.SrcConn = GPDMA_CONN_SSP2_Rx;;
  //GPDMACfg_7.DstConn = 0;
  GPDMACfg_7.DMALLI = 0;
  GPDMA_Setup(&GPDMACfg_7);
  
  LPC_GPDMACH7->CControl |= GPDMA_DMACCxControl_SWidth(1); //2-byte
  LPC_GPDMACH7->CControl |= GPDMA_DMACCxControl_DWidth(1); //2-byte 

  LPC_GPDMACH7->CConfig |= GPDMA_DMACCxConfig_IE;
  LPC_GPDMACH7->CConfig |= GPDMA_DMACCxConfig_ITC;

  NVIC_EnableIRQ (DMA_IRQn);
  
}


