#pragma once
     
#include "emac.hpp"

class CTEST_ETH {
public: 
  
    static const unsigned char MAC_PC[]; 

    unsigned char sendFrame[CEMAC_DRV::ETH_FRAG_SIZE - 4]; //6mac1+6mac2+2type+46data(макс) = 60байт (4crc add EMAC)
    unsigned char rxBuffer[CEMAC_DRV::ETH_FRAG_SIZE];      //6mac1+6mac2+2type+46data+4crc  = 64байта
    
    CEMAC_DRV& rEmac_drv;
    CTEST_ETH(CEMAC_DRV&);
    void init();
    void test();    

private:
 
};
