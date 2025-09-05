#include "test_eth.hpp"

/* ----Тестовое ПО. К рабочим решениям, отношения не имеет!---- */

// MAC адрес ПК
const unsigned char CTEST_ETH::MAC_PC[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

CTEST_ETH::CTEST_ETH(CEMAC_DRV& rEmac_drv) : rEmac_drv(rEmac_drv){}

void CTEST_ETH::init() {                                                // Тестовый кадр:
  short L_MAC = sizeof(CTEST_ETH::MAC_PC);
  for(short n = 0; n < L_MAC; n++) 
  {
    sendFrame[n] = MAC_PC[n];                                           // MAC получателя (PC)                             
  }                                                                     
  for(short n = 0; n < L_MAC; n++) 
  {
    sendFrame[n + L_MAC] = rEmac_drv.MAC_Controller[n];                 // MAC отправителя (Controller)
  }                                                                     
  sendFrame[L_MAC * 2] = 0x08; sendFrame[1 + (L_MAC * 2)] = 0x00;       // Тип кадра 0x0800 - IPv4
  for(unsigned char n = 0; n < (CEMAC_DRV::ETH_FRAG_SIZE - 4); n++) 
  {
    sendFrame[n + 2 + (L_MAC * 2)] = n;                                 // 46 байт данных
  }    
}                                                                               

void CTEST_ETH::test() {
  
  static unsigned int prev_TC0;  
  
  unsigned int dTrs = LPC_TIM0->TC - prev_TC0; //Текущая дельта [0.1*mks]
  if(dTrs < 5000000) return;
  prev_TC0 = LPC_TIM0->TC;
  
  rEmac_drv.sendFrame(sendFrame);
  Pause_us(1000);
  if(rEmac_drv.receiveFrame(rxBuffer) == CEMAC_DRV::ReceiveStatus::FRAME_RECIVED)
  {
    sendFrame[0 + 2 + (2 * sizeof(CTEST_ETH::MAC_PC))]++;
  }  

}
