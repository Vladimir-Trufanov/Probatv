/** Arduino, ESP32, C/C++ ********************************** ESPxWebFlMgr.h ***
 * 
 *                                                   Файловый менеджер на Esp32
 * mods by James Zahary Dec 28, 2021   https://github.com/jameszah/ESPxWebFlMgr
 * based on                        https://github.com/holgerlembke/ESPxWebFlMgr
 * 
 *      Управление файлами с помощью простого веб-интерфейса. Поддержка Arduino 
 *           ESP8266 и Arduino ESP32. Локальное редактирование, переименование, 
 *                                                           загрузка и сжатие.
 * 
 *                                                   Модификация: Труфанов В.Е.
 * v3.0.0, 28.01.2026                                Дата начальная: 28.01.2026
 * 
**/

#ifndef ESPxWebFlMgr_h
#define ESPxWebFlMgr_h

/*
  Changes
    V1.03
     x удалены все SPIFF-файлы из версии ESP32, полностью переключен на Little FS
     x fixed rename+delete for ESP32+LittleFS (added "/")

    V1.02
     x исправлен способ выбора файловой системы с помощью условных определений

    V1.01
     + добавлен прогресс с именем файла при загрузке
     x fixed error in ZIP file structure (zip.bitflags needs a flag)

    V1.00
     + out of V0.9998...
     + ESP8266: LittleFS is default 
     + javascript: added "msgline();"
     + javascript: added "Loading..." как нерабочая подсказка, показывающая, что Javascript отключен
     + cleaning up the "/"-stuff (from SPIFF with leading "/" to LittleFS without...)
     + Warning: esp8266 2.7.4 has an error in mime::getContentType(path) for .TXT. Fix line 65 is { kTxtSuffix, kTxt },
     + review of "edit file", moved some stuff to ESPxWebFlMgrWpF.h
*/

#include <Arduino.h>
#include <inttypes.h>

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <FS.h>
  #include <LittleFS.h>
  #define ESPxWebFlMgr_FileSystem LittleFS
#endif

#ifdef ESP32
  #include <WiFi.h>
  #include <WebServer.h>
  #include <FS.h>
  #include <SD_MMC.h> //jz #include <LittleFS.h>
  #define ESPxWebFlMgr_FileSystem SD_MMC //jz #define ESPxWebFlMgr_FileSystem LittleFS
#endif

#ifndef ESPxWebFlMgr_FileSystem
#pragma message ("ESPxWebFlMgr_FileSystem not defined.")
#endif

// Следующее определение можно убрать, чтобы сэкономить около 10 кб места для кода,
// но для этого необходимо поместить файлы из "<library>/filemanager" в FS. 
#define fileManagerServerStaticsInternally
// Следующее определение позволяет показать кнопку редактирования для каждого 
// типа файла, даже двоичного и ему подобного
// #define fileManagerEditEverything

class ESPxWebFlMgr 
{
  private:
    word _Port;
    #ifdef ESP8266
      ESP8266WebServer * fileManager = NULL;
    #endif
    #ifdef ESP32
      WebServer * fileManager = NULL;
    #endif
    bool _ViewSysFiles = false;
    String _SysFileStartPattern = "/.";
    File fsUploadFile;
    String _backgroundColor = "black";

    void fileManagerNotFound(void);
    String dispIntDotted(size_t i); 
    String dispFileString(size_t fs); 
    String CheckFileNameLengthLimit(String fn);

    // веб-страница
    void fileManagerIndexpage(void);
    void fileManagerJS(void);
    void fileManagerCSS(void);
    void fileManagerGetBackGround(void);

    // javascript, включая xmlhttp 
    String colorline(int i);
    String escapeHTMLcontent(String html);
    void fileManagerFileListInsert(void);
    void fileManagerFileEditorInsert(void);
    boolean allowAccessToThisFile(const String filename);
    void fileManagerCommandExecutor(void);
    void fileManagerReceiverOK(void);
    void fileManagerReceiver(void);

    // Zip-файл, несжатие/сохранение
    void getAllFilesInOneZIP(void);
    int WriteChunk(const char* b, size_t l);

    // helper: fs.h из esp32 и esp8266 не имеют совместимого решения для получения списка файлов из каталога
    #ifdef ESP32
    #define Dir File
    #endif
    File nextFile(Dir &dir);
    File firstFile(Dir &dir);
    size_t totalBytes(void);
    size_t usedBytes(void);

  public:
    ESPxWebFlMgr(word port);
    virtual ~ESPxWebFlMgr();

    void begin();
    void end();
    virtual void handleClient();

    // Это функция должно быть вызвано до того, как веб-страница будет загружена 
    // в браузер (она обеспечивает допустимое название цвета css, 
    // см. https://en.wikipedia.org/wiki/Web_colors
    void setBackGroundColor(const String backgroundColor);

    void setViewSysFiles(bool vsf);
    bool getViewSysFiles(void);

    void setSysFileStartPattern(String sfsp);
    String getSysFileStartPattern(void);
};

#endif

/*
      History

        -- 2019-07-07
           + модуль переименован в ESPxWebFlMgr и обеспечена его работа с esp32 и esp8266
           + выделена отдельная веб-страница файлового менеджера, и "build script" для ее создания

        -- 2019-07-06
           + "Download all files" создает zip-файл из всех файлов и загружает его
           + обеспечена возможность установки цвета фона
           - исправления в html5

        -- 2019-07-03
           + Public Release on https://github.com/holgerlembke/ESP8266WebFlMgr

      Что нужно сделать
        ?? унифицировать доступ к файловой системе для SPIFFS, LittleFS и HDFS
*/

// ********************************************************* ESPxWebFlMgr.h ***

