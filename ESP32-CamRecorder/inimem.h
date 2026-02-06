/** Arduino, ESP32, C/C++ **************************************** inimem.h ***
 * 
 *                      Объявить/проинициализировать общепрограммные переменные
 *                                                     
 * v1.0.4, 06.02.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 24.01.2026
**/

#pragma once     

#include "time.h"
#include "esp_log.h"
#include "FS.h"
#include <SD_MMC.h>

// Объявляем дескрипторы файлов
File logfile;  // файл информационных сообщений и сообщений об ошибках
File avifile;  // файл потока графических изображений (кадров)
File idxfile;  // файл указателей кадров

static const char _hsoftIP[] ="IP-адрес своей сети контроллера - http://";
static const char _hlocalIP[]="IP-адрес в локальной сети       - http://";
static const char vernum[]="v62.34.0"; // версия приложения
const word filemanagerport=8080;       // порт файлового менеджера
char localip[20];                      // IP-адрес локальной сети
char softip[20];                       // IP-адрес собственной сети контроллера
bool found_router = false;             // true - определена локальная сеть

time_t now;
struct tm timeinfo;

TaskHandle_t the_camera_loop_task;
TaskHandle_t the_streaming_loop_task;
SemaphoreHandle_t baton;

long current_frame_time;
long last_frame_time;

bool restart_now = false;   // true - начать запись нового avi-видео
bool reboot_now = false;    // true - завершить запись и перезагрузить контроллер
bool web_stop = false;      // true - завершить запись для OTA или по команде Stop из браузера

#define Lots_of_Stats true  // true - выводить статистику состояний
#define blinking 0

char devname[30];           // название камеры
int framesize;              // размер кадра
int quality;                // качество изображения
int avi_length ;            // сколько длится фильм в секундах -- 1800 sec = 30 min
int frame_interval ;        // интервал между записями кадров в миллисекундах (ms) 
int speed_up_factor ;       // ускорение воспроизведения (1 - в режиме реального времени)
int stream_delay ;          // задержка между кадрами не менее 500 мс (интервал между потоковыми кадрами - ms)
bool configfile = false;    // true - считан файл config2.txt

// Московская timezone в соответствии с:
// https://www.gnu.org/software/libc/manual/html_node/Proleptic-TZ.html
// String TIMEZONE = "GMT0BST,M3.5.0/01,M10.5.0/02";
String TIMEZONE = "MSK-3";

String cssid1, cssid2, cssid3;
String cpass1, cpass2, cpass3;

int framesizeconfig;        //
int qualityconfig;          //
int buffersconfig;          // количество отдельных буферов для кадров

// Буфер для 4 кадров, в соответствии с [config.h].cbuffersconfig = 4,
// первоначально сформированный и загруженный при инициировании камеры
int frame_buffer_size; 

bool do_the_reindex = false;
//bool done_the_reparse = false;
bool done_the_reindex = false;

char file_to_edit[50] = "/JamCam0481.0007.avi"; //61.3

#define BUFFSIZE 512
uint8_t buf[BUFFSIZE];

long avi_start_time = 0;   // время начала видео-записи
long avi_end_time = 0;
char avi_file_name[100];   // название записываемого файла *.avi
uint16_t frame_cnt = 0;    // общее количество кадров в файле

// *************************************************************** inimem.h ***
