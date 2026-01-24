/** Arduino, ESP32, C/C++ ******************************************** sd.h ***
 * 
 *                                                 Обслужить работу с SD-картой
 *                                                     
 * v1.0.0, 24.01.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 24.01.2026
**/

#pragma once   

#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "FS.h"
#include <SD_MMC.h>

#include "inimem.h"
#include "jpr.h"

// Обеспечиваем ускорение записи на SD-карту [https://github.com/espressif/esp32-camera/issues/182],
// ранее было fbs=64 - столько КБ статической оперативной памяти 
// для psram -> буфер sram для записи на sd
#define fbs  1 
uint8_t fb_record_static[fbs * 1024 + 20];

// ****************************************************************************
// *                       Инициализировать SD-карту                          *
// ****************************************************************************
static bool init_sdcard()
{
  Serial.println("Инициализируется SD-карта ...");
  bool Result=true;
  int succ = SD_MMC.begin("/sdcard", true, false, BOARD_MAX_SDMMC_FREQ, 7);
  if (succ) 
  {
    Serial.println("SD_MMC инициализирована успешно");
    uint8_t cardType = SD_MMC.cardType();
    Serial.print("Тип карты SD_MMC: ");
    if (cardType == CARD_MMC) 
    {
      Serial.println("MMC");
    } 
    else 
    if (cardType == CARD_SD) 
    {
      Serial.println("SDSC");
    } 
    else if (cardType == CARD_SDHC) 
    {
      Serial.println("SDHC");
    } 
    else 
    {
      Serial.println("неопределена");
    }
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("Ёмкость SD_MMC-карты: %llu MB\n", cardSize);
  } 
  else 
  {
    Serial.printf("Ошибка инициализации SD-карты на файловой системе VFAT\n");
    Serial.println("Проверьте контакты 12 и 13 , они не заземлены или заземлены резисторами 10 ком!");
    Result=false;
  }
  return Result;
}

// ******************************************************************* sd.h ***
