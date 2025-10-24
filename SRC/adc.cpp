#include "adc.hpp"
#include "system_LPC177x.h"
#include <math.h>

void CADC::conv(short c1)
{  
  setChannel(c1);
  setChannel(c1);
  getData(setChannel(ch_HRf));  //r-c1
}

void CADC::conv(short c1, short c2)
{
  setChannel(c1);
  setChannel(c2);
  getData(setChannel(ch_HRf));    //r-c1  
  getData(setChannel(ch_HRf));    //r-c2
}

void CADC::conv(short c1, short c2, short c3)
{
  setChannel(c1);
  setChannel(c2);  
  getData(setChannel(c3));      //r-c1 
  getData(setChannel(ch_HRf));  //r-c2
  getData(setChannel(ch_HRf));  //r-c3
}

//void ADC::conv(short c1, short c2, short c3, short c4)
//void ADC::conv(short c1, short c2, short c3, short c4, short c5)
//void ADC::conv(short c1, short c2, short c3, short c4, short c5, short c6)

void CADC::conv(short c1, short c2, short c3, short c4, short c5, short c6, short c7)
{
  setChannel(c1);
  setChannel(c2); 
  getData(setChannel(c3));      //r-c1          
  getData(setChannel(c4));      //r-c2 
  getData(setChannel(c5));      //r-c3
  getData(setChannel(c6));      //r-c4 
  getData(setChannel(c7));      //r-c5 
  getData(setChannel(ch_HRf));  //r-c6  
  getData(setChannel(ch_HRf));  //r-c7
}

CADC::CADC()
{  
  // SPI-1
  
  LPC_IOCON->P4_20  = D_MODE_PULLUP | IOCON_SPI; //SCK1
  LPC_IOCON->P4_21  = D_MODE_PULLUP | IOCON_SPI; //SSEL1
  LPC_IOCON->P4_22  = D_MODE_PULLUP | IOCON_SPI; //MISO1
  LPC_IOCON->P4_23  = D_MODE_PULLUP | IOCON_SPI; //MOSI1 

  LPC_SC->PCONP |= CLKPWR_PCONP_PCSSP1; 
  LPC_SSP1->CR0 = 0;
  LPC_SSP1->CR0 = bits_tr - 1; // (16 - 1) -> 16 bits 
  LPC_SSP1->CR1 = 0 ; 
  SPI_Config::set_spi_clock(LPC_SSP1, Hz_SPI, PeripheralClock );
  LPC_SSP1->CR1 |= SPI_Config:: CR1_SSP_EN ;
  
  unsigned short tmp_dat;
  LPC_SSP1->DR = (1UL << 12) | (1UL << 11); //0x1800 - manual mode and prog b0...b6
  while( LPC_SSP1->SR & SPI_Config::SR_RNE ) 
  {
    tmp_dat = LPC_SSP1->DR;
  }  
  LPC_SSP1->DR = cN_CH[0];  
  while (LPC_SSP1->SR & SPI_Config::SR_BSY)
  {
    tmp_dat = LPC_SSP1->DR;  
  }
  while( LPC_SSP1->SR & SPI_Config::SR_RNE ) 
  {
    tmp_dat = LPC_SSP1->DR;
  }  
};

