#include "terminal.hpp"

const char* CTerminal::header_str[] = {
    "\r\nDate and Time   \r\n",
    "\r\nPi0         bits\r\n",
    "\r\nPi1         bits\r\n",
    "\r\nPi-SPI      bits\r\n",
    "\r\nPulse test      \r\n",
    "\r\nF-Sync,F-Us   Hz\r\n",
    "\r\nRel_Leakage     \r\n",
    "\r\nRS-485 loop test\r\n",
    "\r\nCAN loop test   \r\n",   
    "\r\nETH Tx[0], Rx[0]\r\n",
    "\r\nCD card RCA     \r\n"
};

CTerminal::CTerminal(const SDependencies& deps) : deps(deps)
  {
    Pause_us(500000); // Долго инициализируется ПТ
    deps.rComPort.transfer_char(static_cast<char>(ELED::LED_OFF));
    deps.rComPort.transfer_char('\r');
    deps.rComPort.transfer_string(const_cast<char*>("\r\n                "));
    deps.rComPort.transfer_string(const_cast<char*>("\r\n                "));
    edit = false;
    index_win = 0;
    ind_max = (sizeof(header_str) / sizeof(header_str[0])) - 1;
    deps.rComPort.transfer_string(const_cast<char*>(header_str[index_win]));
  }
  
void CTerminal::char_to_bits(char* d, char c)
{
  for(char b = 0; b < 8; b++)
  {
    if(c & (1 << b)) 
    {
      d[7 - b] = '1';
    }
    else 
    {
      d[7 - b] = '0';
    }
  } 
  d[8] = 0;
}

void CTerminal::terminal()
{  
  receive_char = deps.rComPort.receive_char();
  unsigned int dT0 = LPC_TIM0->TC - prev_TC0; //Текущая дельта [0.1*mks]   
  
  switch(receive_char)
  {   
  case 0x2B: //"Up"
    index_win--;
    prev_TC0 = LPC_TIM0->TC;
    if(index_win < 0) 
    {
      index_win = ind_max;
    }
    deps.rComPort.transfer_string(const_cast<char*>(header_str[index_win]));    
    break;
  case 0x2D: //"Dn"     
    index_win++;
    prev_TC0 = LPC_TIM0->TC;
    if(index_win > ind_max)  
    {
      index_win = 0;
    }
    deps.rComPort.transfer_string(const_cast<char*>(header_str[index_win]));
    break;   
  case 0x3D: //"Fn+Up"
    fup = true;
    break;  
  case 0x5F: //"Fn+Dn"
    fdn = true;
    break;  
  case 0x0D: //"Enter"
    edit = true;
    break;
  case 0x1B: //"Esc"
    edit = false;
    break;
  case 0x70: //"START"
    break;
  case 0x2A: //"STOP"
    break;
  case 0x78: //"FN+Enter" - Запись уставок    
    deps.rComPort.transfer_char(static_cast<char>(ELED::LED_GREEN));
    deps.rComPort.transfer_char('\r');
    CEEPSettings::getInstance().saveSettings();
    Pause_us(200000);
    deps.rComPort.transfer_char(static_cast<char>(ELED::LED_OFF));
    deps.rComPort.transfer_char('\r');
    break;
  case 0x00: 
    if(dT0 < 1500000) return;
    break;
  } 
  char data_port_input[9];  
  switch(index_win)
  {  
  case 0:     
    sprintf(formVar, "%02u.%02u %02u:%02u:%02u\r",
            deps.rRt_clock.get_now().day,  deps.rRt_clock.get_now().month,
            deps.rRt_clock.get_now().hour, deps.rRt_clock.get_now().minute, 
            deps.rRt_clock.get_now().second);            
    deps.rComPort.transfer_string(formVar);
    break;
  case 1:     
    char_to_bits(data_port_input, deps.rDin_cpu.UData_din_f_Pi0.all);
    sprintf(formVar, "%s\r", data_port_input);    
    deps.rComPort.transfer_string(formVar);
    break;
    
  case 2: 
    char_to_bits(data_port_input, deps.rDin_cpu.UData_din_Pi1.all);
    sprintf(formVar, "%s\r", data_port_input);
    deps.rComPort.transfer_string(formVar);
    break;
    
  case 3: 
    char_to_bits
      (
       data_port_input, 
       deps.rSpi_ports.UData_din_f[static_cast<char>(CSPI_ports::EBytesDinNumber::BYTE_DIN_CPU)].all
         );
    sprintf(formVar, "%s\r", data_port_input);
    deps.rComPort.transfer_string(formVar);
    break;
    
  case 4: 
    static signed short bridge = 0;
    if(fup) 
    {
      fup = false;
      bridge++; 
      if(bridge > 2) 
      {
        bridge = 0;
      }
    }
    if(fdn) 
    {
      fdn = false;
      bridge--; 
      if(bridge < 0) 
      {
        bridge = 2;
      }
    }
    switch(bridge)
    {  
    case 0:
      deps.rComPort.transfer_string(const_cast<char*>("STOP            \r"));
      deps.rPuls.forcing_bridge = false;
      deps.rPuls.main_bridge = false;
      break;
    case 1:
      deps.rComPort.transfer_string(const_cast<char*>("MAIN            \r"));
      deps.rPuls.forcing_bridge = false;
      deps.rPuls.main_bridge = true;
      break;
    case 2:
      deps.rComPort.transfer_string(const_cast<char*>("FORSING         \r"));
      deps.rPuls.main_bridge = false;
      deps.rPuls.forcing_bridge = true;
      break;
    }
    break;
    
  case 5:
    sprintf(formVar, "%.1f  ", deps.rCompare.SYNC_FREQUENCY);
    deps.rComPort.transfer_string(formVar);
    sprintf(formVar, "%.1f    \r", deps.rCompare.STATOR_FREQUENCY);
    deps.rComPort.transfer_string(formVar);
    break; 
    
  case 6: 
    static signed short K = 0;
    if(fup) 
    {
      fup = false;
      K++; 
      if(K > 2) 
      {
        K = 0;
      }
    }
    if(fdn) 
    {
      fdn = false;
      K--; 
      if(K < 0)
      {
        K = 2;
      }
    }   
    switch(K)
    {  
    case 0:
      deps.rComPort.transfer_string(const_cast<char*>("REL OFF         \r"));
      deps.rDout_cpu.REL_LEAKAGE_P(OFF);
      deps.rDout_cpu.REL_LEAKAGE_N(OFF);
      break;
    case 1:
      deps.rComPort.transfer_string(const_cast<char*>("REL P is ON     \r"));
      deps.rDout_cpu.REL_LEAKAGE_P(ON);
      deps.rDout_cpu.REL_LEAKAGE_N(OFF);
      break;
    case 2:
      deps.rComPort.transfer_string(const_cast<char*>("REL N is ON      \r"));
      deps.rDout_cpu.REL_LEAKAGE_P(OFF);
      deps.rDout_cpu.REL_LEAKAGE_N(ON);
      break;
    }
    break;
    
  case 7:     
    sprintf(formVar, "1:%03d  ", deps.rTs.receive_RS485_1);
    deps.rComPort.transfer_string(formVar);
    sprintf(formVar, "2:%03d    \r", deps.rTs.receive_RS485_2);
    deps.rComPort.transfer_string(formVar);
    break;
    
  case 8:     
    sprintf(formVar, "1:%03d  ", deps.rTs.CAN1_data_rx);
    deps.rComPort.transfer_string(formVar);
    sprintf(formVar, "2:%03d    \r", deps.rTs.CAN2_data_rx);
    deps.rComPort.transfer_string(formVar);
    break;
    
  case 9:     
    sprintf(formVar, "Tx:%03d ", deps.rTest_eth.sendFrame[14]);
    deps.rComPort.transfer_string(formVar);
    sprintf(formVar, "Rx:%03d   \r", deps.rTest_eth.rxBuffer[14]);
    deps.rComPort.transfer_string(formVar);
    break;
    
  case 10:     
    sprintf(formVar, "%05d     \r", deps.rSD_card.RCA);
    deps.rComPort.transfer_string(formVar);
    break;
    
  }
  
  prev_TC0 = LPC_TIM0->TC;
  
}

