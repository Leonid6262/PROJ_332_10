#pragma once

// Привязка каналов
namespace DMA_MAP 
{
  constexpr unsigned char Ch0 = 0;
  constexpr unsigned char Ch1 = 1;
  constexpr unsigned char Ch2 = 2;
  constexpr unsigned char Ch3 = 3;
  constexpr unsigned char Ch4 = 4;
  constexpr unsigned char Ch5 = 5;
  constexpr unsigned char SPI2_Tx_Channel = 6;
  constexpr unsigned char SPI2_Rx_Channel = 7;
}
// Размер единичной передачи
namespace DmaBurst {
  constexpr unsigned int SIZE_1   = 0;
  constexpr unsigned int SIZE_4   = 1; // Оптимум для SPI
  constexpr unsigned int SIZE_8   = 2;
  constexpr unsigned int SIZE_16  = 3;
  constexpr unsigned int SIZE_32  = 4;
  constexpr unsigned int SIZE_64  = 5;
  constexpr unsigned int SIZE_128 = 6;
  constexpr unsigned int SIZE_256 = 7;
}

// Типы каналов
enum class ETransferType : unsigned char 
{
  TYPE_M2P = 1,          // Memory to Peripheral
  TYPE_P2M = 2,          // Peripheral to Memory
};  

// Размер элемента
enum class EWidth : unsigned char 
{ 
  BYTE  = 0, 
  SHORT = 1, 
  LONG  = 2 
};

// Конфигурация канала
struct SChannelConfig 
{
  unsigned char channelNum;    // Номер канала из DMA_MAP
  ETransferType transferType;  // Тип канала из ETransferType
  unsigned int  Conn;          // Номер периферийного подключения из Connection number definitions
  unsigned int  BurstSize;     // Количество единичных элементов транзакции из DmaBurst  
  EWidth Width;                // Размер единичного элемента из EWidth
  bool enableInterrupt;        // Разрешение/запрет события окончания передачи (ON/OFF) 
};