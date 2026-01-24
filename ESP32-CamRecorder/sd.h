/** Arduino, ESP32, C/C++ ******************************************** sd.h ***
 * 
 *                                                 Обслужить работу с SD-картой
 *                                                     
 * v1.0.0, 24.01.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 24.01.2026
**/

#pragma once   

// MicroSD
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "FS.h"
#include <SD_MMC.h>

#include "inimem.h"
#include "jpr.h"

// Обеспечиваем ускорение записи на SD-карту
// https://github.com/espressif/esp32-camera/issues/182
// ранее было fbs=64 - столько КБ статической оперативной памяти 
// для psram -> буфер sram для записи на sd
#define fbs  1 
uint8_t fb_record_static[fbs * 1024 + 20];


  // SD camera init

static esp_err_t _init_sdcard()
{

  int succ = SD_MMC.begin("/sdcard", true, false, BOARD_MAX_SDMMC_FREQ, 7);
  if (succ) 
  {
    Serial.printf("SD_MMC Begin: %d\n", succ);
    uint8_t cardType = SD_MMC.cardType();
    Serial.print("SD_MMC Card Type: ");
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
      Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

  } else {
    Serial.printf("Failed to mount SD card VFAT filesystem. \n");
    Serial.println("Do you have an SD Card installed?");
    Serial.println("Check pin 12 and 13, not grounded, or grounded with 10k resistors!\n\n");
    major_fail();
  }

  return ESP_OK;
}

static bool init_sdcard()
{
  bool Result=true;
  Serial.println("Mounting the SD card ...");
  esp_err_t card_err = _init_sdcard();
  if (card_err != ESP_OK) 
  {
    Serial.printf("SD Card init failed with error 0x%x", card_err);
    Result=false;
  }
  return Result;
}


// ******************************************************************* sd.h ***
