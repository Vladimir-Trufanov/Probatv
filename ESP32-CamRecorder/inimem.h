/** Arduino, ESP32, C/C++ **************************************** inimem.h ***
 * 
 *                      Объявить/проинициализировать общепрограммные переменные
 *                                                     
 * v1.0.0, 24.01.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 24.01.2026
**/

#pragma once     

#include "esp_log.h"
#include "FS.h"
#include <SD_MMC.h>

// Объявляем дескрипторы файлов
File logfile;  // файл информационных сообщений и сообщений об ошибках
File avifile;
File idxfile;

static const char vernum[] = "v62.34.0";   // версия приложения

#define Lots_of_Stats 1
#define blinking 0

char devname[30];           // название камеры
int framesize;              // размер кадра
int quality;                // качество изображения
int avi_length ;            // сколько длится фильм в секундах -- 1800 sec = 30 min
int frame_interval ;        // интервал между записями кадров в миллисекундах (ms) 
int speed_up_factor ;       // воспроизведение в режиме реального времени (частота кадров)
int stream_delay ;          // задержка между кадрами не менее 500 мс (интервал между потоковыми кадрами - ms)
bool configfile = false;    // true - считан файл config2.txt

// Московская timezone в соответствии с:
// https://www.gnu.org/software/libc/manual/html_node/Proleptic-TZ.html
// String TIMEZONE = "GMT0BST,M3.5.0/01,M10.5.0/02";
String TIMEZONE = "MSK+00";

String cssid1, cssid2, cssid3;
String cpass1, cpass2, cpass3;

int framesizeconfig;
int qualityconfig;
int buffersconfig;

// *************************************************************** inimem.h ***
