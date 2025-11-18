#pragma once

/*
    Такое распределение обусловленно проекцией типовых 
    8-ми бит Cortex на 5-ти битный диапазон NXP LPC4078 

  NVIC_SetPriorityGrouping(2) -	5 бит  на группу и 0 бит  на подгруппу
  NVIC_SetPriorityGrouping(3) -	4 бита на группу и 1 бит  на подгруппу
  NVIC_SetPriorityGrouping(4) -	3 бита на группу и 2 бита на подгруппу
  NVIC_SetPriorityGrouping(5) -	2 бита на группу и 3 бита на подгруппу <-----
  NVIC_SetPriorityGrouping(6) -	1 бит  на группу и 4 бита на подгруппу
  NVIC_SetPriorityGrouping(7) -	0 бит  на группу и 5 бит  на подгруппу

*/

namespace Priorities 
{
  constexpr unsigned int G4S8 = 5;         // 4 группы (2 бита) по 8 подгрупп (3 бита)
  constexpr unsigned int SubgroupBits = 3; // Количество бит подгруппы
  
  constexpr unsigned int make_priority(unsigned int group, unsigned int subgroup) 
  {
    return (group << SubgroupBits) | subgroup;
  }
  
  // --- Группа 0 зарезервирована для критических системных прерываний ---
  
  // ---  Группа 1  --- 8 подгрупп 
  constexpr unsigned int Timer3 = make_priority(1, 0);
  constexpr unsigned int Timer2 = make_priority(1, 1);
  constexpr unsigned int Timer1 = make_priority(1, 2);
  
  // ---  Группа 2  --- 8 подгрупп 
  constexpr unsigned int UART   = make_priority(2, 0);
  constexpr unsigned int DMA    = make_priority(2, 1);
  constexpr unsigned int CAN    = make_priority(2, 2);
  
  // ---  Группа 3  --- 8 подгрупп 
  
  // ---  Группа 4  --- 8 подгрупп 
}
