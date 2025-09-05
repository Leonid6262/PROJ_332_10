#include "uart.hpp"

CUART::CUART(EUartInstance UN) 
{   
  switch(UN)
  {
  case EUartInstance::UART_TERMINAL:
    UART_N = LPC_UART0; 
    LPC_IOCON->P0_2  = IOCON_U0_TXD;       // U0_TXD
    LPC_IOCON->P0_3  = IOCON_U0_RXD;       // U0_RXD
    LPC_SC->PCONP |= CLKPWR_PCONP_PCUART0; // Включение питания UART0 
    /* Настройка частоты */
    UART_N->TER = 0x00;                    // Запрет передачи на момент настройки  
    UART_N->LCR = LCR_DLAB_ON;             // b7 - DLAB вкл. 
    UART_N->DLM   = baud_19200.DLM;
    UART_N->DLL   = baud_19200.DLL;
    UART_N->FDR   = baud_19200.FDR;
    break;
  case EUartInstance::UART_RS485_01:  
    UART_N = LPC_UART2;
    LPC_IOCON->P2_6     = IOCON_U2_OE;     // U2_OE
    LPC_IOCON->P2_8     = IOCON_U2_TXD;    // U2_TXD
    LPC_IOCON->P2_9     = IOCON_U2_RXD;    // U2_RXD  
    LPC_SC->PCONP |= CLKPWR_PCONP_PCUART2; // Включение питания UART2
    UART_N->RS485CTRL   = DCTRL;           // Автоматическое переключение OE
    UART_N->RS485CTRL  |= OINV;            // Инверсия OE   
    /* Настройка частоты */
    UART_N->TER = 0x00;                    // Запрет передачи на момент настройки  
    UART_N->LCR = LCR_DLAB_ON;             // b7 - DLAB вкл.        
    UART_N->DLM   = baud_115200.DLM;
    UART_N->DLL   = baud_115200.DLL;
    UART_N->FDR   = baud_115200.FDR;
    break;
  case EUartInstance::UART_RS485_02:     
    UART_N = LPC_UART3;
    LPC_IOCON->P1_30    = IOCON_U3_OE;     // U3_OE
    LPC_IOCON->P4_28    = IOCON_U3_TXD;    // U3_TXD
    LPC_IOCON->P4_29    = IOCON_U3_RXD;    // U3_RXD  
    LPC_SC->PCONP |= CLKPWR_PCONP_PCUART3; // Включение питания UART3
    UART_N->RS485CTRL   = DCTRL;           // Автоматическое переключение OE
    UART_N->RS485CTRL  |= OINV;            // Инверсия OE
    /* Настройка частоты */
    UART_N->TER = 0x00;                    // Запрет передачи на момент настройки  
    UART_N->LCR = LCR_DLAB_ON;             // b7 - DLAB вкл.        
    UART_N->DLM   = baud_115200.DLM;
    UART_N->DLL   = baud_115200.DLL;
    UART_N->FDR   = baud_115200.FDR;
    break;
  }  
  UART_N->LCR   = LCR_DLAB_OFF; // b7 - DLAB откл., чётность откл., 1-стоп бит, символ 8бит 
  UART_N->FCR   = FIFOEN;       // FIFO. b2-очистка TXFIFO, b1-очистка RXFIFO, b0-вкл FIFO  
  UART_N->TER   = TXEN;         // Разрешение передачи   
  while (UART_N->LSR & RDR)
  {
    int tmp = UART_N->RBR; // Очистка приёмника
  }  
}

/*--- transfer_char, receive_char и transfer_string - Тестовые методы! ---*/

void CUART::transfer_char(char data)
{  
  while(!(UART_N->LSR & THRE)){}
  UART_N->THR = data;
}

char CUART::receive_char()
{  
  if(UART_N->LSR & RDR)
  {
    return UART_N->RBR;
  }
  return 0;
}

void CUART::transfer_string(char* string)
{  
  for(short b = 0; string[b] != '\0'; b++)
  {
    transfer_char(string[b]);
  }
}





