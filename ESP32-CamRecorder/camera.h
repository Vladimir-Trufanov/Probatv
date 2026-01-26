/** Arduino, ESP32, C/C++ **************************************** camera.h ***
 * 
 *                                             Обслужить работу с видео-камерой
 *                                                     
 * v1.0.1, 26.01.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 25.01.2026
 * 
**/

#pragma once   

#include "esp_camera.h"
#include "sensor.h"

#include "inimem.h"
#include "jpr.h"
#include "sd.h"

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

// ****************************************************************************
// *                      Установить параметры камеры                         *
// ****************************************************************************
static void config_camera() 
{
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

  jpr("Frame config %d, quality config %d, buffers config %d\n", framesizeconfig, qualityconfig, buffersconfig);

  config.frame_size =  (framesize_t)framesize;
  config.jpeg_quality = quality;
  config.fb_count = buffersconfig;  // количество отдельных буферов для кадров

  // Обеспечиваем размещение в буферах последних кадров
  // https://github.com/espressif/esp32-camera/issues/357#issuecomment-1047086477
  config.grab_mode = CAMERA_GRAB_LATEST; // 61.92
  // Показываем состояние памяти
  if (Lots_of_Stats) print_mem("MEM - перед инициированием камеры              ");
  // Задаём 5 попыток инициации камеры
  esp_err_t cam_err = ESP_FAIL;
  int attempt = 5;
  while (attempt && cam_err != ESP_OK) 
  {
    cam_err = esp_camera_init(&config);
    if (cam_err != ESP_OK) 
    {
      jpr("Ошибка инициировании камеры 0x%x\n", cam_err);
      // Передёргиваем контакт питания
      digitalWrite(PWDN_GPIO_NUM, 1);
      delay(500);
      digitalWrite(PWDN_GPIO_NUM, 0); // power cycle the camera (OV2640)
      // Уменьшаем счётчик
      attempt--;
    }
  }
  if (Lots_of_Stats) print_mem("MEM - после инициирования камеры               ");
  // Если неудачное инициирование камеры, то перезагружаем контроллер
  if (cam_err != ESP_OK) major_fail();
  // Получаем указатель (дескриптор) на структуру данных сенсора (камеры)
  sensor_t * ss = esp_camera_sensor_get();
  jprln("Камера стартовала корректно, id.PID: %x (hex) of 9650, 7725, 2640, 3660, 5640", ss->id.PID);
  // По id.PID достраиваем параметры
  if (ss->id.PID == OV5640_PID ) 
  {
    //Serial.println("56 - going mirror");
    ss->set_hmirror(ss, 1);        // 0 = disable , 1 = enable
  } 
  else 
  {
    ss->set_hmirror(ss, 0);        // 0 = disable , 1 = enable
  }
  ss->set_brightness(ss, 1);       // up the blightness just a bit
  ss->set_saturation(ss, -2);      // lower the saturation

  int x = 0;
  delay(500);
  for (int j = 0; j < 30; j++) 
  {
    // Получаем буфер кадра с камеры (esp_camera_fb_get)
    // (функция esp_camera_fb_get возвращает указатель на структуру camera_fb_t. 
    // В этой структуре хранятся, например: 
    //   указатель на пиксельные данные (поле buf);
    //   длина буфера в байтах (поле len);
    //   ширина буфера в пикселях (поле width);
    //   высота буфера в пикселях (поле height);
    //   формат структуры пиксельных данных (поле format);
    //   отметка времени (поле timestamp).
    // После использования буфера память, выделенную функцией esp_camera_fb_get(),
    // нужно освободить с помощью функции esp_camera_fb_return(). 
    // Пример использования.
    //
    // Функция esp_camera_fb_get используется, например, для получения одного снимка 
    // с камеры. В коде может быть вызов: camera_fb_t *fb = esp_camera_fb_get(). 
    // Если fb = null (захват камеры не удался), функция выводит сообщение об ошибке,
    // ждёт 1 секунду и затем перезагружает плату ESP32.
    //
    // Важно: функция может быть вызвана только один раз, деинициализировать 
    // модуль камеры с её помощью нельзя. 
    camera_fb_t * fb = esp_camera_fb_get(); // get_good_jpeg();
    if (!fb) Serial.println("Не удалось выполнить захват с камеры");
    else 
    {
      if (j < 3 || j > 27) jpr("Pic %2d, len=%7d, at mem %X\n", j, fb->len, (long)fb->buf);
      x = fb->len;
      esp_camera_fb_return(fb);
      delay(30);
    }
  }
  frame_buffer_size  = (( (x * 4) / (16 * 1024) ) + 1) * 16 * 1024  ;
  // 4 times buffer size, rounded up to 16kb
  jprln("Размер буфера изображений для %d равен %d", x, frame_buffer_size);
  print_mem("MEM - после завершения инициирования камеры    ");
}

// *************************************************************** camera.h ***
