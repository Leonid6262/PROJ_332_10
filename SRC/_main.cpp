#include "main.hpp"

/*  
    1.Проект компилируется как C++, для Cortex-M4. FPU включен. 
    2.Скрипт распределения памяти MemoryMap.icf (бывший ldscript_irom_iar) - отредактирован:
      - снят конфликт правил для секции .bss;
      - В RAM добавлен блок (4К) DMA_BUFFERS, под DMA 
        UART, SPI, CD (#pragma location = ".dma_buffers").
      - HEAP (0.5К) хотя и не используется, оставлена на не непредвиденный случай.
    3.Все внедряемые зависимости, кроме crc16 - Reference.
    4.Вместо устаревших конструкций include guards используются директивы #pragma once.
    5.В большинстве нумерованных списков enum, применяется рекомндуемый, типобезопасный enum class.
    6.Приведение типов используются, только, операторами static_cast<>, const_cast<> и reinterpret_cast<>.
    7.Вместо константных макросов типа #define const, используются constexpr выражения.
    8.Все функциональные макросы заменены на inline / #pragma inline = forced функции.
    9.Зависимостей от функций "си" файлов LPC-Open, - нет.
*/

/* Буферы передачи/приёма по DMA класса CREM_OSC */
#pragma location = ".dma_buffers"
__root signed short CREM_OSC::tx_dma_buffer[CREM_OSC::TRANSACTION_LENGTH];
#pragma location = ".dma_buffers"
__root signed short CREM_OSC::rx_dma_buffer[CREM_OSC::TRANSACTION_LENGTH];


void UserStartInit()
{     
  SetDiscreteOutputs();    // Определение дискретных выходов микроконтроллера (pins)
  UserLedOn();             // Включается для визуального контроля инициализации
  EMC_Init_Check();        // Инициализации exr RAM и шины расширения. Контроль exr RAM
}

/* 
    Фактически, вызов EMC_Init_Check() в SystemInit() нужен для корректной инициализации в секции data, глобальных,
    объявленных в статической области файла system_LPC177x.c, переменных SystemCoreClock, PeripheralClock и EMCClock.
*/

void main(void)
{                   

  NVIC_SetPriorityGrouping(Priorities::G4S8);   // Распределение векторов по группам реализовано в файле IntPriority.h
  Timers_Initializing();                        // Настройки реализованы в виде одной функции для удобства.
  static CCRC16 crc16;                          // Объект класса расчёт КС (как минимум,ещё понадобится в ModBus)   
  
  CEEPSettings::getInstance().init_EEP(&crc16); // Singleton класса CEEPSettings создаётся здесь (паттерн Майерса).
                                                // Решение спорное, но в Embedded - минимальное/неизбежное зло.                              
                                                // В данном случае, допускаем передачу pointer crc16
                                                // (передать в singleton reference намного сложнее) 
  // Загрузка уставок (RAM <- EEPROM) 
  if(CEEPSettings::getInstance().loadSettings() != StatusRet::SUCCESS)          
  {
   /*  
      Если CRC не сошлись (StatusRet::ERROR), используются уставки по умолчанию.
      Вывести сообщение на ПТ: "Ошибка CRC. Используются уставки по умолчанию!"  
  */
  }  

  /*
    Объявление объектов классов устройств. Намеренно создаются как static в main().
    Без внедрения зависимостей, по "найди меня" (extern MyClass myObj) - ВИДНЫ НЕ БУДУТ!
  */
  
  static CDMAcontroller cont_dma; // Класс управления каналами DMA. От lpc177x_8x_gpdma.c/h не зависит.
                                  // Пример инициализации и использования каналов 
                                  // с.м. в классе CREM_OSC (Дистанционный осцилограф)
  
  static CUART COM_Port(CUART::EUartInstance::UART_TERMINAL);       // COM Port (uart0 - Terminal)
  static CUART RS485_01(CUART::EUartInstance::UART_RS485_01);       // RS485-1 (uart2)
  static CUART RS485_02(CUART::EUartInstance::UART_RS485_02);       // RS485-2 (uart3)
  
  static CCAN can1(CCAN::ECAN_Id_Instance::CAN1_Id);                // CAN-1   
  static CCAN can2(CCAN::ECAN_Id_Instance::CAN2_Id);                // CAN-2   
  
  static CDAC0 dac0;                                                // DAC-0 (Controller dac)
  static CDAC_PWM pwm_dac1(CDAC_PWM::EPWM_DACInstance::PWM_DAC1);   // DAC-1 (PWM1:5)
  static CDAC_PWM pwm_dac2(CDAC_PWM::EPWM_DACInstance::PWM_DAC2);   // DAC-2 (PWM1:4)
  
  static CIADC i_adc;           // Внутренее ADC.
  static CADC adc;              // Внешнее ADC. 
  
  static CSPI_ports spi_ports;  // Дискретные входы и выходы доступные по SPI. Примеры доступа: 
                                //      if(rSpi_ports.Stator_Key()){...}
                                //      rSpi_ports.Lamp_REDY(ON);
  
  static CDin_cpu din_cpu;      // Дискретные входы контроллера (порты Pi0 и Pi1 по аналогии с СМ3)
  static CDout_cpu dout_cpu;    // Дискретные выходы контроллера (порт Po0 по аналогии с СМ3)
                                // Примеры доступа:
                                //      if(rDin_cpu.Setting_More()){...}
                                //      rDout_cpu.REL_LEAKAGE_P(OFF);
    
  static CSDCard sd_card;       /* Инициализация CD карты. При успешной инициализации, читается RCA карты.
                                   Файловая система не реализована. */
  
  if(sd_card.init() != StatusRet::SUCCESS)          
  {
    /*Вывести сообщение на ПТ: "CD карта не определена!"*/
  }
   
  static CEMAC_DRV emac_drv;     /* Драйверы EMAC. Позволяют производить приём/передачу Ethernet кадров в raw формате.
                                    UDP или TCR/IP сервер не реализован, но, для обмена типа точка-точка, 
                                    например, контроллер-ноутбук, raw Ethernet кадров достаточно.*/
  
  if(emac_drv.initEMAC() != StatusRet::SUCCESS)          
  {
    /*Вывести сообщение на ПТ: "Нет готовности Ethernet!"*/
  }
  
  static CRTC rt_clock;      /* Системные часы (Real Time Clock по документации) */
                             /* Пример "атомарного" чтения:
                                  rRt_clock.update_now(); <--данные читаются из RTC в момент выполнения update_now()
                                  sec   = rRt_clock.get_now().second; min  = rRt_clock.get_now().minute; 
                                  hour  = rRt_clock.get_now().hour;   day  = rRt_clock.get_now().day; 
                                  month = rRt_clock.get_now().month;  year = rRt_clock.get_now().year;
                                Пример "атомарной" записи:
                                  CRTC::SDateTime CurDateTime = {year, month, day, hour, min, sec};                                       
                                  rRt_clockc.setDateTime(CurDateTime); <--данные пишутся в RTC в момент выполнения setDateTime(CurDateTime)
                              */
  
  // Пример структуры инициализирующих значений CREM_OSC. Дистанционный осцилограф (ESP32 c WiFi модулем)
  static CREM_OSC::SSET_init set_init
  {
    {
      // Указатели на отображаемые переменные
      &adc.data[CADC::ROTOR_CURRENT], &adc.data[CADC::STATOR_CURRENT],
      &adc.data[CADC::ROTOR_VOLTAGE], &adc.data[CADC::STATOR_VOLTAGE]
      // По pData[NUMBER_TRACKS] определяется фактическое количество треков. 
    },
    {
      // Имена треков (как будут подписаны в ПО ПК)
      "IROT","ISTAT","UROT","USTAT"
    },
    {
      // Коэффициенты отображения (дискрет на 100%)
      CEEPSettings::getInstance().getSettings().disp_c.p_ROTOR_CURRENT,
      CEEPSettings::getInstance().getSettings().disp_c.p_STATOR_CURRENT,
      CEEPSettings::getInstance().getSettings().disp_c.p_ROTOR_VOLTAGE,
      CEEPSettings::getInstance().getSettings().disp_c.p_STATOR_VOLTAGE
    },
    // Режим работы Access_point или Station
    CREM_OSC::Operating_mode::Access_point,
    // Серийный номер платы контроллера (задаётся для режима Access_point)
    CEEPSettings::getInstance().getSettings().SNboard_number,
    // Имя сети и пароль (задаются для режима Station)
    // Как задавать ssid и password пока не ясно. При отсутствии панели оператора
    // и сетевых интерфейсов, возможно с ноутбука. Задавать с ПТ удовольствие так себе.
    CEEPSettings::getInstance().getSettings().ssid,
    CEEPSettings::getInstance().getSettings().password 
  };
  static CREM_OSC rem_osc(cont_dma, set_init);  // Дистанционный осцилограф (ESP32 c WiFi модулем).Карту каналов DMA с.м. в controllerDMA.hpp               // 
                                                // Передача данных (метод send_data()) осуществляется в точке, где отображаемые переменные обновлены,
                                                // например в СИФУ. В примере, send_data() вызывается в классе теста ИУ (имитация СИФУ)
                                                // с.м файл обработчиков прерываний "handlers_IRQ.cpp" и "Puls.cpp"
  /*--Объекты классов тестов--*/

  static CPULS puls(rem_osc);   // Тест импульсов управления. Выдаётся классическая последовательность СИФУ, передаются данные в ESP32             

  static CCOMPARE compare;      // Тест компараторов. Измеряет частоту синхронизации и напряжения статора
  
  CProxyHandlerTIMER123::getInstance().set_pointers(&puls, &compare); // Proxy Singleton доступа к Handler TIMER1,2,3 создаётся здесь (паттерн Майерса)
                                                                      // Данная технология позволяет избежать глобальных ссылок на puls и compare
  puls.start();                 // Старт теста ИУ
  compare.start();              // Старт теста компараторов
  
  static CTEST_ETH test_eth(emac_drv);  // loop Test Ethernet. По физической петле передаёт/принимает тестовые raw кадры 

  /* 
    Тесты CAN1, CAN2, RS485-1, RS485-2, DAC0, PWM_DAC1, PWM_DAC2.
    Так как интерфейсы CAN и UART проверяются на функционирование,
    в UART используются FIFO (к DMA и Interrupts не подвязаны).
  */
  static CTESTS tests(RS485_01, RS485_02, can1, can2, dac0, pwm_dac1, pwm_dac2); 
  
  /* 
    При большом количестве ссылок, целесообразно использовать вспомогательную 
    встроенную структуру, как альтернативный дизайн внедрения зависимостей.
  */    
  static CTerminal::SDependencies deps
  {
    COM_Port,   
    din_cpu, 
    spi_ports,  
    tests,
    dout_cpu,   
    i_adc,   
    adc,        
    sd_card,
    test_eth,
    rt_clock,
    puls,
    compare 
  };
  static CTerminal terminal(deps); // Класс CTerminal НЕ ПО ПТ! Используется, только для индикации и управления тестами
  
  UserLedOff();  // Визуальный контроль окончания инициализации (львинную долю времени занимает CD и Ethernet)
  
  while(true)
  {              
    // Измерение всех используемых (в ВТЕ) аналоговых сигналов (внешнее ADC)
    adc.conv
      (
       CADC::ROTOR_CURRENT,         
       CADC::STATOR_CURRENT,             
       CADC::ROTOR_VOLTAGE,            
       CADC::STATOR_VOLTAGE,            
       CADC::LEAKAGE_CURRENT,       
       CADC::EXTERNAL_SETTINGS,     
       CADC::LOAD_NODE_CURRENT   
         );
    /* 
      Для сокращения записи аргументов здесь использован enum вмесо enum class.
      CADC::ROTOR_CURRENT вместо static_cast<char>(CADC::EADC_NameCh::ROTOR_CURRENT) - считаю, разумный компромисс.
      Пример доступа к измеренным значениям - rADC.data[CADC::ROTOR_CURRENT] 
    */
    
    // Измерение напряжения питания +/- 5V (внутреннее ADC)
    i_adc.measure_5V();
    
    // Чтение состояния дискретных входов контроллера
    din_cpu.input_Pi0();
    din_cpu.input_Pi1();
    
    // Запись в дискретные выходы и чтение дискретных входов доступных по SPI
    spi_ports.rw();
    
    // loop test RS485
    tests.testRS485();
    
    // loop Test CAN
    tests.testCAN();
    
    // Тест DAC-0, PWM-DAC1, PWM-DAC2 (контроль функционированя - осцилографом)
    tests.testDAC0();
    tests.testDAC1_PWM();
    tests.testDAC2_PWM();
    
    // Тест компараторов (индикация измеренных частот Sync и Us)
    compare.test();    
    
    // loop Test Ethernet
    test_eth.test();
    
    // Обновление экземпляра структуы SDateTime данными из RTC
    rt_clock.update_now();
    
    // Terminal (индикация и управление тестами)
    terminal.terminal();        
    
    Pause_us(100);
  } 
}


