#pragma once

class CCRC16{ 
  
private:
  
  static const unsigned short CRCT16Table[256];
  
public:
  
  CCRC16();
  unsigned short calc(unsigned char*, unsigned short);
  
};
