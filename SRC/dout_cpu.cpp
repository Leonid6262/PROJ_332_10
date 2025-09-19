#include "dout_cpu.hpp"

void CDout_cpu::edit_bit(char bit_number, bool type_op)    
{ 
  if(type_op)
  {
    LPC_GPIO2->SET = static_cast<unsigned int>(1UL << (B0_PORT_OUT + (bit_number & 0x07)));
  }
  else
  {
    LPC_GPIO2->CLR = static_cast<unsigned int>(1UL << (B0_PORT_OUT + (bit_number & 0x07)));
  }
}
  
  
  
  

