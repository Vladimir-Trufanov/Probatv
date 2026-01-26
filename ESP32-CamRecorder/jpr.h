/** Arduino, ESP32, C/C++ ******************************************* jpr.h ***
 * 
 *                      Объявить/проинициализировать общепрограммные переменные
 *                                                     
 * v1.0.0, 24.01.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 24.01.2026
**/

#pragma once     

#include "inimem.h"

#define jpr(format, ...) \
  { \
    char buffer[256]; \
    snprintf(buffer, sizeof(buffer), format, ##__VA_ARGS__); \
    Serial.print(buffer); \
    if (logfile) { \
      logfile.print(buffer); \
    } \
  }

#define jprln(format, ...) \
  { \
    char buffer[256]; \
    snprintf(buffer, sizeof(buffer), format, ##__VA_ARGS__); \
    Serial.println(buffer); \
    if (logfile) { \
      logfile.println(buffer); \
    } \
  }

// ****************************************************************************
// *      Мигать задним красным светодиодом в случае неудачного включения     *
// *                           камеры или sd-карты                            *
// ****************************************************************************
void major_fail() 
{
  Serial.println(" ");
  logfile.close();
  // 10 loops or about 100 seconds then reboot
  for  (int i = 0;  i < 10; i++) 
  {                
    for (int j = 0; j < 3; j++) 
    {
      digitalWrite(33, LOW);   delay(150);
      digitalWrite(33, HIGH);  delay(150);
    }
    delay(1000);
    for (int j = 0; j < 3; j++) 
    {
      digitalWrite(33, LOW);  delay(500);
      digitalWrite(33, HIGH); delay(500);
    }
    delay(1000);
    Serial.print("Крупная неудача "); Serial.print(i); 
    Serial.print("/"); Serial.println(10);
  }
  ESP.restart();
}
// ****************************************************************************
// *              Показать состояние памяти с заданным префиксом              *
// ****************************************************************************

/**
 * xPortGetCoreID()        - функция возвращает номер ядра, на котором выполняется текущая задача
 * uxTaskPriorityGet(NULL) - возвращает приоритет текущей задачи (задачи, из которой была вызвана функция)
 * ESP.getFreeHeap()       - возвращает размер свободной кучи (heap) в байтах
 * ESP.getHeapSize()       - возвращает полный размер внутренней оперативной памяти в байтах (ОЗУ)
 * ESP.getFreePsram()      - свободный объём внешней оперативной памяти PSRAM
 * ESP.getPsramSize()      - полный объём внешней оперативной памяти PSRAM
**/
void print_mem(const char* text) 
{
  jprln("[%s] ядро: %d, приоритет: %d, свободная куча %6d от ОЗУ %6d, FreePSRAM %6d от FLASH %6d", text, xPortGetCoreID(), uxTaskPriorityGet(NULL), ESP.getFreeHeap(), ESP.getHeapSize(), ESP.getFreePsram(), ESP.getPsramSize() );
}

// ****************************************************************** jpr.h ***
