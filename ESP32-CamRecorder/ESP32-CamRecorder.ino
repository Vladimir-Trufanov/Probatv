/** Arduino, ESP32, C/C++ *************************** ESP32-CamRecorder.ino ***
 * 
 *     Видеомагнитофон на Esp32-Cam - переделка ESP32-CAM-Video-Recorder-junior
 *                (https://github.com/jameszah/ESP32-CAM-Video-Recorder-junior) 
 *                                                     для умного хозяйства tve
 *                                                     
 * v1.0.4, 28.01.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 11.01.2026
 * 
 * Modify by James Zahary Sep 12, 2020 - jamzah.plc@gmail.com
 * 
 * По версии https://github.com/jameszah/ESP32-CAM-Video-Recorder,
 * которая включает работу с Wi-Fi, потоковым видео, управлением по http, 
 * через telegram, pir-контроль, сенсорное управление, загрузка по ftp и другое.
 * 
 * Программа записывает видео в формате mjpeg avi на sd-карту ESP 32-CAM. 
 * По умолчанию файлы имеют такие названия, как: desklens001.003.avi
 * "desklens" - имя определяемое разработчиком,
 * 001 - это число, сохраненное в eprom, которое будет увеличиваться при каждой загрузке устройства
 * 003 - это третий файл, созданный во время текущей загрузки
 * 
 * Arduino IDE 2.3.7 
 * Esp32 от Espressif Systems версии 3.3.5
 * Payment:           "Al Thinker ESP32-CAM"
 * CPU Frequency:     "240MHz (WiFi/BT)"
 * Flash Frequency:   "80MHz"
 * Flash Mode:        "QIO"
**/

#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_camera.h"
#include "sensor.h"

#include "inimem.h"
#include "jpr.h"
#include "sd.h"
#include "eprom.h"
#include "config.h"
#include "camera.h"
#include "CameraServer.h"

//bool configfile = false;
//bool reboot_now = false;
//bool restart_now = false;

//String czone;
//char apssid[30];
//char appass[14];

/*
TaskHandle_t the_camera_loop_task;
TaskHandle_t the_sd_loop_task;
TaskHandle_t the_streaming_loop_task;

static SemaphoreHandle_t wait_for_sd;
static SemaphoreHandle_t sd_go;
SemaphoreHandle_t baton;

long current_frame_time;
long last_frame_time;
bool web_stop = false;
*/

/*
// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
*/

camera_fb_t * fb_curr = NULL;
camera_fb_t * fb_next = NULL;

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "soc/soc.h"
#include "esp_cpu.h" // #include "soc/cpu.h"
#include "soc/rtc_cntl_reg.h"

static esp_err_t cam_err;
//float most_recent_fps = 0;
//int most_recent_avg_framesize = 0;

/*
uint8_t* fb_record;
uint8_t* fb_curr_record_buf;
uint8_t* fb_streaming;
uint8_t* fb_capture;

int fb_record_len;
int fb_curr_record_len;
int fb_streaming_len;
int fb_capture_len;
long fb_record_time = 0;
long fb_curr_record_time = 0;
long fb_streaming_time = 0;
long fb_capture_time = 0;

int first = 1;
long frame_start = 0;
long frame_end = 0;
long frame_total = 0;
long frame_average = 0;
long loop_average = 0;
long loop_total = 0;
long total_frame_data = 0;
long last_frame_length = 0;
int done = 0;
*/

/*
long avi_start_time = 0;
long avi_end_time = 0;
int start_record = 0;
int start_record_2nd_opinion = -2;
int start_record_1st_opinion = -1;
*/

int we_are_already_stopped = 0;
long total_delay = 0;
long bytes_before_last_100_frames = 0;
long time_before_last_100_frames = 0;

long time_in_loop = 0;
long time_in_camera = 0;
long time_in_sd = 0;
long time_in_good = 0;
long time_total = 0;
//long time_in_web1 = 0;
//long time_in_web2 = 0;
long delay_wait_for_sd = 0;
long wait_for_cam = 0;
int very_high = 0;

//bool do_the_ota = false;

int do_it_now = 0;
int gframe_cnt;
int gfblen;
int gj;
int  gmdelay;

// MicroSD
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "FS.h"
#include <SD_MMC.h>

//char avi_file_name[100];
//char file_to_edit[50] = "/JamCam0481.0007.avi"; //61.3

static int i = 0;
//uint16_t frame_cnt = 0;
uint16_t remnant = 0;
uint32_t length = 0;
uint32_t startms;
uint32_t elapsedms;
uint32_t uVideoLen = 0;

int bad_jpg = 0;
int extend_jpg = 0;
int normal_jpg = 0;

long boot_time = 0;

long totalp;
long totalw;

#define BUFFSIZE 512

uint8_t buf[BUFFSIZE];

#define AVIOFFSET 240 // AVI main header length

unsigned long movi_size = 0;
unsigned long jpeg_size = 0;
unsigned long idx_offset = 0;

uint8_t zero_buf[4] = {0x00, 0x00, 0x00, 0x00};
uint8_t dc_buf[4] = {0x30, 0x30, 0x64, 0x63};      // "00dc"
uint8_t dc_and_zero_buf[8] = {0x30, 0x30, 0x64, 0x63, 0x00, 0x00, 0x00, 0x00};

uint8_t avi1_buf[4] = {0x41, 0x56, 0x49, 0x31};    // "AVI1"
uint8_t idx1_buf[4] = {0x69, 0x64, 0x78, 0x31};    // "idx1"


struct frameSizeStruct 
{
  uint8_t frameWidth[2];
  uint8_t frameHeight[2];
};

// Здесь используются две ссылки на Git-репозитарии, которые сохранены в каталоге DownLoads приложения: 
// data structure from here https://github.com/s60sc/ESP32-CAM_MJPEG2SD/blob/master/avi.cpp, extended for ov5640
// must match https://github.com/espressif/esp32-camera/blob/b6a8297342ed728774036089f196d599f03ea367/driver/include/sensor.h#L87
// which changed in Nov 2024
static const frameSizeStruct frameSizeData[] = 
{
  {{0x60, 0x00}, {0x60, 0x00}}, // FRAMESIZE_96X96,    // 96x96    0 framesize
  {{0xA0, 0x00}, {0x78, 0x00}}, // FRAMESIZE_QQVGA,    // 160x120  1
  {{0x60, 0x00}, {0x60, 0x00}}, // FRAMESIZE_128X128   // 128x128  2
  {{0xB0, 0x00}, {0x90, 0x00}}, // FRAMESIZE_QCIF,     // 176x144  3
  {{0xF0, 0x00}, {0xB0, 0x00}}, // FRAMESIZE_HQVGA,    // 240x176  4
  {{0xF0, 0x00}, {0xF0, 0x00}}, // FRAMESIZE_240X240,  // 240x240  5
  {{0x40, 0x01}, {0xF0, 0x00}}, // FRAMESIZE_QVGA,     // 320x240  6
  {{0x40, 0x01}, {0xF0, 0x00}}, // FRAMESIZE_320X320,  // 320x320  7
  {{0x90, 0x01}, {0x28, 0x01}}, // FRAMESIZE_CIF,      // 400x296  8
  {{0xE0, 0x01}, {0x40, 0x01}}, // FRAMESIZE_HVGA,     // 480x320  9
  {{0x80, 0x02}, {0xE0, 0x01}}, // FRAMESIZE_VGA,      // 640x480  10
  //               38,400    61,440    153,600
  {{0x20, 0x03}, {0x58, 0x02}}, // FRAMESIZE_SVGA,     // 800x600   11
  {{0x00, 0x04}, {0x00, 0x03}}, // FRAMESIZE_XGA,      // 1024x768  12
  {{0x00, 0x05}, {0xD0, 0x02}}, // FRAMESIZE_HD,       // 1280x720  13
  {{0x00, 0x05}, {0x00, 0x04}}, // FRAMESIZE_SXGA,     // 1280x1024 14
  {{0x40, 0x06}, {0xB0, 0x04}}, // FRAMESIZE_UXGA,     // 1600x1200 15
  // 3MP Sensors
  {{0x80, 0x07}, {0x38, 0x04}}, // FRAMESIZE_FHD,      // 1920x1080 16
  {{0xD0, 0x02}, {0x00, 0x05}}, // FRAMESIZE_P_HD,     //  720x1280 17
  {{0x60, 0x03}, {0x00, 0x06}}, // FRAMESIZE_P_3MP,    //  864x1536 18
  {{0x00, 0x08}, {0x00, 0x06}}, // FRAMESIZE_QXGA,     // 2048x1536 19
  // 5MP Sensors
  {{0x00, 0x0A}, {0xA0, 0x05}}, // FRAMESIZE_QHD,      // 2560x1440 20
  {{0x00, 0x0A}, {0x40, 0x06}}, // FRAMESIZE_WQXGA,    // 2560x1600 21
  {{0x38, 0x04}, {0x80, 0x07}}, // FRAMESIZE_P_FHD,    // 1080x1920 22
  {{0x00, 0x0A}, {0x80, 0x07}}  // FRAMESIZE_QSXGA,    // 2560x1920 23

};

const int avi_header[AVIOFFSET] PROGMEM = 
{
  0x52, 0x49, 0x46, 0x46, 0xD8, 0x01, 0x0E, 0x00, 0x41, 0x56, 0x49, 0x20, 0x4C, 0x49, 0x53, 0x54,
  0xD0, 0x00, 0x00, 0x00, 0x68, 0x64, 0x72, 0x6C, 0x61, 0x76, 0x69, 0x68, 0x38, 0x00, 0x00, 0x00,
  0xA0, 0x86, 0x01, 0x00, 0x80, 0x66, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x02, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x49, 0x53, 0x54, 0x84, 0x00, 0x00, 0x00,
  0x73, 0x74, 0x72, 0x6C, 0x73, 0x74, 0x72, 0x68, 0x30, 0x00, 0x00, 0x00, 0x76, 0x69, 0x64, 0x73,
  0x4D, 0x4A, 0x50, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x74, 0x72, 0x66,
  0x28, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00,
  0x01, 0x00, 0x18, 0x00, 0x4D, 0x4A, 0x50, 0x47, 0x00, 0x84, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x4E, 0x46, 0x4F,
  0x10, 0x00, 0x00, 0x00, 0x6A, 0x61, 0x6D, 0x65, 0x73, 0x7A, 0x61, 0x68, 0x61, 0x72, 0x79, 0x20,
  0x76, 0x36, 0x32, 0x20, 0x4C, 0x49, 0x53, 0x54, 0x00, 0x01, 0x0E, 0x00, 0x6D, 0x6F, 0x76, 0x69,
};


//
// Writes an uint32_t in Big Endian at current file position
//
static void inline print_quartet(unsigned long i, File fd) {

  uint8_t y[4];
  y[0] = i % 0x100;
  y[1] = (i >> 8) % 0x100;
  y[2] = (i >> 16) % 0x100;
  y[3] = (i >> 24) % 0x100;
  size_t i1_err = fd.write(y , 4);
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

#include "lwip/sockets.h"
#include <lwip/netdb.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  delete_old_stuff() - delete oldest files to free diskspace
//

#include <list>
#include <tuple>

void delete_old_stuff() {
  using namespace std;
  using records = tuple<String, String, size_t, time_t>;
  list<records> dirList;

  int card = SD_MMC.cardSize()  / (1024 * 1024);
  int total = SD_MMC.totalBytes()  / (1024 * 1024);
  int used = SD_MMC.usedBytes()  / (1024 * 1024);

  jpr("Card  space: %5dMB\n", card);  // %llu
  jpr("Total space: %5dMB\n", total);
  jpr("Used  space: %5dMB\n", used);

  float full = 1.0 * used / total;
  if (full  <  0.8) {
    jpr("Nothing deleted, %.1f%% disk full\n", 100.0 * full);
  } else {
    jpr("Disk is %.1f%% full ... deleting ...\n", 100.0 * full);

    int x = millis();
    File xdir = SD_MMC.open("/");
    File xf = xdir.openNextFile();

    while (xf) {
      if (xf.isDirectory()) {
        String the_dir = xf.name();
        if (SD_MMC.rmdir("/" + the_dir )) {                      // remove empty dir
          jpr("Dir removed\n"); Serial.println("/" + the_dir);
        } else {
          String log_name = "/" + the_dir + "/" + the_dir + ".999.txt";
          //Serial.println(log_name);
          File the_log = SD_MMC.open(log_name, "r");
          time_t the_fold = xf.getLastWrite();
          time_t the_logfile = the_log.getLastWrite();
          the_log.close();

          if ( the_fold > the_logfile) {
            dirList.emplace_back("", the_dir, 0, the_fold);
          } else {
            dirList.emplace_back("", the_dir, 0, the_logfile);
            //Serial.printf("Log is newer than dir by %d -- ", the_logfile - the_fold);
          }
        }
      } else {
        // skip files
        //dirList.emplace_back("", xf.name(), xf.size(), xf.getLastWrite());
        //Serial.printf("Added: "); Serial.println(xf.name());
      }
      xf = xdir.openNextFile();
    }
    xdir.close();

    dirList.sort([](const records & f, const records & l) {                                 // sort by date
      return get<3>(f) < get<3>(l);
      return false;
    });

    jpr("Sort files took %d ms\n", millis() - x);

    for ( auto& iter : dirList) {
      String fn =  get<1>(iter);

      //jpr("Oldest file is "); Serial.print(fn);
      deleteFolderOrFile(fn.c_str());

      total = SD_MMC.totalBytes()  / (1024 * 1024);
      used = SD_MMC.usedBytes()  / (1024 * 1024);

      full = 1.0 * used / total;

      Serial.println(full);
      if (full < 0.7) break;
    }
  }
}

void deleteFolderOrFile(const char * val) {
  Serial.printf("Deleting : %s\n", val);
  File f = SD_MMC.open("/" + String(val));
  if (!f) {
    jpr("Failed to open %s\n", val);
    return;
  }

  if (f.isDirectory()) {
    File file = f.openNextFile();
    while (file) {
      if (file.isDirectory()) {
        Serial.print("  DIR : ");
        Serial.println(file.name());
      } else {
        Serial.print("  FILE: ");
        Serial.print(file.name());
        Serial.print("  SIZE: ");
        Serial.print(file.size());
        if (SD_MMC.remove("/" + String(val) + "/" + file.name())) {
          Serial.println(" deleted.");
        } else {
          Serial.println(" FAILED.");
        }
      }
      int total = SD_MMC.totalBytes()  / (1024 * 1024);
      int used = SD_MMC.usedBytes()  / (1024 * 1024);

      float full = 1.0 * used / total;

      Serial.println(full);
      if (full < 0.7) break;
      file = f.openNextFile();
    }
    f.close();
    //Remove the dir
    if (SD_MMC.rmdir("/" + String(val))) {
      Serial.printf("Dir %s removed\n", val);
    } else {
      Serial.println("Remove dir failed");
    }

  } else {
    //Remove the file
    if (SD_MMC.remove("/" + String(val))) {
      Serial.printf("File %s deleted\n", val);
    } else {
      Serial.println("Delete failed");
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  get_good_jpeg()  - take a picture and make sure it has a good jpeg
//
camera_fb_t *  get_good_jpeg() {

  camera_fb_t * fb;

  long start;
  int failures = 0;

  do {
    int fblen = 0;
    int foundffd9 = 0;
    long bp = millis();
    long mstart = micros();

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera Capture Failed");
      failures++;
    } else {
      long mdelay = micros() - mstart;

      int get_fail = 0;

      totalp = totalp + millis() - bp;
      time_in_camera = totalp;

      fblen = fb->len;

      for (int j = 1; j <= 1025; j++) {
        if (fb->buf[fblen - j] != 0xD9) {
          // no d9, try next for
        } else {                                     //Serial.println("Found a D9");
          if (fb->buf[fblen - j - 1] == 0xFF ) {     //Serial.print("Found the FFD9, junk is "); Serial.println(j);
            if (j == 1) {
              normal_jpg++;
            } else {
              extend_jpg++;
            }
            foundffd9 = 1;
            if (Lots_of_Stats) {
              if (j > 9000) {                // was 900             //  rarely happens - sometimes on 2640
                jpr("Frame %d, Len %d, Extra %d ", frame_cnt, fblen, j - 1 );
                logfile.flush();
              }

              if ( (frame_cnt % 1000 == 50) || (frame_cnt < 1000 && frame_cnt % 100 == 50)) {
                gframe_cnt = frame_cnt;
                gfblen = fblen;
                gj = j;
                gmdelay = mdelay;
                //Serial.printf("Frame %6d, len %6d, extra  %4d, cam time %7d ", frame_cnt, fblen, j - 1, mdelay / 1000);
                //logfile.printf("Frame %6d, len %6d, extra  %4d, cam time %7d ", frame_cnt, fblen, j - 1, mdelay / 1000);
                do_it_now = 1;
              }
            }
            break;
          }
        }
      }

      if (!foundffd9) {
        bad_jpg++;
        jpr("Bad jpeg, Frame %d, Len = %d \n", frame_cnt, fblen);
        esp_camera_fb_return(fb);
        failures++;

      } else {
        break;
        // count up the useless bytes
      }
    }

  } while (failures < 10);   // normally leave the loop with a break()

  // if we get 10 bad frames in a row, then quality parameters are too high - set them lower (+5), and start new movie
  if (failures == 10) {
    jpr("10 failures");

    sensor_t * ss = esp_camera_sensor_get();
    int qual = ss->status.quality ;
    ss->set_quality(ss, qual + 5);
    quality = qual + 5;
    jpr("\n\nDecreasing quality due to frame failures %d -> %d\n\n", qual, qual + 5);
    delay(1000);

    start_record = 0;
    //reboot_now = true;
  }
  return fb;
}

/*
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  eprom functions  - increment the file_group, so files are always unique
//

#include <EEPROM.h>

struct eprom_data {
  int eprom_good;
  int file_group;
};

void do_eprom_read() {

  eprom_data ed;

  EEPROM.begin(200);
  EEPROM.get(0, ed);

  if (ed.eprom_good == MagicNumber) {
    jpr("Good settings in the EPROM ");
    file_group = ed.file_group;
    file_group++;
    jpr("New File Group "); Serial.println(file_group );
  } else {
    jpr("No settings in EPROM - Starting with File Group 1 ");
    file_group = 1;
  }
  do_eprom_write();
  file_number = 1;
}

void do_eprom_write() {

  eprom_data ed;
  ed.eprom_good = MagicNumber;
  ed.file_group  = file_group;

  Serial.println("Writing to EPROM ...");

  EEPROM.begin(200);
  EEPROM.put(0, ed);
  EEPROM.commit();
  EEPROM.end();
}
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Make the avi functions
//
//   start_avi() - open the file and write headers
//   another_pic_avi() - write one more frame of movie
//   end_avi() - write the final parameters and close the file


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// start_avi - open the files and write in headers
//

static void start_avi() {
  char the_directory[50];

  long start = millis();

  jpr("Starting an avi ");
  sprintf(the_directory, "/%s%03d",  devname, file_group);
  SD_MMC.mkdir(the_directory);

  sprintf(avi_file_name, "/%s%03d/%s%03d.%03d.avi",  devname, file_group, devname, file_group, file_number);

  file_number++;

  avifile = SD_MMC.open(avi_file_name, "w");
  idxfile = SD_MMC.open("/idx.tmp", "w");

  if (avifile) {
    jpr("File open: %s\n", avi_file_name);
  }  else  {
    jpr("Could not open avi file");
    major_fail();
  }

  if (idxfile)  {
    //Serial.printf("File open: %s\n", "//idx.tmp");
  }  else  {
    jpr("Could not open file /idx.tmp");
    major_fail();
  }

  for ( i = 0; i < AVIOFFSET; i++) {
    char ch = pgm_read_byte(&avi_header[i]);
    buf[i] = ch;
  }

  memcpy(buf + 0x40, frameSizeData[framesize].frameWidth, 2);
  memcpy(buf + 0xA8, frameSizeData[framesize].frameWidth, 2);
  memcpy(buf + 0x44, frameSizeData[framesize].frameHeight, 2);
  memcpy(buf + 0xAC, frameSizeData[framesize].frameHeight, 2);

  size_t err = avifile.write(buf, AVIOFFSET);

  uint8_t ex_fps = 1;
  if (frame_interval == 0) {
    if (framesize >= 11) {
      ex_fps = 12.5 * speed_up_factor ;;
    } else {
      ex_fps = 25.0 * speed_up_factor;
    }
  } else {
    ex_fps = round(1000.0 / frame_interval * speed_up_factor);
  }

  avifile.seek( 0x84 , SeekSet);
  print_quartet((int)ex_fps, avifile);

  avifile.seek( 0x30 , SeekSet);
  print_quartet(3, avifile);  // magic number 3 means frame count not written // 61.3

  avifile.seek( AVIOFFSET, SeekSet);

  jpr("Recording %d seconds\n", avi_length);

  startms = millis();

  totalp = 0;
  totalw = 0;

  jpeg_size = 0;
  movi_size = 0;
  uVideoLen = 0;
  idx_offset = 4;

  bad_jpg = 0;
  extend_jpg = 0;
  normal_jpg = 0;

  time_in_loop = 0;
  time_in_camera = 0;
  time_in_sd = 0;
  time_in_good = 0;
  time_total = 0;
  time_in_web1 = 0;
  time_in_web2 = 0;
  delay_wait_for_sd = 0;
  wait_for_cam = 0;
  very_high = 0;

  time_in_sd += (millis() - start);

  logfile.flush();
  avifile.flush();

} // end of start avi

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  another_save_avi saves another frame to the avi file, uodates index
//           -- pass in a fb pointer to the frame to add
//

static void another_save_avi(uint8_t* fb_buf, int fblen ) {
  long start = millis();

  int fb_block_length;
  uint8_t* fb_block_start;

  jpeg_size = fblen;

  remnant = (4 - (jpeg_size & 0x00000003)) & 0x00000003;

  long bw = millis();
  long frame_write_start = millis();

  int block_delay[10];
  int block_num = 0;

  fb_record_static[0] = 0x30;       // "00dc"
  fb_record_static[1] = 0x30;
  fb_record_static[2] = 0x64;
  fb_record_static[3] = 0x63;

  int jpeg_size_rem = jpeg_size + remnant;

  fb_record_static[4] = jpeg_size_rem % 0x100;
  fb_record_static[5] = (jpeg_size_rem >> 8) % 0x100;
  fb_record_static[6] = (jpeg_size_rem >> 16) % 0x100;
  fb_record_static[7] = (jpeg_size_rem >> 24) % 0x100;

  fb_block_start = fb_buf;

  if (fblen > fbs * 1024 - 8 ) {                     // fbs is the size of frame buffer static
    fb_block_length = fbs * 1024;
    fblen = fblen - (fbs * 1024 - 8);
    memcpy(fb_record_static + 8, fb_block_start, fb_block_length - 8);
    fb_block_start = fb_block_start + fb_block_length - 8;

  } else {
    fb_block_length = fblen + 8  + remnant;
    memcpy(fb_record_static + 8, fb_block_start,  fblen);
    fblen = 0;
  }

  size_t err = avifile.write(fb_record_static, fb_block_length);

  if (err != fb_block_length) {
    start_record = 0;
    jpr("Giving up - Error on avi write: err = %d, len = %d \n", err, fb_block_length);
    return;
  }

  if (block_num < 10) block_delay[block_num++] = millis() - bw;

  while (fblen > 0) {

    if (fblen > fbs * 1024) {
      fb_block_length = fbs * 1024;
      fblen = fblen - fb_block_length;
    } else {
      fb_block_length = fblen  + remnant;
      fblen = 0;
    }

    memcpy(fb_record_static, fb_block_start, fb_block_length);

    size_t err = avifile.write(fb_record_static,  fb_block_length);

    if (err != fb_block_length) {
      jpr("Giving up - Error on avi write: err = %d, len = %d \n", err, fb_block_length);
      return;
    }

    if (block_num < 10) block_delay[block_num++] = millis() - bw;

    fb_block_start = fb_block_start + fb_block_length;
    delay(0);
  }


  movi_size += jpeg_size;
  uVideoLen += jpeg_size;
  long frame_write_end = millis();

  print_2quartet(idx_offset, jpeg_size, idxfile);

  idx_offset = idx_offset + jpeg_size + remnant + 8;

  movi_size = movi_size + remnant;

  if ( do_it_now == 1 ) {  // && frame_cnt < 1011
    do_it_now = 0;
    //jpr("Frame %6d, len %6d, extra  %4d, cam time %7d,  sd time %4d -- \n", gframe_cnt, gfblen, gj - 1, gmdelay / 1000, millis() - bw);
    jpr("Frame %6d, len %6d, cam time %7d,  sd time %4d -- \n", gframe_cnt, gfblen, gmdelay / 1000, millis() - bw);
    logfile.flush();
  }

  totalw = totalw + millis() - bw;
  time_in_sd += (millis() - start);


  if ( (millis() - bw) > totalw / frame_cnt * 10) {
    unsigned long x = avifile.position();
    jpr ("Frame %6d, sd time very high %4d >>> %4d -- pos %X, ",  frame_cnt, millis() - bw, (totalw / frame_cnt), x );

    very_high++;
    jpr("Block %d, delay %5d ... \n", 0, block_delay[0]);
    //for (int i = 1; i < block_num; i++) {
    //  jpr("Block %d, delay %5d ..., ", i, block_delay[i] - block_delay[i - 1]);
    //}
    //Serial.println(" ");
    //logfile.println(" ");
  }
  avifile.flush();
  idxfile.flush();

} // end of another_pic_avi

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  end_avi writes the index, and closes the files
//

static void end_avi() {

  long start = millis();

  unsigned long current_end = avifile.position();

  jpr("End of avi - closing the files");

  if (frame_cnt <  5 ) {
    jpr("Recording screwed up, less than 5 frames, forget index\n");
    idxfile.close();
    avifile.close();
    int xx = remove("/idx.tmp");
    int yy = remove(avi_file_name);

  } else {

    elapsedms = millis() - startms;

    float fRealFPS = (1000.0f * (float)frame_cnt) / ((float)elapsedms) * speed_up_factor;

    float fmicroseconds_per_frame = 1000000.0f / fRealFPS;
    uint8_t iAttainedFPS = round(fRealFPS) ;
    uint32_t us_per_frame = round(fmicroseconds_per_frame);

    //Modify the MJPEG header from the beginning of the file, overwriting various placeholders

    avifile.seek( 4 , SeekSet);
    print_quartet(movi_size + 240 + 16 * frame_cnt + 8 * frame_cnt, avifile);

    avifile.seek( 0x20 , SeekSet);
    print_quartet(us_per_frame, avifile);

    unsigned long max_bytes_per_sec = (1.0f * movi_size * iAttainedFPS) / frame_cnt;

    avifile.seek( 0x24 , SeekSet);
    print_quartet(max_bytes_per_sec, avifile);

    avifile.seek( 0x30 , SeekSet);
    print_quartet(frame_cnt, avifile);

    avifile.seek( 0x8c , SeekSet);
    print_quartet(frame_cnt, avifile);

    avifile.seek( 0x84 , SeekSet);
    print_quartet((int)iAttainedFPS, avifile);

    avifile.seek( 0xe8 , SeekSet);
    print_quartet(movi_size + frame_cnt * 8 + 4, avifile);

    jpr("\n*** Video recorded and saved ***\n");

    jpr("Recorded %5d frames in %5d seconds\n", frame_cnt, elapsedms / 1000);
    jpr("File size is %u bytes\n", movi_size + 12 * frame_cnt + 4);
    jpr("Adjusted FPS is %5.2f\n", fRealFPS);
    jpr("Max data rate is %lu bytes/s\n", max_bytes_per_sec);
    jpr("Frame duration is %d us\n", us_per_frame);
    jpr("Average frame length is %d bytes\n", uVideoLen / frame_cnt);
    jpr("Average picture time (ms) %f\n", 1.0 * totalp / frame_cnt);
    jpr("Average write time (ms)  %f\n", 1.0 * totalw / frame_cnt );
    jpr("Normal jpg % %3.1f\n", 100.0 * normal_jpg / frame_cnt );
    jpr("Extend jpg % %3.1f\n", 100.0 * extend_jpg / frame_cnt );
    jpr("Bad    jpg % %6.5f\n", 100.0 * bad_jpg / frame_cnt);
    jpr("Slow sd writes %d, %5.3f %% \n", very_high, 100.0 * very_high / frame_cnt, 5 );

    jpr("Writng the index, %d frames\n", frame_cnt);

    avifile.seek( current_end , SeekSet);

    idxfile.close();

    size_t i1_err = avifile.write(idx1_buf, 4);

    print_quartet(frame_cnt * 16, avifile);

    idxfile = SD_MMC.open("/idx.tmp", "r");

    if (idxfile)  {
      //Serial.printf("File open: %s\n", "//idx.tmp");
      //logfile.printf("File open: %s\n", "/idx.tmp");
    }  else  {
      jpr("Could not open index file");
      major_fail();
    }

    char * AteBytes;
    AteBytes = (char*) malloc (8);

    for (int i = 0; i < frame_cnt; i++) {
      size_t res = idxfile.readBytes( AteBytes, 8);
      size_t i1_err = avifile.write(dc_buf, 4);
      size_t i2_err = avifile.write(zero_buf, 4);
      size_t i3_err = avifile.write((uint8_t *)AteBytes, 8);
    }

    free(AteBytes);

    idxfile.close();
    avifile.close();

    //    int resss = SD_MMC.mkdir(the_directory);
    //    Serial.printf("remake the foler ?? %d\n",resss);
    int xx = SD_MMC.remove("/idx.tmp");
  }

  jpr("---\n");

  time_in_sd += (millis() - start);

  //Serial.println("");
  time_total = millis() - startms;
  jpr("waiting for cam %10dms, %4.1f%%\n", wait_for_cam , 100.0 * wait_for_cam  / time_total);
  jpr("Time in camera  %10dms, %4.1f%%\n", time_in_camera, 100.0 * time_in_camera / time_total);
  jpr("waiting for sd  %10dms, %4.1f%%\n", delay_wait_for_sd , 100.0 * delay_wait_for_sd  / time_total);
  jpr("Time in sd      %10dms, %4.1f%%\n", time_in_sd    , 100.0 * time_in_sd     / time_total);
  jpr("web (core 1)    %10dms, %4.1f%%\n", time_in_web1  , 100.0 * time_in_web1   / time_total);
  jpr("web (core 0)    %10dms, %4.1f%%\n", time_in_web2  , 100.0 * time_in_web2   / time_total);
  jpr("time total      %10dms, %4.1f%%\n", time_total    , 100.0 * time_total     / time_total);

  logfile.flush();

  if (file_number == 100) {
    reboot_now = true;
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Time
#include "time.h"

#include "WiFi.h"
#include <WiFiMulti.h>
WiFiMulti jMulti;
#include <ArduinoOTA.h>
//char ssidota[20];
#include "ESPmDNS.h"

#include "ESPxWebFlMgr.h"          //v56
//const word filemanagerport = 8080;
ESPxWebFlMgr filemgr(filemanagerport); // we want a different port than the webserver

/*
time_t now;
struct tm timeinfo;
*/
WiFiEventId_t eventID;
#include "esp_wifi.h"
bool found_router = false;

// ****************************************************************************
// *       Подключить локальные WiFi и создать одну свою от контроллера       *
// ****************************************************************************
bool init_wifi() 
{
  //uint32_t brown_reg_temp = READ_PERI_REG(RTC_CNTL_BROWN_OUT_REG);
  //Serial.printf("Brownout was %d\n", brown_reg_temp);
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  //WiFi.disconnect(true, true);

  // Устанавливаем режим работы WiFi, как станции (STA). В этом режиме контроллер 
  // не создаёт собственную сеть, а подключается к уже существующей сети WiFi, 
  // например, к локальной сети (роутеру или иному «раздающему» устройству). 
  //
  // В режиме STA ESP32 действует как другое клиентское устройство в сети (например, ноутбук или смартфон). 
  // Его основная цель — обнаружить и подключиться к точке доступа (AP) с помощью SSID (имени сети) и пароля (обычно Pre-Shared Key или PSK). 
  // После подключения ESP32 получает IP-адрес от AP (обычно через DHCP от роутера) 
  // и может общаться с другими устройствами в локальной сети и, если AP предоставляет это, получать доступ в интернет.
  //
  // Важно: подключение к Wi-Fi не является мгновенным, поэтому необходимо регулярно проверять статус соединения 
  // с помощью функции WiFi.status(). После успешного подключения функция возвращает WL_CONNECTED. 
  WiFi.mode(WIFI_STA);
  
  // Настраиваем имя, которое клиент DHCP использует для идентификации устройства
  // (в типичной сетевой настройке это имя отображается в списке устройств маршрутизатора Wi-Fi.
  // По умолчанию имя ESP32 — «espressif». С помощью этой функции можно изменить стандартное имя, 
  // например, для дифференциации устройств в режиме мягкого доступа к точке доступа. 
  // Аргумент - это строка, которая содержит новое имя. Она должна быть не длиннее 
  // 32 символов, содержать только буквы, цифры и символ «-»). 

  // Важно: функцию нужно вызвать до начала Wi-Fi с помощью WiFi.begin(), WiFi.softAP(), 
  // WiFi.mode() или WiFi.run(). Чтобы изменить имя, можно сбросить Wi-Fi с помощью WiFi.mode(WIFI_MODE_NULL), 
  // затем вызвать WiFi.setHostname() и перезагрузить Wi-Fi с нуля. 
  // Если hostname не указан, будет назначено стандартное имя на основе типа чипа и MAC-адреса. 
  WiFi.setHostname(devname);
  // Резервируем место для параметров входа первой локальной сети
  char ssidch1[20]; char passch1[20];
  // Резервируем место для параметров входа второй локальной сети
  char ssidch2[20]; char passch2[20];
  // Резервируем место для параметров входа собственной сети контроллера
  char ssidch3[20]; char passch3[20];
  // Заполняем параметры для WiFi сетей и показываем их
  cssid1.toCharArray(ssidch1, cssid1.length() + 1);
  cpass1.toCharArray(passch1, cpass1.length() + 1);
  cssid2.toCharArray(ssidch2, cssid2.length() + 1);
  cpass2.toCharArray(passch2, cpass2.length() + 1);
  if (cssid3 == "ssid") cssid3 = String(devname);
  cssid3.toCharArray(ssidch3, cssid3.length() + 1);
  cssid3.toCharArray(ssidota, cssid3.length() + 1);
  cpass3.toCharArray(passch3, cpass3.length() + 1);
  jpr("\n>>>>>>>>>>>>>>>>>>>>>%s<\n", ssidch1);
  jpr  (">>>>>>>>>>>>>>>>>>>>>%s<\n", ssidch2);
  jpr  (">>>>>>>>>>>>>>>>>>>>>%s</>%s<\n", ssidch3, passch3);
  // Подключаемся к локальным сетям
  if (String(cssid1)!="ssid") 
  {
    found_router = true;
    jMulti.addAP(ssidch1, passch1);
  }
  if (String(cssid2)!="ssid") 
  {
    found_router = true;
    jMulti.addAP(ssidch2, passch2);
  }
  if (found_router) 
  {
    jMulti.run();
  }
  // Выбираем и показываем Mac-адрес WiFi
  String wifiMacString = WiFi.macAddress();
  Serial.println("Mac-адрес точки доступа: "+wifiMacString);
  // Выбираем и показываем версию ESP-IDF
  String idfver = esp_get_idf_version();
  Serial.println("Версия компилятора ESP:  "+idfver);

  // Задаем режим программной точки доступа (soft-AP) для установления Wi-Fi-сети. 
  // (то есть создаём собственную сеть Wi-Fi, а другие устройства (станции) могут 
  // подсоединяться к ней без необходимости соединения с маршрутизатором. 

  // Простая версия функции — WiFi.softAP(ssid) — используется для настройки открытой 
  // Wi-Fi-сети. Чтобы задать сеть, защищённую паролем, или настроить дополнительные 
  // параметры сети, используется вариант WiFi.softAP(ssid, password, channel, hidden). 
  // Первый параметр обязателен: ssid — символьная строка, содержащая SSID сети (не более 63 символов);
  // password — опциональная символьная строка для пароля. Для сети WPA-PSK её размер 
  // должен быть не более 8 символов; channel — параметр для настройки WiFi-канала (от «1» до «13»). 
  // Канал по умолчанию — «1»; hidden = true спрячет SSID.
  
  // По умолчанию IP-адресом настроенной программной точки доступа будет «192.168.4.1». 
  // Его можно поменять при помощи функции softAPConfig. 
  jprln("Контроллер устанавливает собственная точка доступа …");
  WiFi.softAP(ssidch3, passch3);
  /*
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  */
  sprintf(localip, "%s", WiFi.softAPIP().toString().c_str());
  Serial.print(_soft_IP); Serial.println(localip); 
  jprln("Контроллер подключается к локальной точке доступа …");
  // Инициируем нулевую попытку подключения
  int connAttempts = 0;
  Serial.println(" ");
  while (WiFi.status() != WL_CONNECTED ) 
  {
    delay(1000);
    Serial.print(".");
    if (connAttempts++ == 15) break;   
  }
  sprintf(localip,"%s",WiFi.localIP().toString().c_str());
  Serial.print(_localIP); Serial.println(localip); Serial.println(" ");

  jprln("Определяется локальное время …");
  configTime(0, 0, "pool.ntp.org");
  char tzchar[60];
  TIMEZONE.toCharArray(tzchar, TIMEZONE.length() + 1);        // name of your camera for mDNS, Router, and filenames
  Serial.printf("Char >%s<\n", tzchar);
  setenv("TZ", tzchar, 1);  // mountain time zone from #define at top
  tzset();
  time(&now);
  // try for 15 seconds to get the time, then give up - 10 seconds after boot
  while (now < 5) 
  {        
    delay(1000);
    Serial.print("o");
    time(&now);
  }
  Serial.print("Локальное время: "); Serial.print(ctime(&now));

  // Запускаем службу multicast DNS (mDNS). Это позволяет использовать имя хоста 
  // в веб-браузере вместо IP-адреса при доступе к ESP32 с компьютера. 
  // Особенности работы: динамическая обработка IP-адресов — даже если IP-адрес 
  // устройства изменится, mDNS автоматически преобразует новый IP-адрес в то же имя хоста;
  // нулевая настройка — дополнительная настройка DNS не требуется, поскольку mDNS 
  // работает автономно в локальных сетях; автоматическое добавление суффикса — 
  // если имя хоста уже существует в локальной сети, библиотека автоматически добавляет суффикс. 
  // Например, если esp32.local уже существует, она переименовывает его в esp32-2.local. 
  //
  // В коде ESP32 функция MDNS.begin() вызывается в функции setup() после подключения к сети. 
  // В качестве аргумента метода передаётся желаемое имя хоста. Например, если IP-адрес устройства
  // — 192.168.4.1, можно получить доступ к нему с помощью «esp32.local в веб-браузере. 
  // Важно: имя хоста не должно быть длиннее 63 символов. 
  // 
  // Некоторые ошибки, которые могут возникать при использовании функции MDNS.begin, и способы их решения:
  // а) mDNS не запущен в коде — если MDNS.begin() не вызван или помещён перед подключением к Wi-Fi, сервис не будет рекламироваться. 
  // Решение: инициализировать mDNS после успешного подключения, поместив MDNS.begin("esp32") 
  // внутри события подключения к Wi-Fi;
  // б) Firewall или антивирус блокируют mDNS — в системах без встроенной поддержки mDNS 
  // (Linux, Windows) правила брандмауэра могут препятствовать входящим ответам mDNS 
  // на порт 5353. Решение: проверить правила брандмауэра и, если нужно, изменить их;
  // в) сеть не позволяет многоадресный трафик — некоторые режимы AP, горячие точки или
  // гостевые сети Wi-Fi изолируют клиентов и блокируют многоадресный трафик, включая mDNS. 
  // Решение: проверить политики сети и, если нужно, изменить их.
  if (!MDNS.begin(devname)) 
  {
    jprln("Ошибка при запуске multicast DNS (mDNS)");
  } 
  else 
  {
    jprln("multicast DNS (mDNS) стартовал с именем '%s'", devname);
  }

  eventID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) 
  {
    //  info.disconnected.reason ==>  info.wifi_sta_disconnected.reason - update with esp32_arduino 2.00 v58
    if (info.wifi_sta_disconnected.reason != 201) 
    {
      jpr( "\nframe_cnt: %8d, WiFi event Reason: %d , Status: %d\n", frame_cnt, info.wifi_sta_disconnected.reason, WiFi.status());
    }
  });

  // Отключаем режим энергосбережения: 
  // esp_wifi_get_ps — функция из API драйвера Wi-Fi для платы ESP32, которая получает 
  // режим энергосбережения (sleep mode) Wi-Fi. 
  // Функция входит в класс WiFi и возвращает значение типа wifi_ps_type_t. 
  // Режим энергосбережения WiFi влияет на скорость соединения. Можно установить один из трёх режимов: 
  // WIFI_PS_NONE — режим отключён; WIFI_PS_MIN_MODEM — минимальное энергосбережение; WIFI_PS_MAX_MODEM — максимальное энергосбережение.
  // Функция позволяет системе автоматически просыпаться из сна, когда это требуется драйвером Wi-Fi, 
  // и поддерживать соединение с точкой доступа (AP). 
  // Функция esp_wifi_get_ps вызывается в коде приложения, когда нужно получить текущий 
  // режим энергосбережения Wi-Fi. Например, в функции, которая управляет работой Wi-Fi, 
  // можно вызвать WiFi.getSleep() — она вернёт значение типа wifi_ps_type_t.
  // Важно: режим энергосбережения влияет на то, как драйвер Wi-Fi обрабатывает 
  // пакеты данных — при включённом режиме энергосбережения полученные данные могут 
  // быть задержаны на период, указанный в настройках DTIM. 
  wifi_ps_type_t the_type;
  esp_err_t get_ps = esp_wifi_get_ps(&the_type);
  Serial.printf("Начальный режим энергосбережения: %d\n", the_type);
  esp_err_t set_ps = esp_wifi_set_ps(WIFI_PS_NONE);
  esp_err_t new_ps = esp_wifi_get_ps(&the_type);
  Serial.printf("-Текущий- режим энергосбережения: %d\n", the_type);
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, brown_reg_temp);
  return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//
#include <HTTPClient.h>

/*
httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream81_httpd = NULL;
httpd_handle_t stream82_httpd = NULL;

char the_page[4200];
int previous_capture = 0;
int capture_timer = 0;
int captures = 0;
int total_captures = 0;
int skips = 0;
int extras = 0;
*/

#include "lwip/sockets.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  Streaming stuff based on Random Nerd
//

bool start_streaming = false;
bool stream_82 = false;
bool stream_81 = false;

httpd_req_t *req_82;
httpd_req_t *req_81;

#define PART_BOUNDARY "123456789000000000000987654321"

static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

void the_streaming_loop (void* pvParameter);
int stream_81_frames ;
long stream_81_start ;
int stream_82_frames ;
long stream_82_start ;

static esp_err_t stream_82_handler(httpd_req_t *req) {

  esp_err_t res;
  long start = millis();

  print_mem("stream_82_handler");

  stream_82 = true;
  req_82 = req;
  stream_82_frames = 0;
  stream_82_start = millis();

  if (stream_82) {
    res = httpd_resp_set_type(req_82, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK) {
      stream_82 = false;
    }
  }

  time_in_web1 += (millis() - start);

  while (stream_82 == true) {          // we have to keep the *req alive
    delay(1000);
    //Serial.print("<82>");
  }
  Serial.println(" stream_82 done");
  delay(500);
  httpd_resp_send_408(req_82);
  req_82 = NULL;

  return ESP_OK;
}

static esp_err_t stream_81_handler(httpd_req_t *req) {

  esp_err_t res;
  long start = millis();

  print_mem("stream_81_handler");

  stream_81 = true;
  req_81 = req;
  stream_81_frames = 0;
  stream_81_start = millis();

  time_in_web1 += (millis() - start);

  if (stream_81) {
    res = httpd_resp_set_type(req_81, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK) {
      stream_81 = false;
    }
  }

  while (stream_81 == true) {          // we have to keep the *req alive
    delay(1000);
    //Serial.print("<81>");
  }
  Serial.println(" stream_81 done");
  delay(500);
  httpd_resp_send_408(req_81);
  req_81 = NULL;
  return ESP_OK;
}


////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  Streaming stuff based on Random Nerd
//


void the_streaming_loop (void* pvParameter) 
{

  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

  long start = millis();

  print_mem("MEM - стартовала задача the_streaming_loop     ");

  while (true) 
  {

    if (!stream_81 && !stream_82) 
    {
      delay(5);
    } 
    else 
    {
      if (stream_81) stream_81_frames++;
      if (stream_82) stream_82_frames++;

      xSemaphoreTake( baton, portMAX_DELAY );

      if (fb_record_time > (millis() - 500)) 
      {
        //Serial.printf("*");
        fb_streaming_len = fb_record_len;
        fb_streaming_time = fb_record_time;
        memcpy(fb_streaming, fb_record,  fb_record_len);  // v59.5
        xSemaphoreGive( baton );
      } 
      else 
      {
        xSemaphoreGive( baton );
        fb = esp_camera_fb_get(); //get_good_jpeg();
        //Serial.println("loop take");
        //Serial.printf("millis %d, fb1 %d, fb2 %d\n", millis(), fb_record_time, fb_streaming_time);
        if (!fb) 
        {
          Serial.println("Photos - Camera Capture Failed");  // i guess we stream the previous contents of fb_streaming //34
          //start_streaming = false;
        } 
        else 
        {
          //34 xSemaphoreTake( baton, portMAX_DELAY );
          fb_streaming_len = fb->len;
          fb_streaming_time = millis();
          memcpy(fb_streaming, fb->buf, fb->len);
          //34 xSemaphoreGive( baton );
          esp_camera_fb_return(fb);
        }
      }

      _jpg_buf_len = fb_streaming_len;
      _jpg_buf = fb_streaming;

      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);

      long send_time = millis();
      long xx;
      xx = millis();

      if (stream_82) {
        res = httpd_resp_send_chunk(req_82, (const char *)part_buf, hlen);
        if (res != ESP_OK) 
        {
          stream_82 = false;
          Serial.printf("Stream error - 82/1st %d\n", res);
        }
      }
      if (stream_81) 
      {
        res = httpd_resp_send_chunk(req_81, (const char *)part_buf, hlen);
        if (res != ESP_OK) 
        {
          stream_81 = false;
          Serial.printf("Stream error - 81/1st %d\n", res);
        }
      }

      xx = millis();

      if (stream_82) 
      {
        res = httpd_resp_send_chunk(req_82, (const char *)_jpg_buf, _jpg_buf_len);
        if (res != ESP_OK) 
        {
          stream_82 = false;
          Serial.printf("Stream error - 82/2nd %d\n", res);
        }
      }
      if (stream_81) 
      {
        res = httpd_resp_send_chunk(req_81, (const char *)_jpg_buf, _jpg_buf_len);
        if (res != ESP_OK) 
        {
          stream_81 = false;
          Serial.printf("Stream error - 81/2nd %d\n", res);
        }
      }

      xx = millis();

      if (stream_82) 
      {
        res = httpd_resp_send_chunk(req_82, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        if (res != ESP_OK) 
        {
          stream_82 = false;
          Serial.printf("Stream error - 82/3rd %d\n", res);
        }
      }
      if (stream_81) 
      {
        res = httpd_resp_send_chunk(req_81, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        if (res != ESP_OK) 
        {
          stream_81 = false;
          Serial.printf("Stream error - 81/3rd %d\n", res);
        }
      }

      if (stream_81_frames % 100 == 10) 
      {
        if (Lots_of_Stats) 
        {
          jpr("Stream 81 at %3.3f fps\n", (float)1000 * stream_81_frames / (millis() - stream_81_start));
        }
      }
      if (stream_82_frames % 100 == 10) {
        if (Lots_of_Stats) {
          jpr("Stream 82 at %3.3f fps\n", (float)1000 * stream_82_frames / (millis() - stream_82_start));
        }
      }

      int new_delay = stream_delay - (millis() - send_time);
      //Serial.printf(", streamdelay %5d, send_time %5d, newdelay %5d\n", stream_delay, millis() - send_time, new_delay);
      if (millis() - send_time > 5000) 
      {
        new_delay = 1000;
        Serial.printf("wifi slow %d - take a 1s break\n", millis() - send_time);
      }

      if (new_delay < 10) 
      {
        new_delay = 10;
      }

      delay(new_delay) ; //delay(stream_delay);

      start = millis();

    }
  }  // stream forever
}

void start_Stream_81_server() {
  httpd_config_t config2 = HTTPD_DEFAULT_CONFIG();
  config2.server_port = 81;
  config2.ctrl_port = 32123; //         = 32768,
  Serial.print("http Stream task prio: "); Serial.println(config2.task_priority);

  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_81_handler,
    .user_ctx  = NULL
  };

  if (httpd_start(&stream81_httpd, &config2) == ESP_OK) {
    httpd_register_uri_handler(stream81_httpd, &stream_uri);
  } else {
    Serial.println("Error with stream start 81");
  }

  Serial.println("Stream 81 http started");
}

void start_Stream_82_server() {
  httpd_config_t config2 = HTTPD_DEFAULT_CONFIG();
  config2.server_port = 82;
  config2.ctrl_port = 32124; //         = 32768,
  Serial.print("http Stream task prio: "); Serial.println(config2.task_priority);

  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_82_handler,
    .user_ctx  = NULL
  };

  if (httpd_start(&stream82_httpd, &config2) == ESP_OK) {
    httpd_register_uri_handler(stream82_httpd, &stream_uri);
  } else {
    Serial.println("Error with stream start 82");
  }

  Serial.println("Stream 82 http started");
}


/*
//////////////////////////////
//61.3 oneframe find_a_frame (char * avi_file_name, int frame_pct) ; // from avi.cpp file

struct oneframe {
  uint8_t* the_frame;
  int the_frame_length;
  long the_frame_number;
  long the_frame_total;
};
*/

/*
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
*/
//
// Writes an uint32_t in Big Endian at current file position
//
static void inline print_dc_quartet(unsigned long i, File fd) {

  uint8_t y[8];
  y[0] = 0x30;       // "00dc"
  y[1] = 0x30;
  y[2] = 0x64;
  y[3] = 0x63;

  y[4] = i % 0x100;
  y[5] = (i >> 8) % 0x100;
  y[6] = (i >> 16) % 0x100;
  y[7] = (i >> 24) % 0x100;
  size_t i1_err = fd.write(y , 8);
}
void re_index( char * avi_file_name, char * out_file_name) 
{
  //once++;
  //if (once > 1) return;

  extern uint8_t* fb_faf;
  uint16_t remnant = 0;

  // JamCam0005.0037.avi
  //const char * avi_file_name = "JamCam0090.0001.avi";
  const char * idx_file_name = "/re_idx.tmp"; // "/JamCam0190.0001.idx";
  //const char * out_file_name = "/JamCam0090.0001new.avi";

#define fbs 4 //  how many kb of static ram for psram -> sram buffer for sd write
  uint8_t fb_faf_static[fbs * 1024 + 20];

  File avifile = SD_MMC.open(avi_file_name, "r"); // avifile = SD_MMC.open(avi_file_name, "w");
  File idxfile = SD_MMC.open(idx_file_name, "w"); //idxfile = SD_MMC.open("/idx.tmp", "w");
  File outfile = SD_MMC.open(out_file_name, "w"); //idxfile = SD_MMC.open("/idx.tmp", "w");

  if (avifile) {
    //Serial.printf("File open: %s\n", avi_file_name);
  }  else  {
    Serial.printf("Could not open %s file\n", avi_file_name);
  }

  if (idxfile)  {
    //Serial.printf("File open: %s\n", idx_file_name);
  }  else  {
    Serial.printf("Could not open file %s\n", idx_file_name);
  }

  if (!avifile) {
    return;
  } else {
    //size_t err = avifile.read( fb_faf_static, 240);

    avifile.seek( 0x24 , SeekSet);
    int max_bytes_per_sec  = read_quartet( avifile);
    //Serial.printf("Max bytes per sec %d\n", max_bytes_per_sec);

    avifile.seek( 0x30 , SeekSet);
    int frame_cnt = read_quartet( avifile);
    //Serial.printf("Frames %d\n", frame_cnt);

    /*
        if (frame_cnt < frame_start) {
          Serial.printf("Only %d frames, less than %d frame_start -- start at 0\n", frame_cnt, frame_end);
          frame_start = 0;
        }
        // if frame_end is 0, or too high, it will go to frame_cnt,
        if (frame_cnt < frame_end || frame_end == 0) {
          Serial.printf("Only %d frames, less than %d frame_end -- end at max frames\n", frame_cnt, frame_end);
          frame_end = frame_cnt;
        }
    */
    int num_out_frames = frame_cnt;   // / (skip_frames + 1);
    Serial.printf("Original %d frames, so %d output frames\n", frame_cnt, num_out_frames );


    //avifile.seek( 0x8c , SeekSet);
    //int frame_cnt8c = read_quartet( avifile);

    avifile.seek( 0x84 , SeekSet);
    int iAttainedFPS = read_quartet( avifile);
    //Serial.printf("fps %d\n", iAttainedFPS);

    avifile.seek( 0xe8 , SeekSet);
    int index_start = read_quartet( avifile);
    //Serial.printf("Len of movi %d\n", index_start);

    Serial.printf("-----------------\n");

    avifile.seek( 0 , SeekSet);
    size_t err = avifile.read(fb_faf_static, AVIOFFSET);
    Serial.printf("avi read header %d\n", err);
    size_t err2 = outfile.write(fb_faf_static, AVIOFFSET);
    Serial.printf("avi write header %d\n", err2);
    outfile.seek( 240 , SeekSet);


    Serial.printf("-----------------\n");

    int xx2;
    int flen;
    int prev_frame_length = 0;
    int next_frame_start = 240;
    int new_frame_length;
    int prev_frame_start = 240;
    int index_frame_length;
    int index_frame_start;
    int idx_offset = 4;
    int movi_size = 0;
    int frame_cnt_out = 0;
    int frame_num = 0;

    avifile.seek( next_frame_start  , SeekSet);  //240

    bool one_more_frame = true;

    //for (int frame_num = 0; frame_num < frame_cnt; frame_num = frame_num + 1) {

    while (one_more_frame) {

      //i avifile.seek( index_start + 244 + frame_num * 16 + 8 , SeekSet);
      //i index_frame_start = read_quartet( avifile);
      //i index_frame_length = read_quartet( avifile);

      //avifile.seek( next_frame_start  , SeekSet);   // start at 240, then read everything - 2 quart + frame, then repeat

      int the_oodc = read_quartet (avifile);   //240
      if (the_oodc == 1667510320) {
        //Serial.printf("%d, good frame, num %d\n",the_oodc,frame_num);
      } else {
        Serial.printf("%d, bad frame, num %d\n", the_oodc, frame_num);
        break;
      }


      //avifile.seek( next_frame_start + 4  , SeekSet);
      new_frame_length = read_quartet( avifile); //244

      index_frame_length = new_frame_length; // reuse the variable

      prev_frame_start = next_frame_start;
      index_frame_start = prev_frame_start; // reuse

      next_frame_start = prev_frame_start + new_frame_length + 8;

      //prev_frame_length = new_frame_length;

      if (frame_num < 5 || frame_num % 500 == 0) {
        Serial.printf("Frame %4d, index len %9d, frame len %9d, index start %9d, frame start %9d\n", frame_num, index_frame_length, new_frame_length, index_frame_start + 236 , prev_frame_start);
      }

      if (frame_num < 5 || frame_cnt_out % 100 == 0) {
        Serial.printf("Frame %4d, index len %9d, frame len %9d, index start %9d, frame start %9d\n", frame_num, index_frame_length, new_frame_length, index_frame_start + 236 , prev_frame_start);
        Serial.printf("in %d, out %d\n", frame_num, frame_cnt_out);
      }

      //avifile.seek( index_frame_start + 244 , SeekSet); // already 248

      remnant = (4 - (index_frame_length & 0x00000003)) & 0x00000003;
      int index_frame_length_rem = index_frame_length + remnant;

      int left_to_write = index_frame_length_rem;

      // check next frame start
      int where_now = avifile.position();
      avifile.seek( where_now + left_to_write  , SeekSet);
      the_oodc = read_quartet (avifile);
      if (the_oodc == 1667510320) {
        //Serial.printf("%d, good frame, num %d\n",the_oodc,frame_num);
      } else {
        Serial.printf("%d, next frame is bad frame, num %d\n", the_oodc, frame_num);

        break;
      }
      avifile.seek( where_now  , SeekSet);
      // now write the dc and length

      print_dc_quartet( index_frame_length_rem, outfile);

      while (left_to_write > 0) {
        if (left_to_write > fbs * 1024) {
          size_t err = avifile.read(fb_faf_static, fbs * 1024);
          size_t err2 = outfile.write(fb_faf_static, fbs * 1024);
          left_to_write = left_to_write - fbs * 1024;
        } else {
          size_t err = avifile.read(fb_faf_static, left_to_write);
          size_t err2 = outfile.write(fb_faf_static, left_to_write);
          left_to_write = 0;
        }
      }

      movi_size += index_frame_length;
      movi_size += remnant;

      print_2quartet(idx_offset, index_frame_length, idxfile);

      idx_offset = idx_offset + index_frame_length_rem + 8;

      frame_cnt_out++;
      frame_num++;

    }  // every frame in file


    //Serial.printf("frame %4d, outfile %9d, avifile %9d, idxfile %9d\n", frame_cnt , outfile.position(), avifile.position(), avifile.position());

    idxfile.close();
    size_t i1_err = outfile.write(idx1_buf, 4);
    if (!i1_err) Serial.printf("idx write\n");

    print_quartet(frame_cnt_out * 16, outfile);

    idxfile = SD_MMC.open(idx_file_name, "r");
    if (idxfile)  {
      Serial.printf("File open: %s\n", idx_file_name);
    }  else  {
      Serial.printf("Could not open file %s\n", idx_file_name);
    }

    char * AteBytes;
    AteBytes = (char*) malloc (8);

    for (int i = 0; i < frame_cnt_out; i++) {
      size_t res = idxfile.readBytes( AteBytes, 8);
      if (!res) Serial.printf("idx read\n");
      size_t i1_err = outfile.write(dc_and_zero_buf, 8);
      if (!i1_err) Serial.printf("dc write\n");
      //size_t i2_err = outfile.write(zero_buf, 4);
      //if (!i2_err) Serial.printf("zero write\n");
      size_t i3_err = outfile.write((uint8_t *)AteBytes, 8);
      if (!i3_err) Serial.printf("ate write\n");
    }

    free(AteBytes);

    outfile.seek( 4 , SeekSet);         //shit
    print_quartet(movi_size + 240 + 16 * frame_cnt_out + 8 * frame_cnt_out, outfile);

    avifile.seek( 0xe8 , SeekSet);
    int lom = read_quartet( avifile);
    Serial.printf("Len of movi was %d, now is %d\n", lom, movi_size);

    outfile.seek( 0xe8 , SeekSet);
    // shit print_quartet (movi_size, outfile);
    print_quartet(movi_size + frame_cnt_out * 8 + 4, outfile);

    avifile.seek( 0x30 , SeekSet);
    int fc = read_quartet( avifile);
    Serial.printf("Frames was %d, now is %d\n", fc, frame_cnt_out);
    outfile.seek( 0x30 , SeekSet);
    print_quartet (frame_cnt_out, outfile);
    outfile.seek( 0x8c , SeekSet);
    print_quartet (frame_cnt_out, outfile);

    //avifile.seek( 0x84 , SeekSet);
    //int fps = read_quartet( avifile);
    //Serial.printf("fps was %d\n", fps);

    //float fnewfps = ( 1.0f * fps ) / (1 + skip_frames)   ;
    //int newfps = round(fnewfps);

    //Serial.printf("newfps is %f, %d\n", fnewfps, newfps);
    //avifile.seek( 0x84 , SeekSet);
    //print_quartet(newfps, avifile);

    //avifile.seek( 0x20 , SeekSet);
    //int us_per_frame = read_quartet( avifile);
    //Serial.printf("us_per_frame was %d\n", us_per_frame);

    //float newus = 1000000.0f / fnewfps;
    //uint32_t new_us_per_frame = round (newus);


    //Serial.printf("new_us_per_frame is %f, %d\n", newus, new_us_per_frame);
    //avifile.seek( 0x20 , SeekSet);
    //print_quartet(new_us_per_frame, avifile);


    idxfile.close();
    avifile.close();
    outfile.close();

    int xx = SD_MMC.remove(idx_file_name);
  }
}

void re_index_bad( char * avi_file_name) {

  //once++;
  //if (once > 1) return;

  extern uint8_t* fb_faf;
  uint16_t remnant = 0;

  const char * idx_file_name = "/reidx.tmp"; // "/JamCam0190.0001.idx";

  //#define fbs 4 //  how many kb of static ram for psram -> sram buffer for sd write
  //  uint8_t fb_faf_static[fbs * 1024 + 20];

  File  avifile = SD_MMC.open(avi_file_name, "w"); // avifile = SD_MMC.open(avi_file_name, "w");
  File idxfile = SD_MMC.open(idx_file_name, "w"); //idxfile = SD_MMC.open("/idx.tmp", "w");
  //outfile = SD_MMC.open(out_file_name, "w"); //idxfile = SD_MMC.open("/idx.tmp", "w");

  if (avifile) {
    Serial.printf("File open: %s\n", avi_file_name);
  }  else  {
    Serial.printf("Could not open %s file\n", avi_file_name);
  }

  if (idxfile)  {
    Serial.printf("File open: %s\n", idx_file_name);
  }  else  {
    Serial.printf("Could not open file %s\n", idx_file_name);
  }

  if (!avifile) {
    return;
  } else {
    //size_t err = avifile.read( fb_faf_static, 240);

    avifile.seek( 0x24 , SeekSet);
    int max_bytes_per_sec  = read_quartet( avifile);
    Serial.printf("Max bytes per sec %d\n", max_bytes_per_sec);

    avifile.seek( 0x30 , SeekSet);
    int frame_cnt = read_quartet( avifile);
    Serial.printf("Frames %d\n", frame_cnt);

    /*
        if (frame_cnt < frame_start) {
          Serial.printf("Only %d frames, less than %d frame_start -- start at 0\n", frame_cnt, frame_end);
          frame_start = 0;
        }
        // if frame_end is 0, or too high, it will go to frame_cnt,
        if (frame_cnt < frame_end || frame_end == 0) {
          Serial.printf("Only %d frames, less than %d frame_end -- end at max frames\n", frame_cnt, frame_end);
          frame_end = frame_cnt;
        }
    */
    int num_out_frames = frame_cnt;   // / (skip_frames + 1);
    Serial.printf("Original %d frames, so %d output frames\n", frame_cnt, num_out_frames );


    //avifile.seek( 0x8c , SeekSet);
    //int frame_cnt8c = read_quartet( avifile);

    avifile.seek( 0x84 , SeekSet);
    int iAttainedFPS = read_quartet( avifile);
    Serial.printf("fps %d\n", iAttainedFPS);

    avifile.seek( 0xe8 , SeekSet);
    int index_start = read_quartet( avifile);
    Serial.printf("Len of movi %d\n", index_start);

    Serial.printf("-----------------\n");

    /*
        avifile.seek( 0 , SeekSet);
        size_t err = avifile.read(fb_faf_static, AVIOFFSET);
        Serial.printf("avi read header %d\n", err);
        size_t err2 = outfile.write(fb_faf_static, AVIOFFSET);
        Serial.printf("avi write header %d\n", err2);

        outfile.seek( 240 , SeekSet);
    */

    Serial.printf("-----------------\n");

    int xx2;
    int flen;
    int prev_frame_length = 0;
    int next_frame_start = 240;
    int new_frame_length;
    int prev_frame_start = 240;
    int index_frame_length;
    int index_frame_start;
    int idx_offset = 4;
    int movi_size = 0;
    int frame_cnt_out = 0;
    int frame_num = 0;

    int the;
    next_frame_start = 232;
    avifile.seek(  next_frame_start , SeekSet);
    Serial.printf("addr %d, ", next_frame_start);
    the = read_quartet (avifile);

    next_frame_start = next_frame_start + 4;
    avifile.seek(  next_frame_start , SeekSet);
    Serial.printf("addr %d, ", next_frame_start);
    the = read_quartet (avifile);

    next_frame_start = next_frame_start + 4;
    avifile.seek(  next_frame_start , SeekSet);
    Serial.printf("addr %d, ", next_frame_start);
    the = read_quartet (avifile);

    next_frame_start = next_frame_start + 4;
    avifile.seek(  next_frame_start , SeekSet);
    Serial.printf("addr %d, ", next_frame_start);
    the = read_quartet (avifile);

    next_frame_start = next_frame_start + 4;
    avifile.seek(  next_frame_start , SeekSet);
    Serial.printf("addr %d, ", next_frame_start);
    the = read_quartet (avifile);



    next_frame_start = 240;


    bool one_more_frame = true;

    //for (int frame_num = 0; frame_num < frame_cnt; frame_num = frame_num + 1) {

    size_t start_index_here;

    while (one_more_frame) {

      //i avifile.seek( index_start + 244 + frame_num * 16 + 8 , SeekSet);
      //i index_frame_start = read_quartet( avifile);
      //i index_frame_length = read_quartet( avifile);

      //avifile.seek( next_frame_start  , SeekSet);   // start at 240, then read everything - 2 quart + frame, then repeat

      avifile.seek( next_frame_start  , SeekSet);
      start_index_here = avifile.position();
      int the_oodc = read_quartet (avifile);   //240
      if (the_oodc == 1667510320) {
        //Serial.printf("%d, good frame, num %d\n",the_oodc,frame_num);
      } else {
        Serial.printf("%d, bad frame, num %d\n", the_oodc, frame_num);
        break;
      }




      //avifile.seek( next_frame_start + 4  , SeekSet);
      new_frame_length = read_quartet( avifile); //244

      index_frame_length = new_frame_length; // reuse the variable

      prev_frame_start = next_frame_start;
      index_frame_start = prev_frame_start; // reuse

      next_frame_start = prev_frame_start + new_frame_length + 8;

      //prev_frame_length = new_frame_length;

      if (frame_num < 5 || frame_num % 500 == 0) {
        Serial.printf("Frame %4d, index len %9d, frame len %9d, index start %9d, frame start %9d\n", frame_num, index_frame_length, new_frame_length, index_frame_start + 236 , prev_frame_start);
      }

      if (frame_num < 5 || frame_cnt_out % 100 == 0) {
        Serial.printf("Frame %4d, index len %9d, frame len %9d, index start %9d, frame start %9d\n", frame_num, index_frame_length, new_frame_length, index_frame_start + 236 , prev_frame_start);
        Serial.printf("in %d, out %d\n", frame_num, frame_cnt_out);
      }

      //avifile.seek( index_frame_start + 244 , SeekSet); // already 248

      remnant = (4 - (index_frame_length & 0x00000003)) & 0x00000003;
      int index_frame_length_rem = index_frame_length + remnant;

      int left_to_write = index_frame_length_rem;

      // print_dc_quartet( index_frame_length_rem, outfile);
      /*
            while (left_to_write > 0) {
              if (left_to_write > fbs * 1024) {
                size_t err = avifile.read(fb_faf_static, fbs * 1024);
                size_t err2 = outfile.write(fb_faf_static, fbs * 1024);
                left_to_write = left_to_write - fbs * 1024;
              } else {
                size_t err = avifile.read(fb_faf_static, left_to_write);
                size_t err2 = outfile.write(fb_faf_static, left_to_write);
                left_to_write = 0;
              }
            }
      */
      movi_size += index_frame_length;
      movi_size += remnant;

      print_2quartet(idx_offset, index_frame_length, idxfile);

      idx_offset = idx_offset + index_frame_length_rem + 8;

      frame_cnt_out++;
      frame_num++;

    }  // every frame in file

    //frame_cnt_out--;
    //movi_size = movi_size - remnant;
    //movi_size = movi_size - index_frame_length;

    //Serial.printf("frame %4d, outfile %9d, avifile %9d, idxfile %9d\n", frame_cnt , outfile.position(), avifile.position(), avifile.position());



    idxfile.close();
    //avifile.close();
    //avifile = SD_MMC.open(avi_file_name, "w");

    avifile.seek(  start_index_here  , SeekSet);

    //size_t i1_err = outfile.write(idx1_buf, 4);
    //if (!i1_err) Serial.printf("idx write\n");

    //print_quartet(frame_cnt_out * 16, outfile);

    idxfile = SD_MMC.open(idx_file_name, "r");
    if (idxfile)  {
      Serial.printf("File open: %s\n", idx_file_name);
    }  else  {
      Serial.printf("Could not open file %s\n", idx_file_name);
    }

    char * AteBytes;
    AteBytes = (char*) malloc (8);


    for (int i = 0; i < frame_cnt_out; i++) {
      size_t res = idxfile.readBytes( AteBytes, 8);
      if (!res) Serial.printf("idx read\n");
      size_t i1_err = avifile.write(dc_and_zero_buf, 8);
      if (!i1_err) Serial.printf("dc write\n");
      //size_t i2_err = outfile.write(zero_buf, 4);
      //if (!i2_err) Serial.printf("zero write\n");
      size_t i3_err = avifile.write((uint8_t *)AteBytes, 8);
      if (!i3_err) Serial.printf("ate write\n");
    }

    free(AteBytes);

    avifile.seek( 4 , SeekSet);         //shit
    print_quartet(movi_size + 240 + 16 * frame_cnt_out + 8 * frame_cnt_out, avifile);

    //avifile.seek( 0xe8 , SeekSet);
    //int lom = read_quartet( avifile);
    //Serial.printf("Len of movi was %d, now is %d\n", lom, movi_size);

    avifile.seek( 0xe8 , SeekSet);
    // shit print_quartet (movi_size, outfile);
    print_quartet(movi_size + frame_cnt_out * 8 + 4, avifile);

    //avifile.seek( 0x30 , SeekSet);
    //int fc = read_quartet( avifile);
    //Serial.printf("Frames was %d, now is %d\n", fc, frame_cnt_out);

    avifile.seek( 0x30 , SeekSet);
    print_quartet (frame_cnt_out, avifile);
    avifile.seek( 0x8c , SeekSet);
    print_quartet (frame_cnt_out, avifile);

    //avifile.seek( 0x84 , SeekSet);
    //int fps = read_quartet( avifile);
    //Serial.printf("fps was %d\n", fps);

    //float fnewfps = ( 1.0f * fps ) / (1 + skip_frames)   ;
    //int newfps = round(fnewfps);

    //Serial.printf("newfps is %f, %d\n", fnewfps, newfps);
    //avifile.seek( 0x84 , SeekSet);
    //print_quartet(newfps, avifile);

    //avifile.seek( 0x20 , SeekSet);
    //int us_per_frame = read_quartet( avifile);
    //Serial.printf("us_per_frame was %d\n", us_per_frame);

    //float newus = 1000000.0f / fnewfps;
    //uint32_t new_us_per_frame = round (newus);


    //Serial.printf("new_us_per_frame is %f, %d\n", newus, new_us_per_frame);
    //avifile.seek( 0x20 , SeekSet);
    //print_quartet(new_us_per_frame, avifile);


    idxfile.close();
    avifile.close();
    //outfile.close();

    int xx = SD_MMC.remove(idx_file_name);
  }
}

/*static esp_err_t delete_handler(httpd_req_t *req) {
  esp_err_t res = ESP_OK;

  Serial.print("delete_handler, core ");  Serial.print(xPortGetCoreID());
  Serial.print(", priority = "); Serial.println(uxTaskPriorityGet(NULL));


  httpd_resp_send(req, page_html, strlen(page_html));
  delay(100);
  delete_all_files = 1;
  return res;;
  }
*/


void the_camera_loop (void* pvParameter);
void the_sd_loop (void* pvParameter);
void delete_old_stuff();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() 
{
  Serial.begin(115200);
  Serial.println("\n\n---");
  Serial.println("Arduino IDE 2.3.7 - Espressif ESP32 3.3.5");

  pinMode(33, OUTPUT);              // little red led on back of chip
  digitalWrite(33, LOW);            // turn on the red LED on the back of chip

  pinMode(4, OUTPUT);               // Blinding Disk-Avtive Light
  digitalWrite(4, LOW);             // turn off

  pinMode(12, INPUT_PULLUP);        // pull this down to stop recording
  pinMode(13, INPUT_PULLUP);        // pull this down switch wifi

  // Инициализируем SD-карту
  if (init_sdcard()) logfile = SD_MMC.open("/boot.txt", FILE_WRITE);
  // Если неудача, то перезагружаем контроллер
  else major_fail();
  
  jprln(" ");
  jprln("---------------------------------------");
  jprln("ESP32-CamRecorder %s", vernum);
  jprln("---------------------------------------");

  // Показываем состояние памяти 
  print_mem("MEM - В начале SETUP                           ");
  // Определяем и показываем причину последнего сброса (reset reason). 
  esp_reset_reason_t reason = esp_reset_reason();
  jpr("Причина перезагрузки: ");
  switch (reason) 
  {
    case ESP_RST_UNKNOWN : jprln("ESP_RST_UNKNOWN");  break;
    case ESP_RST_POWERON : jprln("ESP_RST_POWERON"); break;
    case ESP_RST_EXT : jprln("ESP_RST_EXT");  break;
    case ESP_RST_SW : jprln("ESP_RST_SW");  break;
    case ESP_RST_PANIC : jprln("ESP_RST_PANIC");  break;
    case ESP_RST_INT_WDT : jprln("ESP_RST_INT_WDT");  break;
    case ESP_RST_TASK_WDT : jprln("ESP_RST_TASK_WDT");  break;
    case ESP_RST_WDT : jprln("ESP_RST_WDT");  break;
    case ESP_RST_DEEPSLEEP : jprln("ESP_RST_DEEPSLEEP");  break;
    case ESP_RST_BROWNOUT : jprln("ESP_RST_BROWNOUT");  break;
    case ESP_RST_SDIO : jprln("ESP_RST_SDIO");  break;
    default  : jprln("Reset resaon"); break;
  }
  // Запускаем продолжение нумерации файлов avi 
  // (или инициируем новую нумерацию)
  do_eprom_read();

  jprln("Выбираются параметры из config2.txt ...");
  read_config_file();

  jprln("Устанавливаются параметры камеры ...");
  config_camera();

  // Выделяем память под рабочие буферы для хранения jpg в движении 
  // (должны быть больше больших кадров с ov5640),
  // размер устанавливаем от ранее сформированного и расчитанного config_camera
  // fb_record = (uint8_t*)ps_malloc(512 * 1024); 
  fb_record =          (uint8_t*)ps_malloc(frame_buffer_size); 
  fb_curr_record_buf = (uint8_t*)ps_malloc(frame_buffer_size);
  fb_streaming =       (uint8_t*)ps_malloc(frame_buffer_size); 
  fb_capture =         (uint8_t*)ps_malloc(frame_buffer_size); 
  // Показываем состояние памяти 
  print_mem("MEM - после выделения памяти для jpg в движении");

  // Объявляем мьютекс между задачами
  baton = xSemaphoreCreateMutex();

  jprln("Создаётся задача the_camera_loop на 0 ядре");
  xTaskCreatePinnedToCore(
    the_camera_loop,       // TaskFunction_t pvTaskCode          - имя функции, которая содержит код
    "the_camera_loop",     // const char * const pcName          - имя задачи
    5000,                  // const uint32_t usStackDepth        - количество байт, выделенное для стека задачи
    NULL,                  // void * const pvParameters          - указатель на параметры для задачи
    4,                     // UBaseType_t uxPriority             - приоритет задачи
    &the_camera_loop_task, // TaskHandle_t * const pxCreatedTask - указатель на задачу, который можно использовать для ссылки на задачу позже (например, для её завершения)
    0                      // const BaseType_t xCoreID           - ядро процессора, на которое нужно назначить задачу (0 для ядра 0, 1 для 1 или tskNO_AFFINITY - на обоих ядрах
  ); //soc14
  delay(100);

  jprln("Создаётся задача the_streaming_loop");
  xTaskCreate(
    the_streaming_loop,    // TaskFunction_t pvTaskCode          - имя функции, которая содержит код
    "the_streaming_loop",  // const char * const pcName          - имя задачи
    8000,                  // const uint32_t usStackDepth        - количество байт, выделенное для стека задачи
    NULL,                  // void * const pvParameters          - указатель на параметры для задачи 
    2,                     // UBaseType_t uxPriority             - приоритет задачи
    &the_streaming_loop_task
  );
  if (the_streaming_loop_task == NULL ) 
  {
    //vTaskDelete( xHandle );
    Serial.printf("do_the_steaming_task failed to start! %d\n", the_streaming_loop_task);
  }
  
  if (InternetOff) 
  {
    print_mem("MEM - перед подключением WiFi                  ");
    // Подключаем локальные WiFi и создаём одну свою от контроллера
    init_wifi();
    print_mem("MEM - перед запуском Filemanager               ");
    filemgr.begin();
    filemgr.setBackGroundColor("Gray");
    jpr("Filemanager в своей сети     - http://");
    Serial.print(WiFi.softAPIP()); logfile.print(WiFi.softAPIP());
    jprln(":%d/", filemanagerport);
    jpr("Filemanager в локальной сети - http://");
    Serial.print(WiFi.localIP()); logfile.print(WiFi.localIP());
    jprln(":%d/", filemanagerport);

    print_mem("MEM - перед запуском Web-сервисов              ");
    startCameraServer();
    start_Stream_81_server();
    start_Stream_82_server();

    InternetOff = false;
    print_mem("МЕМ - после запуска WiFi                       ");
  }

  jprln("Checking SD for available space ...");
  delete_old_stuff();

  char logname[60];
  char the_directory[50];

  sprintf(the_directory, "/%s%03d",  devname, file_group);
  SD_MMC.mkdir(the_directory);

  sprintf(logname, "/%s%03d/%s%03d.999.txt",  devname, file_group, devname, file_group);
  jprln("Creating logfile %s\n",  logname);
  if (logfile) {
    logfile.close();
  }
  logfile = SD_MMC.open(logname, FILE_WRITE);
  if (!logfile) {
    Serial.println("Failed to open logfile for writing");
  }

  boot_time = millis();

  const char *strdate = ctime(&now);
  //logfile.println(strdate);

  digitalWrite(33, HIGH);         // red light turns off when setup is complete

  print_mem("End of setup");
  jpr("\n---  End of setup()  ---\n\n");

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// the_sd_loop()
//
/* //8
  void the_sd_loop (void* pvParameter) {

  Serial.print("the_sd_loop, core ");  Serial.print(xPortGetCoreID());
  Serial.print(", priority = "); Serial.println(uxTaskPriorityGet(NULL));

  while (1) {
    xSemaphoreTake( sd_go, portMAX_DELAY );            // we wait for camera loop to tell us to go
    another_save_avi( fb_curr);                        // do the actual sd wrte
    xSemaphoreGive( wait_for_sd );                     // tell camera loop we are done
  }
  }
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// the_camera_loop()
int delete_old_stuff_flag = 0;

void the_camera_loop (void* pvParameter) 
{
  print_mem("MEM - стартовала задача the_camera_loop        ");

  frame_cnt = 0;
  start_record_2nd_opinion = digitalRead(12);
  start_record_1st_opinion = digitalRead(12);
  start_record = 0;

  delay(1000);

  while (1) 
  {
    delay(1);

    // if (frame_cnt == 0 && start_record == 0)  // do nothing
    // if (frame_cnt == 0 && start_record == 1)  // start a movie
    // if (frame_cnt > 0 && start_record == 0)   // stop the movie
    // if (frame_cnt > 0 && start_record != 0)   // another frame

    ///////////////////  NOTHING TO DO //////////////////
    if ( (frame_cnt == 0 && start_record == 0)) {

      // Serial.println("Do nothing");
      // !!! 2026-01-27 здесь, наверное, ip-адрес не равен http://192.168.1.100/start
      if (we_are_already_stopped == 0) jpr("\n\nDisconnect Pin 12 from GND to start recording or http://192.168.1.100/start \n\n");
      we_are_already_stopped = 1;
      delay(100);

      ///////////////////  START A MOVIE  //////////////////
    } else if (frame_cnt == 0 && start_record == 1) {

      //Serial.println("Ready to start");

      we_are_already_stopped = 0;

      avi_start_time = millis();

      jpr("\nStart the avi ... at %d\n", avi_start_time);
      jpr("Framesize %d, quality %d, length %d seconds\n\n", framesize, quality, avi_length);
      logfile.flush();

      //88 frame_cnt++;

      long wait_for_cam_start = millis();
      wait_for_cam += millis() - wait_for_cam_start;

      start_avi();

      wait_for_cam_start = millis();

      ///
      frame_cnt++;

      long delay_wait_for_sd_start = millis();

      delay_wait_for_sd += millis() - delay_wait_for_sd_start;

      fb_curr = get_good_jpeg();    //7

      fb_curr_record_len = fb_curr->len;
      memcpy(fb_curr_record_buf, fb_curr->buf, fb_curr->len);
      fb_curr_record_time = millis();

      xSemaphoreTake( baton, portMAX_DELAY );

      fb_record_len = fb_curr_record_len;
      memcpy(fb_record, fb_curr_record_buf, fb_curr_record_len);   // v59.5
      fb_record_time = fb_curr_record_time;
      xSemaphoreGive( baton );

      esp_camera_fb_return(fb_curr);  //7

      another_save_avi( fb_curr_record_buf, fb_curr_record_len );

      ///
      wait_for_cam += millis() - wait_for_cam_start;
      if (blinking) digitalWrite(33, frame_cnt % 2);                // blink

      ///////////////////  END THE MOVIE //////////////////
    } else if ( restart_now || reboot_now || (frame_cnt > 0 && start_record == 0) ||  millis() > (avi_start_time + avi_length * 1000)) { // end the avi

      jpr("End the Avi");
      restart_now = false;

      if (blinking)  digitalWrite(33, frame_cnt % 2);

      end_avi();                                // end the movie

      if (blinking) digitalWrite(33, HIGH);          // light off

      delete_old_stuff_flag = 1;
      delay(50);

      avi_end_time = millis();

      float fps = 1.0 * frame_cnt / ((avi_end_time - avi_start_time) / 1000) ;

      jpr("End the avi at %d.  It was %d frames, %d ms at %.2f fps...\n", millis(), frame_cnt, avi_end_time, avi_end_time - avi_start_time, fps);

      if (!reboot_now) frame_cnt = 0;             // start recording again on the next loop

      ///////////////////  ANOTHER FRAME  //////////////////
    } else if (frame_cnt > 0 && start_record != 0) {  // another frame of the avi

      //Serial.println("Another frame");

      current_frame_time = millis();
      if (current_frame_time - last_frame_time < frame_interval) {
        delay(frame_interval - (current_frame_time - last_frame_time));             // delay for timelapse
      }
      last_frame_time = millis();

      frame_cnt++;

      long delay_wait_for_sd_start = millis();
      delay_wait_for_sd += millis() - delay_wait_for_sd_start;

      fb_curr = get_good_jpeg();    //7

      fb_curr_record_len = fb_curr->len;
      memcpy(fb_curr_record_buf, fb_curr->buf, fb_curr->len);
      fb_curr_record_time = millis();

      xSemaphoreTake( baton, portMAX_DELAY );

      fb_record_len = fb_curr_record_len;
      memcpy(fb_record, fb_curr_record_buf, fb_curr_record_len);   // v59.5
      fb_record_time = fb_curr_record_time;
      xSemaphoreGive( baton );

      esp_camera_fb_return(fb_curr);  //7

      another_save_avi( fb_curr_record_buf, fb_curr_record_len );

      long wait_for_cam_start = millis();

      wait_for_cam += millis() - wait_for_cam_start;

      if (blinking) digitalWrite(33, frame_cnt % 2);

      if (frame_cnt % 100 == 10 ) {     // print some status every 100 frames
        if (frame_cnt == 10) {
          bytes_before_last_100_frames = movi_size;
          time_before_last_100_frames = millis();
          most_recent_fps = 0;
          most_recent_avg_framesize = 0;
        } else {

          most_recent_fps = 100.0 / ((millis() - time_before_last_100_frames) / 1000.0) ;
          most_recent_avg_framesize = (movi_size - bytes_before_last_100_frames) / 100;

          if ( (Lots_of_Stats && frame_cnt < 1011) || (Lots_of_Stats && frame_cnt % 1000 == 10)) {
            jpr("So far: %04d frames, in %6.1f seconds, for last 100 frames: avg frame size %6.1f kb, %.2f fps ...\n", frame_cnt, 0.001 * (millis() - avi_start_time), 1.0 / 1024  * most_recent_avg_framesize, most_recent_fps);
          }

          total_delay = 0;

          bytes_before_last_100_frames = movi_size;
          time_before_last_100_frames = millis();
        }
      }
    }
  }
}

//


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// loop() - loop runs at low prio, so I had to move it to the task the_camera_loop at higher priority
#include <ESPping.h>
long wakeup;
long last_wakeup = 0;
int loops = 0;
void loop() {
  long run_time = millis() - boot_time;
  loops++;
  if (loops % 10000 == 17) {
    //Serial.printf("loops %10d\n",loops);
  }

  for (int x = 0; x < 1; x++) {
    filemgr.handleClient();  //soc.6
  }

  if (do_the_ota) {
    ArduinoOTA.handle();
  }

  if (delete_old_stuff_flag == 1) {
    delete_old_stuff_flag = 0;
    delete_old_stuff();
  }
  start_record_2nd_opinion = start_record_1st_opinion;
  start_record_1st_opinion = digitalRead(12);

  if (do_the_reindex) {
    done_the_reindex = false;
    do_the_reindex = false;
    re_index ( file_to_read, file_to_write );
    //re_index_bad ( file_to_read );
    done_the_reindex = true;
  }

  wakeup = millis();
  if (wakeup - last_wakeup > (10  * 60 * 1000) ) {
    last_wakeup = millis();
    print_mem("---------- 10 Minute Internet Check -----------\n");
    time(&now);
    jpr("Local time: "); jpr(ctime(&now));
    if (!InternetOff ) 
    {
      esp_err_t client_err;
      struct sockaddr_in *client_list;
      size_t clients = 10;
      size_t client_count = 10;
      int    client_fds[10];

      client_err = httpd_get_client_list(camera_httpd, &client_count, client_fds);
      jpr("camera_httpd Sockets , Num = %d\n", client_count);
      for (size_t i = 0; i < client_count; i++) {
        int sock = client_fds[i];
        int x = httpd_ws_get_fd_info(camera_httpd, sock) ;
        jpr("Socket %d, fd=%d, info=%d \n", i, sock, x);
        print_sock(sock);
      }

      client_err = httpd_get_client_list(stream81_httpd, &client_count, client_fds);
      jpr("stream81_httpd Sockets , Num = %d\n", client_count);
      for (size_t i = 0; i < client_count; i++) {
        int sock = client_fds[i];
        //Serial.printf("%d, sock %d\n", i, sock);
        int x = httpd_ws_get_fd_info(camera_httpd, sock) ;
        jpr("Socket %d, fd=%d, info=%d \n", i, sock, x);
        print_sock(sock);
      }
      client_err = httpd_get_client_list(stream82_httpd, &client_count, client_fds);
      jpr("stream82_httpd Sockets , Num = %d\n", client_count);
      for (size_t i = 0; i < client_count; i++) {
        int sock = client_fds[i];
        //Serial.printf("%d, sock %d\n", i, sock);
        int x = httpd_ws_get_fd_info(camera_httpd, sock) ;
        jpr("Socket %d, fd=%d, info=%d \n", i, sock, x);
        print_sock(sock);
      }

      if (found_router) {
        // Ping local IP
        Serial.println(WiFi.gatewayIP());
        if (Ping.ping(WiFi.gatewayIP()) > 0) {
          jpr(" -- response time : %d/%.2f/%d ms\n", Ping.minTime(), Ping.averageTime(), Ping.maxTime());
        } else {

          jprln("\n\nCannot Ping the gateway - REBOOT");
          jprln("***** WiFi reconnect *****");
          WiFi.reconnect();
          delay(8000);
          if (WiFi.status() != WL_CONNECTED) {
            // Подключаем локальные WiFi и создаём одну свою от контроллера
            jprln("***** WiFi restart *****");
            init_wifi();
          }
          delay(15000);
          if (WiFi.status() != WL_CONNECTED) {
            jprln("***** Reboot *****");
            reboot_now = true;
          }

        }
        delay(1000);

        // Ping Host
        const char* remote_host = "google.com";
        jpr(remote_host);
        if (Ping.ping(remote_host) > 0) {
          jpr(" -- response time : %d/%.2f/%d ms\n", Ping.minTime(), Ping.averageTime(), Ping.maxTime());
        } else {
          jprln(" Ping Error !");
        }
        delay(1000);


        if (WiFi.status() != WL_CONNECTED) 
        {
          jprln("***** WiFi reconnect *****");
          WiFi.reconnect();
          delay(8000);

          if (WiFi.status() != WL_CONNECTED) 
          {
            // Подключаем локальные WiFi и создаём одну свою от контроллера
            jprln("***** WiFi restart *****");
            init_wifi();
          }
        }
      }

      Serial.print(_soft_IP); Serial.println(WiFi.softAPIP());  
      logfile.println(_soft_IP+WiFi.softAPIP());
      Serial.print(_localIP); Serial.println(WiFi.localIP());   
      logfile.println(_localIP+WiFi.localIP());

      if (!MDNS.begin(devname)) {
        jprln("Error setting up MDNS responder!");
      } else {
        jpr("mDNS responder started '%s'\n", devname);
      }
    }  // not internet off
  }  // wakeup

  if (reboot_now == true) {
    jprln(" \n\n\n Rebooting in 5 seconds... \n\n\n");
    delay(5000);
    ESP.restart();
  }

  if (web_stop == true) {
    if (start_record == 1) {
      start_record = 0;
      jprln("web_stop web_stop code");
    }
  } else {
    //jpr("first %d, second %d, web %d\n", start_record_1st_opinion, start_record_2nd_opinion, web_stop);
    if (start_record == 1) {
      if (start_record_1st_opinion == 0 && start_record_2nd_opinion == 0) {
        start_record = 0;
        jprln("stopping in web_stop code");
      }
    } else {
      if (start_record_1st_opinion == 1 && start_record_2nd_opinion == 1) {
        start_record = 1;
        jprln("starting in web_stop code");
      }
    }
  }
}

// ************************************************** ESP32-CamRecorder.ino ***
