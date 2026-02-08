/** Arduino, ESP32, C/C++ *************************** ESP32-CamRecorder.ino ***
 * 
 *     Видеомагнитофон на Esp32-Cam - переделка ESP32-CAM-Video-Recorder-junior
 *                (https://github.com/jameszah/ESP32-CAM-Video-Recorder-junior) 
 *                                                     для умного хозяйства tve
 *                                                     
 * v1.0.7, 06.02.2026                                 Автор:      Труфанов В.Е.
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

#include <stdio.h>
#include "time.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "soc/soc.h"
#include "esp_cpu.h" // #include "soc/cpu.h"
#include "soc/rtc_cntl_reg.h"

static esp_err_t cam_err;

#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "FS.h"
#include <SD_MMC.h>

#include "lwip/sockets.h"
#include <lwip/netdb.h>

#include "WiFi.h"
#include <WiFiMulti.h>
WiFiMulti jMulti;
#include <ArduinoOTA.h>
#include "ESPmDNS.h"

WiFiEventId_t eventID;      
#include "esp_wifi.h"  

#include "inimem.h"
#include "jpr.h"
#include "sd.h"
#include "eprom.h"
#include "config.h"
#include "camera.h"
#include "stream32.h"
#include "CameraServer.h"

#include "ESPxWebFlMgr.h"              // v56
ESPxWebFlMgr filemgr(filemanagerport); // we want a different port than the webserver

// ****************************************************************************
// *       Подключить локальные WiFi и создать одну свою от контроллера       *
// ****************************************************************************
bool init_wifi() 
{
  // Выбираем и показываем версию ESP-IDF
  String idfver = esp_get_idf_version();
  Serial.println("Версия компилятора ESP:  "+idfver);
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
  const char _soft_IP[] = "IP контроллера: ";
  jprln("Контроллер устанавливает собственную точку доступа");
  WiFi.softAP(ssidch3, passch3);
  sprintf(softip, "%s", WiFi.softAPIP().toString().c_str());
  Serial.print(_soft_IP); Serial.println(softip); 
  
  const char _localIP[] = "  Локальный IP: ";
  jprln("Контроллер подключается к локальной точке доступа");
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
  Serial.print(_localIP); Serial.println(localip); 
  
  Serial.println(" ");
  jprln("Определяется местное время");
  // configTime(0, 0, "pool.ntp.org");
  configTime(10800, 0, "ntp.msk-ix.ru");
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
  Serial.print("Местное время: "); Serial.print(ctime(&now));

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

void the_camera_loop (void* pvParameter);
void the_sd_loop (void* pvParameter);
void delete_old_stuff();

bool InternetOff = true;    // true - WiFi не подключен

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
  // (должны быть больше больших кадров с ov2640,ov5640),
  // размер устанавливаем от ранее сформированного и расчитанного config_camera
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
    Serial.printf("Не удалось запустить задачу do_the_steaming_task! %d\n", the_streaming_loop_task);
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

  jprln("Проверяется SD-карта на наличие свободного места ...");
  delete_old_stuff();

  char logname[60];
  char the_directory[50];

  sprintf(the_directory, "/%s%03d",  devname, file_group);
  SD_MMC.mkdir(the_directory);

  sprintf(logname, "/%s%03d/%s%03d.999.txt",  devname, file_group, devname, file_group);
  jprln("Создается logfile %s\n", logname);
  if (logfile) logfile.close();
  logfile = SD_MMC.open(logname, FILE_WRITE);
  if (!logfile) 
  {
    Serial.println("Ошибка открытия logfile для записи");
  }
  const char *strdate = ctime(&now);
  //logfile.println(strdate);
  digitalWrite(33, HIGH);         // red light turns off when setup is complete
  print_mem("МЕМ - после завершения setup                   ");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// loop()             - основной (фоновый) цикл выполняется с 0 - низким приоритетом;
// the_camera_loop()  - цикл фотографирования и записи avi-имеет наибольший приоритет = 4;
// the_streaming_loop - видео-поток имеет приоритет 2
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <ESPping.h>
// Определяем переменную "времени текущего пробуждения"
long wakeup;
// Инициируем переменную "времени прошлого пробуждения"
long last_wakeup = 0;
// Инициируем начальный номер фонового цикла 
int loops = 0;   

void loop() 
{
  loops++;
  if (loops % 10000 == 17) /*Serial.printf("looooooooooooooooooooooooooooops %10d\n",loops)*/;
  //
  for (int x = 0; x < 1; x++) 
  {
    filemgr.handleClient();  //soc.6
  }
  //
  if (do_the_ota) 
  {
    ArduinoOTA.handle();
  }
  // Если установлен флаг удаления старых файлов,
  // то удаляем старые файлы и сбрасываем флаг
  if (delete_old_stuff_flag == 1) 
  {
    delete_old_stuff();
    delete_old_stuff_flag = 0;
  }
  
  start_record_2nd_opinion = start_record_1st_opinion;
  start_record_1st_opinion = digitalRead(12);

  if (do_the_reindex) 
  {
    done_the_reindex = false;
    do_the_reindex = false;
    re_index ( file_to_read, file_to_write );
    //re_index_bad ( file_to_read );
    done_the_reindex = true;
  }

  // Если прошло 10 минут, то выполняем контроль интернета
  wakeup = millis();
  if (wakeup - last_wakeup > (10  * 60 * 1000) ) 
  {
    last_wakeup = millis();
    Serial.println(" "); 
    print_mem("---------- 10 Minute Internet Check -----------");
    time(&now);
    jpr("Текущее время: "); jpr(ctime(&now));
    if (!InternetOff ) 
    {
      // Выводим информацию по сокетам
      esp_err_t client_err;
      //struct sockaddr_in *client_list;
      size_t clients = 10;
      size_t client_count = 10;
      int    client_fds[10];
      client_err = httpd_get_client_list(camera_httpd, &client_count, client_fds);
      jpr("camera_httpd Sockets , Num = %d\n", client_count);
      for (size_t i = 0; i < client_count; i++) 
      {
        int sock = client_fds[i];
        int x = httpd_ws_get_fd_info(camera_httpd, sock) ;
        jpr("Socket %d, fd=%d, info=%d \n", i, sock, x);
        print_sock(sock);
      }
      client_err = httpd_get_client_list(stream81_httpd, &client_count, client_fds);
      jpr("stream81_httpd Sockets , Num = %d\n", client_count);
      for (size_t i = 0; i < client_count; i++) 
      {
        int sock = client_fds[i];
        //Serial.printf("%d, sock %d\n", i, sock);
        int x = httpd_ws_get_fd_info(camera_httpd, sock) ;
        jpr("Socket %d, fd=%d, info=%d \n", i, sock, x);
        print_sock(sock);
      }
      client_err = httpd_get_client_list(stream82_httpd, &client_count, client_fds);
      jpr("stream82_httpd Sockets , Num = %d\n", client_count);
      for (size_t i = 0; i < client_count; i++) 
      {
        int sock = client_fds[i];
        //Serial.printf("%d, sock %d\n", i, sock);
        int x = httpd_ws_get_fd_info(camera_httpd, sock) ;
        jpr("Socket %d, fd=%d, info=%d \n", i, sock, x);
        print_sock(sock);
      }
      //
      if (found_router) 
      {
        // Получаем IP-адрес шлюза (роутера) текущей подключённой сети Wi-Fi и
        // пингуем её. Функция возвращает IP-адрес шлюза подключённой сети Wi-Fi. 
        // Если модуль не подключён к сети, функция вернёт 0.0.0.0. 
        Serial.println("IP-адрес шлюза (роутера): "); Serial.println(WiFi.gatewayIP());
        if (Ping.ping(WiFi.gatewayIP())>0) 
        {
          jpr("Время отклика: %d/%.2f/%d ms\n", Ping.minTime(), Ping.averageTime(), Ping.maxTime());
        } 
        else 
        {
          jprln("Пинг роутера не прошел, отключается WiFi");
          WiFi.reconnect();
          delay(8000);
          if (WiFi.status() != WL_CONNECTED) 
          {
            // Подключаем локальные WiFi и создаём одну свою от контроллера
            jprln("Подключается WiFi заново");
            init_wifi();
          }
          delay(15000);
          if (WiFi.status() != WL_CONNECTED) 
          {
            jprln("Нет поключения к WiFi - перезагрузка контроллера");
            reboot_now = true;
          }
        }
        delay(1000);

        if (WiFi.status() != WL_CONNECTED) 
        {
          jprln("Отключается WiFi");
          WiFi.reconnect();
          delay(8000);

          if (WiFi.status() != WL_CONNECTED) 
          {
            // Подключаем локальные WiFi и создаём одну свою от контроллера
            jprln("Подключается WiFi заново");
            init_wifi();
          }
        }
      }
      Serial.print(_hsoftIP);  Serial.println(WiFi.softAPIP()); 
      Serial.print(_hlocalIP); Serial.println(WiFi.localIP()); 

      logfile.println(WiFi.softAPIP());
      logfile.println(WiFi.localIP());

      if (!MDNS.begin(devname)) 
      {
        jprln("Ошибка установки MDNS responder!");
      } 
      else 
      {
        jprln("mDNS responder стартовал: '%s'", devname);
      }
    }  
  } 

  // Перезагружаем контроллер если установлен флаг "Перезагрузить контроллер" 
  if (reboot_now == true) 
  {
    jprln(" \n\n\n Перезагрузка контроллера в течение 5 секунд ... \n\n\n");
    delay(5000);
    ESP.restart();
  }
  // Реагируем на команду "Остановить запись avi-файла", поступившую из браузера
  if (web_stop == true) 
  {
    // Если запись велась, то сбрасываем флаг запуска записи очередного видео-файла
    if (start_record == 1) 
    {
      start_record = 0;
      jprln("Поступила команда 'Остановить запись avi-файла'");
    }
  } 
  // Если команды на остановку записи видео нет, то реагируем на "12"-ый контакт
  else 
  {
    if (start_record == 1) 
    {
      // Если запись велась, но обе проверки дали останов записи,
      // то сбрасываем флаг запуска записи очередного видео-файла
      if (start_record_1st_opinion == 0 && start_record_2nd_opinion == 0) 
      {
        start_record = 0;
        //jprln("'Остановить запись avi-файла' по событию на 12-том контакте");
      }
    } 
    else 
    {
      // Если запись НЕ велась, но обе проверки дали запуск записи,
      // то устанавливаем флаг запуска записи очередного видео-файла
      if (start_record_1st_opinion == 1 && start_record_2nd_opinion == 1) 
      {
        start_record = 1;
        //jprln("'Запустить запись avi-файла' по событию на 12-том контакте");
      }
    }
  }
}

// ************************************************** ESP32-CamRecorder.ino ***
