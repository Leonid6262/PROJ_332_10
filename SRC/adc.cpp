#include "adc.hpp"
#include "system_LPC177x.h"
#include <math.h>

void CADC::conv(std::initializer_list<char> list)
{
  // Ожидание полной готовности SPI перед burst-записью
  while (!((LPC_SSP0->SR & SPI_Config::SR_TFE) && !(LPC_SSP0->SR & SPI_Config::SR_BSY))) {}
  
  // Запись
  char N_ch = list.size();
  if(N_ch > 6) N_ch = 6;        //Ограничение связанное с размером FIFO в 8 фреймов
  
  for(char i = 0; i < N_ch; i++)
  {
    LPC_SSP1->DR = cN_CH[*(list.begin() + i)];
  }
  LPC_SSP1->DR = cN_CH[ch_HRf];
  LPC_SSP1->DR = cN_CH[ch_HRf];
 
  // Чтение и обработка по мере поступления данных
  unsigned short raw_adc_data;
  unsigned short tmp_Nch;
  for(char i = 0; i < (N_ch + 2); i++)
  {
    while(!(LPC_SSP1->SR & SPI_Config::SR_RNE)){}    
    raw_adc_data = LPC_SSP1->DR;
    tmp_Nch = (raw_adc_data & 0xF000) >> 12;
    if( tmp_Nch < G_CONST::NUMBER_CHANNELS ) 
    {
      data[tmp_Nch] = ((raw_adc_data & 0x0FFF) - CEEPSettings::getInstance().getSettings().shift_adc[tmp_Nch]) *
        (1.0f + CEEPSettings::getInstance().getSettings().incline_adc[tmp_Nch]);
    }
  }  
  
  // Контрольная проверка и очистка FIFO
  while(LPC_SSP1->SR & SPI_Config::SR_RNE)
  {
    raw_adc_data = LPC_SSP1->DR;
  } 
  
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

