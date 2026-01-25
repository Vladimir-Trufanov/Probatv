/* ========================================================================== */
/*                                                                            */
/*   рус    multiple_fb_count.ino */
/* Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */


#include <stdio.h>
// Time
#include "time.h"

#include "esp_camera.h"

int start;

// MicroSD
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include <SD_MMC.h>

struct tm timeinfo;
time_t now;
char strftime_buf[64];

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

static esp_err_t init_sdcard()
{

  esp_err_t ret = ESP_FAIL;
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  host.flags = SDMMC_HOST_FLAG_1BIT;                       // using 1 bit mode
  host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
  int diskspeed = host.max_freq_khz;
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  slot_config.width = 1;                                   // using 1 bit mode
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files = 8,
  };

  sdmmc_card_t *card;

  ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

  if (ret == ESP_OK) {
    Serial.println("SD card mount successfully!");
  }  else  {
    Serial.printf("Failed to mount SD card VFAT filesystem. Error: %s", esp_err_to_name(ret));
  }

  sdmmc_card_print_info(stdout, card);
  
  return ret;

}

static void save_photo_dated(int i, camera_fb_t *fb) {

  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%H.%M.%S", &timeinfo);
  char jfname[130];
  sprintf(jfname, "/sdcard/pic_%d_%s.jpg",  i, strftime_buf);

  Serial.printf("save %d @ %d, len %d, %s\n", i, millis() - start, fb->len, jfname);

  FILE *file = fopen(jfname, "w");
  if (file != NULL)  {
    size_t err = fwrite(fb->buf, 1, fb->len, file);
    Serial.printf("File saved: %s\n", jfname);
  }  else  {
    Serial.printf("Could not open file: %s\n\n ", jfname);
  }
  fclose(file);
}


static void config_camera() {

  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_HD;

  config.jpeg_quality = 6;
  config.fb_count = 4;

  // camera init
  int cam_err = esp_camera_init(&config);
  if (cam_err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", cam_err);
  }

  delay(100);
}



void setup() {

  Serial.begin(115200);
  Serial.println("\n\n---");

  config_camera();

  int card_err = init_sdcard();
  if (card_err != ESP_OK) {
    Serial.printf("SD Card init failed with error 0x%x", card_err);
  }

  delay(4000);

  camera_fb_t * fb;
  for (int j = 10; j < 20; j++) {
    fb = esp_camera_fb_get();
    save_photo_dated(j, fb);
    esp_camera_fb_return(fb);
  }

  delay (6000);

  esp_camera_fb_return(fb);
  esp_camera_fb_return(fb);
  esp_camera_fb_return(fb);
  esp_camera_fb_return(fb);
  
  for (int j = 20; j < 30; j++) {
    fb = esp_camera_fb_get();
    save_photo_dated(j, fb);
    esp_camera_fb_return(fb);
  }

  delay (10000);

  fb = esp_camera_fb_get(); esp_camera_fb_return(fb);
  fb = esp_camera_fb_get(); esp_camera_fb_return(fb);
  fb = esp_camera_fb_get(); esp_camera_fb_return(fb);
  fb = esp_camera_fb_get(); esp_camera_fb_return(fb);

  for (int j = 30; j < 40; j++) {
    fb = esp_camera_fb_get();
    save_photo_dated(j, fb);
    esp_camera_fb_return(fb);
  }


  delay(5000);
  
  Serial.printf("starting %d\n", millis());
  start = millis();
  
  delay(5000);
  
  camera_fb_t * fb1 = esp_camera_fb_get();
  Serial.printf("take 1 @ %d,  len %d\n", millis() - start, fb1->len);
  
  delay(5000);
  
  camera_fb_t * fb2 = esp_camera_fb_get();
  Serial.printf("take 2 @ %d, len %d\n", millis() - start, fb2->len);
  
  delay(5000);
  
  camera_fb_t * fb3 = esp_camera_fb_get();
  Serial.printf("take 3 @ %d, len %d\n", millis() - start, fb3->len);

  delay(2000);
  
  Serial.printf("save 1 @ %d, len %d\n", millis() - start, fb1->len);
  save_photo_dated(51, fb1);
  esp_camera_fb_return(fb1);

  delay(2000);
  
  Serial.printf("save 2 @ %d, len %d\n", millis() - start, fb2->len);
  save_photo_dated(52, fb2);
  esp_camera_fb_return(fb2);

  delay(2000);
  
  Serial.printf("save 3 @ %d, len %d\n", millis() - start, fb3->len);
  save_photo_dated(53, fb3);
  esp_camera_fb_return(fb3);

  delay(5000);
  
  camera_fb_t * fb4 = esp_camera_fb_get();
  Serial.printf("take 4 @ %d, len %d\n", millis(), fb4->len);
  Serial.printf("save 4 @ %d, len %d\n", millis() - start, fb4->len);
  save_photo_dated(54, fb4);
  esp_camera_fb_return(fb4);

  delay(5000);
  
  camera_fb_t * fb5 = esp_camera_fb_get();
  Serial.printf("take 5 @ %d, len %d\n", millis(), fb5->len);
  Serial.printf("save 5 @ %d, len %d\n", millis() - start, fb5->len);
  save_photo_dated(55, fb5);
  esp_camera_fb_return(fb5);

  delay(5000);
  
  camera_fb_t * fb6 = esp_camera_fb_get();
  Serial.printf("take 6 @ %d, len %d\n", millis(), fb6->len);
  Serial.printf("save 6 @ %d, len %d\n", millis() - start, fb6->len);
  save_photo_dated(56, fb6);
  esp_camera_fb_return(fb6);

  delay(7000);
  
  camera_fb_t * fb7 = esp_camera_fb_get();
  Serial.printf("take 7 @ %d, len %d\n", millis(), fb7->len);
  Serial.printf("save 7 @ %d, len %d\n", millis() - start, fb7->len);
  save_photo_dated(57, fb7);
  esp_camera_fb_return(fb7);
  
  Serial.println("done");
}

void loop() {
  
}