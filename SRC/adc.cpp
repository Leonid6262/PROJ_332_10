#include "adc.hpp"
#include "system_LPC177x.h"
#include <math.h>

/* cN_CH - данные для запуска конвертации (формат - см. док.) */
const unsigned short CADC::cN_CH[G_CONST::NUMBER_CHANNELS] = 
{
  0x1000, 0x1080, 0x1100, 0x1180, 0x1200, 0x1280, 0x1300, 0,0,0,0,0,0,0,0, 0x1780
};

void CADC::conv(short c1)
{  
  setChannel(c1);
  setChannel(ch_HRf);
  getData(setChannel(ch_HRf));  //r-c1
}


signed short tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
void CADC::conv(short c1, short c2)
{
  //setChannel(c1);
  //setChannel(ch_HRf);
  //getData(setChannel(c2));      //r-c1
  //setChannel(ch_HRf);
  //getData(setChannel(ch_HRf));  //r-c2
  setChannel(0);
  setChannel(ch_HRf);  
  tmp1 = setChannel(ch_HRf);
  
  setChannel(1);
  setChannel(ch_HRf);  
  tmp2 = setChannel(ch_HRf);
  
  setChannel(2);
  setChannel(ch_HRf);  
  tmp3 = setChannel(ch_HRf);
  
  setChannel(3);
  setChannel(ch_HRf);  
  tmp4 = setChannel(ch_HRf);
  
  setChannel(4);
  setChannel(ch_HRf);  
  tmp5 = setChannel(ch_HRf);
  
  setChannel(5);
  setChannel(ch_HRf);  
  tmp6 = setChannel(ch_HRf);
  
  setChannel(6);
  setChannel(ch_HRf);  
  tmp7 = setChannel(ch_HRf);

}

void CADC::conv(short c1, short c2, short c3)
{
  setChannel(c1);
  setChannel(ch_HRf);  
  getData(setChannel(c2));      //r-c1 
  setChannel(ch_HRf); 
  getData(setChannel(c3));      //r-c2
  setChannel(ch_HRf);
  getData(setChannel(ch_HRf));  //r-c3
}

//void ADC::conv(short c1, short c2, short c3, short c4)
//void ADC::conv(short c1, short c2, short c3, short c4, short c5)
//void ADC::conv(short c1, short c2, short c3, short c4, short c5, short c6)

void CADC::conv(short c1, short c2, short c3, short c4, short c5, short c6, short c7)
{
  setChannel(c1);
  setChannel(ch_HRf); 
  getData(setChannel(c2));    //r-c1          
  setChannel(ch_HRf); 
  getData(setChannel(c3));    //r-c2
  setChannel(ch_HRf); 
  getData(setChannel(c4));    //r-c3
  setChannel(ch_HRf); 
  getData(setChannel(c5));    //r-c4 
  setChannel(ch_HRf);
  getData(setChannel(c6));    //r-c5
  setChannel(ch_HRf); 
  getData(setChannel(c7));    //r-c6  
  setChannel(ch_HRf); 
  getData(setChannel(ch_HRf));//r-c7
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
  LPC_SSP1->CR0 = SPI_Config::CR0_DSS(bits_tr) | SPI_Config::CR0_CPOL_HI ;
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

