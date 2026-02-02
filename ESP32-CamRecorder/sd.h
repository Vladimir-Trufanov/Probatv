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

//////////////////////////////
//61.3 oneframe find_a_frame (char * avi_file_name, int frame_pct) ; // from avi.cpp file

struct oneframe {
  uint8_t* the_frame;
  int the_frame_length;
  long the_frame_number;
  long the_frame_total;
};


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

//
// Reads an uint32_t in Big Endian at current file position
//
int read_quartet( File fd) 
{

  uint8_t y[4];
  size_t i1_err = fd.read(y , 4);
  uint32_t value = y[0] | y[1] << 8 | y[2] << 16 | y[3] << 24;
  //Serial.printf("read_quartet %d %d %d %d, %d\n", y[0], y[1], y[2], y[3], value);
  return value;
}
// ****************************************************************************
// *    Записать в файл беззнаковое целое, как 4 последовательных байта -     *
// *            от первого (с наименьшим адресом) к четвертому                *
// *                      c текущего положения в файле                        *
// ****************************************************************************
static void inline print_quartet(unsigned long i, File fd) 
{
  uint8_t y[4];
  y[0] = i % 0x100;
  y[1] = (i >> 8) % 0x100;
  y[2] = (i >> 16) % 0x100;
  y[3] = (i >> 24) % 0x100;
  size_t i1_err = fd.write(y,4);
}


//
// Writes 2 uint32_t in Big Endian at current file position
//
static void inline print_2quartet(unsigned long i, unsigned long j, File fd) {

  uint8_t y[8];
  y[0] = i % 0x100;
  y[1] = (i >> 8) % 0x100;
  y[2] = (i >> 16) % 0x100;
  y[3] = (i >> 24) % 0x100;
  y[4] = j % 0x100;
  y[5] = (j >> 8) % 0x100;
  y[6] = (j >> 16) % 0x100;
  y[7] = (j >> 24) % 0x100;
  size_t i1_err = fd.write(y , 8);
}




oneframe find_a_frame (char * avi_file_name, long frame_num) 
{
  File findfile;

  oneframe x;
  findfile = SD_MMC.open(avi_file_name, "r");
  if (!findfile) {
    Serial.printf("Could not open %s file\n", avi_file_name);
    x.the_frame = NULL;
    return x;
  }  else  {
    //Serial.printf("Size %d\n",findfile.size());
    //Serial.printf("Last %d\n",findfile.getLastWrite());
    time_t lastw = findfile.getLastWrite();
    //Serial.printf("Lastw %d\n",lastw);

    time_t current;
    time (&current);

    int age = current - lastw;
    //Serial.printf("Age %d\n", age);

    //Serial.printf("File open: %s\n", avi_file_name);
    findfile.seek( 0x30 , SeekSet);
    long frame_total = read_quartet( findfile);
    //Serial.printf("Frames from file %ld\n", frame_total);

    if (age < 10) {
      //Serial.printf("Frame file %d, current %d\n", frame_total, frame_cnt - 1);
      frame_total = frame_cnt - 1;

      File idxfile =  SD_MMC.open("/idx.tmp", "r");
      if (!idxfile) {
        Serial.printf("Could not open /idx.tmp file\n");
        x.the_frame = NULL;
        return x;
      }

      int the_offset = frame_num * 8;
      idxfile.seek(the_offset, SeekSet);
      //Serial.printf("the frame %d, the offset %d\n", frame_num, the_offset);
      int the_addr = read_quartet (idxfile);
      int the_idx_len = read_quartet (idxfile);
      //Serial.printf("from index, the addr %d, the length %d\n",the_addr,the_idx_len);

      idxfile.close();

      findfile.seek( the_addr  + 236 , SeekSet);

      int the_oodc = read_quartet (findfile);
      //Serial.printf("the oodc %d\n",the_oodc);

      if (the_oodc != 1667510320) {
        Serial.printf("No frame %d, %d, %d\n%s file, num %d\n", frame_num, the_addr, the_idx_len);
        x.the_frame = NULL;
        return x;
      }
      //findfile.seek( the_addr + 8 + 236 , SeekSet);
      int the_len = read_quartet (findfile);
      //Serial.printf("frame len %d \n", the_len);

      //Serial.printf("Your frame is %d bytes, at address %d or %X\n", index_frame_length, index_frame_start, index_frame_start);
      uint8_t* fb_faf;
      fb_faf = (uint8_t*)ps_malloc(the_len + 24);

      // findfile.seek( 4 + 244 , SeekSet);

      size_t err = findfile.read(fb_faf, the_len);

      x.the_frame = fb_faf;
      x.the_frame_length = the_len;
      x.the_frame_number = frame_num;
      x.the_frame_total = frame_total;
      return x;


    }

    //findfile.seek( 0x8c , SeekSet);
    //long frame_cnt8c = read_quartet( findfile);
    //Serial.printf("Frames8c is %ld\n", frame_cnt8c);

    //int frame_num = 0.01 * frame_pct * frame_cnt;
    //Serial.printf("Frames pct %d, Frame num %d \n", frame_pct, frame_num);

    if (frame_total < frame_num) {
      Serial.printf("Only %ld frames, less than %ld frame_num -- start at 0\n", frame_cnt, frame_num);
      frame_num = 0;
    }
    if (frame_total == 3) {
      Serial.printf("Three 3 frames - we dont know how many! -- start at 0\n");

      frame_num = 0;
      findfile.seek( 4 + 236 , SeekSet);
      int the_oodc = read_quartet (findfile);
      if (the_oodc != 1667510320) {
        Serial.printf("No frame %s file, num %d\n", avi_file_name, frame_num);
        x.the_frame = NULL;
        return x;
      }
      int the_len = read_quartet (findfile);
      //Serial.printf("frame len %d \n", the_len);

      //Serial.printf("Your frame is %d bytes, at address %d or %X\n", index_frame_length, index_frame_start, index_frame_start);
      uint8_t* fb_faf;
      fb_faf = (uint8_t*)ps_malloc(the_len + 24);

      findfile.seek( 4 + 244 , SeekSet);

      size_t err = findfile.read(fb_faf, the_len);

      x.the_frame = fb_faf;
      x.the_frame_length = the_len;
      x.the_frame_number = frame_num;
      x.the_frame_total = frame_total;
      return x;
    }

    findfile.seek( 0xe8 , SeekSet);
    long index_start = read_quartet( findfile);
    //Serial.printf("Len of movi / index_start %ld\n", index_start);

    //bool success = findfile.seek(  , SeekEnd);

    //Serial.printf("Len of file %ld\n", findfile.size());
    //Serial.printf("Seek %d\n",  index_start + 244 + frame_num * 16 + 8);

    if (findfile.size() < index_start + 244 + frame_num * 16 + 8 , SeekSet) {
      Serial.printf("File too small / broken %s file\n", avi_file_name);
      x.the_frame = NULL;
      return x;
    }
    bool success = findfile.seek( index_start + 244 + frame_num * 16 + 8 , SeekSet);
    if (!success) {
      Serial.printf("File incomplete %s file\n", avi_file_name);
      x.the_frame = NULL;
      return x;

    }
    long index_frame_start = read_quartet( findfile);
    long index_frame_length = read_quartet( findfile);

    findfile.seek( index_frame_start + 236 , SeekSet);
    int the_oodc = read_quartet (findfile);
    if (the_oodc != 1667510320) {
      Serial.printf("No frame %s file, num %d\n", avi_file_name, frame_num);
      x.the_frame = NULL;
      return x;
    }
    int the_len = read_quartet (findfile);
    //Serial.printf("frame len %d \n", the_len);

    //Serial.printf("Your frame is %d bytes, at address %d or %X\n", index_frame_length, index_frame_start, index_frame_start);
    uint8_t* fb_faf;

    //fb_faf = (uint8_t*)ps_malloc(48 * 1024);  // danger 48kb may not be enough
    fb_faf = (uint8_t*)ps_malloc(the_len + 24);
    findfile.seek( index_frame_start + 244 , SeekSet);

    size_t err = findfile.read(fb_faf, index_frame_length);

    x.the_frame = fb_faf;
    x.the_frame_length = index_frame_length;
    x.the_frame_number = frame_num;
    x.the_frame_total = frame_total;
    return x;

  } // else yes to no avi file
}


// ******************************************************************* sd.h ***
