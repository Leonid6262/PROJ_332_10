#include "spi_ports.hpp"
#include "spi_init.hpp"
#include "system_LPC177x.h"

// Постоянные времени интегрирования фильтра (ms первый множитель) din портов. 
// TIC_ms = 10000 дискрет таймера на 1ms.  
// То есть, 50*TIC_ms = 50ms, 0*TIC_ms - нет фильтрации, и т.п. 
const unsigned int CSPI_ports::const_integr_spi[G_CONST::BYTES_RW][8] = 
{
  {50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms}, // Контроллер
  {50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms}, // S600 - байт-0
  {50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms}  // S600 - байт-1
};

void CSPI_ports::rw() 
{  
  unsigned int dT = LPC_TIM0->TC - prev_TC0; //Текущая дельта [0.1*mks]
  prev_TC0 = LPC_TIM0->TC;
 
  for(char byte = 0; byte < G_CONST::BYTES_RW; byte++) 
  {
    //Запись в dout с учётом инверсии
    LPC_SSP0->DR = UData_dout[byte].all ^ CEEPSettings::getInstance().getSettings().dout_spi_invert[byte]; 
    
    //data_din_invert - входные данные (предыдущего цикла r/w) din портов  с учётом инверсии
    unsigned char data_din_invert = data_din[byte] ^ CEEPSettings::getInstance().getSettings().din_spi_invert[byte];
    
    //Фильтр 8-ми бит
    for(char b = 0; b < N_BITS; b++){      
      if(data_din_invert & (1UL << b))
      {           
        //Если на входе 1
        integrator[byte][b] += dT;
        if(integrator[byte][b] >= const_integr_spi[byte][b])
        { 
          //Если интегратор в насыщении, на выходе 1
          integrator[byte][b] = const_integr_spi[byte][b];
          UData_din_f[byte].all |= (1UL << b);
        }
      }
      else
      {
        //Если на входе 0
        integrator[byte][b] -= dT;
        if(integrator[byte][b] <= 0)
        { 
          //Если интегратор в нуле, на выходе 0
          integrator[byte][b] = 0;
          UData_din_f[byte].all &= ~(1UL << b);       
        }
      }       
    }
    
    // После окончание операции r/w считываем байт din порта.
    // учитывая, что процесс фильтрации происходит на фоне транзакции spi,ожидания 
    // при частотах spi до 900 кГц в while (LPC_SSP0->SR & SR_BSY){} - не происходит
    while (LPC_SSP0->SR & SPI_Config::SR_BSY){}
    data_din[byte] = LPC_SSP0->DR;
    
  } 
  
  //Захват din и обновление dout (1-0-1 HOLD bit).
  LPC_GPIO0->CLR = HOLD;
  for(short Counter = 0x7; Counter > 0; Counter--){}
  LPC_GPIO0->SET = HOLD;  
}


CSPI_ports::CSPI_ports()
{
  // SPI-0 
  LPC_IOCON->P0_15  = IOCON_SPI;      // SCK0
  LPC_IOCON->P0_17  = IOCON_SPI;      // MISO0
  LPC_IOCON->P0_18  = IOCON_SPI;      // MOSI0
  
  LPC_SC->PCONP |= CLKPWR_PCONP_PCSSP0;
  LPC_SSP0->CR0 = 0;
  LPC_SSP0->CR0 = SPI_Config::CR0_DSS(bits_tr);// | CR0_CPOL_HI;// | CR0_CPHA_SECOND;
  LPC_SSP0->CR1 = 0;
  SPI_Config::set_spi_clock(LPC_SSP0, Hz_SPI, PeripheralClock );
  LPC_SSP0->CR1 |= SPI_Config::CR1_SSP_EN; 

  // Обнуление случайных значений в выходных регистрах и сброс интеграторов.
  for(char byte = 0; byte < G_CONST::BYTES_RW; byte++) 
  {
    UData_dout[byte].all = 0;
    for(char b = 0; b < N_BITS; b++) 
    {      
      integrator[byte][b] = 0;
    }
  }    
  rw();
  // Активизация выходных регистров (перевод из 3-го состояния в активное)
  LPC_GPIO0->CLR = OUT_E;      
  prev_TC0 = LPC_TIM0->TC;
}
