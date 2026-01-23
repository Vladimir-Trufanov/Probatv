## ESP32-CamRecorder - видеомагнитофон на Esp32-Cam

Переделка [***ESP32-CAM-Video-Recorder-junior***](https://github.com/jameszah/ESP32-CAM-Video-Recorder-junior) для умного хозяйства tve.

Версия ***junior62*** проверена и работает в ***Arduino IDE 2.3.7 - Espressif ESP32 3.3.5***.

Трассировка вывода в последовательный порт первого запуска приложения:

```
12:15:17.091 -> ---
12:15:17.091 -> Arduino IDE 2.3.7 - Espressif ESP32 3.3.5
12:15:27.092 -> Mounting the SD card ...
12:15:27.160 -> SD_MMC Begin: 1
12:15:27.160 -> SD_MMC Card Type: SDHC
12:15:27.160 -> SD_MMC Card Size: 15103MB
12:15:27.160 ->                                     
12:15:27.160 -> ---------------------------------------
--
12:15:27.160 -> ---------------------------------------
12:15:27.160 -> ESP32-CAM-Video-Recorder-junior v62.34
12:15:27.160 -> ---------------------------------------
12:15:27.160 -> setup core: 1, Prio: 1, Internal Free Heap 245700 of 295688, SPI Free 4154188 of 4194304
12:15:27.205 -> --- reboot ------ because: ESP_RST_POWERON
12:15:27.205 -> Good settings in the EPROM New File Group 6
12:15:27.205 -> Writing to EPROM ...
12:15:27.205 -> Try to get parameters from config2.txt ...
12:15:28.175 -> Opened config2.txt from SDReading config2.txt
12:15:28.208 -> =========   Data from config2.txt and defaults  =========
12:15:28.208 -> Name desklens
12:15:28.208 -> Framesize 13
12:15:28.208 -> Quality 12
12:15:28.208 -> Buffers config 4
12:15:28.208 -> Length 1800
12:15:28.208 -> Interval 0
12:15:28.208 -> Speedup 1
12:15:28.208 -> Streamdelay 0
12:15:28.208 -> Zone len 3, GMT
12:15:28.208 -> ssid1 OPPO A9 2020
---
12:15:28.208 -> ssid1 OPPO A9 2020
12:15:28.208 -> ssid2 ssid
12:15:28.208 -> ssid3 ssid
12:15:28.208 -> pass3 12344321
12:15:28.208 -> Setting up the camera ...
12:15:28.208 -> Frame config 0, quality config 0, buffers config 4
12:15:28.248 -> Before camera config ...  core: 1, Prio: 1, Internal Free Heap 245176 of 295688, SPI Free 4154188 of 4194304
12:15:28.435 -> After  camera config ...  core: 1, Prio: 1, Internal Free Heap 202292 of 295688, SPI Free 3416844 of 4194304
12:15:28.435 -> 
12:15:28.435 -> Camera started correctly, Type is 26 (hex) of 9650, 7725, 2640, 3660, 5640
12:15:28.435 -> 
12:15:28.935 -> Pic  0, len=  41931, at mem 3F863D10
12:15:28.973 -> Pic  1, len=  44531, at mem 3F890D30
12:15:28.973 -> Pic  2, len=  44353, at mem 3F809CD0
12:15:31.008 -> Pic 28, len=  35311, at mem 3F809CD0
12:15:31.091 -> Pic 29, len=  24548, at mem 3F836CF0
12:15:31.091 -> Buffer size for 24548 is 98304
12:15:31.091 -> End of camera setup core: 1, Prio: 1, Internal Free Heap 202292 of 295688, SPI Free 3416844 of 4194304
12:15:31.133 -> setup - after malloc core: 1, Prio: 1, Internal Free Heap 202292 of 295688, SPI Free 3015420 of 4194304
12:15:31.133 -> Creating the_camera_loop_task
12:15:31.133 -> the_camera_loop core: 0, Prio: 4, Internal Free Heap 196688 of 295688, SPI Free 3015420 of 4194304
12:15:31.214 -> the_streaming_loop core: 1, Prio: 2, Internal Free Heap 188120 of 295688, SPI Free 3015420 of 4194304
12:15:31.257 -> Starting the streaming
12:15:31.257 -> Starting the wifi ... core: 1, Prio: 1, Internal Free Heap 188120 of 295688, SPI Free 3015420 of 4194304
12:15:31.351 -> 
12:15:31.351 -> >>>>>>>>>>>>>>>>>>>>>OPPO A9 2020<
12:15:31.351 -> >>>>>>>>>>>>>>>>>>>>>ssid<
12:15:31.351 -> >>>>>>>>>>>>>>>>>>>>>desklens< / >12344321<
---
12:15:31.351 -> >>>>>>>>>>>>>>>>>>>>>desklens< / >12344321<
12:15:32.146 -> 
12:15:32.146 -> 
12:15:32.146 -> Disconnect Pin 12 from GND to start recording or http://192.168.1.100/start 
12:15:32.146 -> 
12:15:36.083 -> F8:B3:B7:A7:E1:18
12:15:36.083 -> v5.5.1-931-g9bb7aa84fe
12:15:36.137 -> Setting AP (Access Point)вЂ¦AP IP address: 192.168.4.1
12:15:36.137 -> AP IP: 192.168.4.1
12:15:36.137 ->  
12:15:36.137 -> Char >GMT<
12:15:36.137 -> 
12:15:36.137 -> Local time: Thu Jan  1 00:00:19 1970
12:15:36.137 -> IP: 10.227.133.2
12:15:36.137 ->  
12:15:36.137 -> mDNS responder started 'desklens'
12:15:36.137 -> Starting the fileman ... core: 1, Prio: 1, Internal Free Heap 128284 of 295688, SPI Free 3010936 of 4194304
12:15:36.137 -> 
12:15:36.137 -> fm begin, core 1, priority = 1
12:15:36.137 -> start fm bindings
12:15:36.149 -> Open Filemanager with http://192.168.4.1:8080/
12:15:36.149 -> Open Filemanager with http://10.227.133.2:8080/
12:15:36.149 -> Starting Web Services ... core: 1, Prio: 1, Internal Free Heap 125904 of 295688, SPI Free 3010936 of 4194304
12:15:36.149 -> http task prio: 5
12:15:36.149 -> Camera http started
12:15:36.149 -> http Stream task prio: 5
12:15:36.149 -> Stream 81 http started
12:15:36.149 -> http Stream task prio: 5
12:15:36.149 -> Stream 82 http started
12:15:36.149 -> After the WiFi core: 1, Prio: 1, Internal Free Heap  96508 of 295688, SPI Free 3010360 of 4194304
---
12:15:36.149 -> After the WiFi core: 1, Prio: 1, Internal Free Heap  96508 of 295688, SPI Free 3010360 of 4194304
12:15:36.189 -> Checking SD for available space ...
12:15:36.189 -> Card  space: 15103MB
12:15:36.189 -> Total space: 15095MB
12:15:36.189 -> Used  space:  3404MB
12:15:36.189 -> Nothing deleted, 22.6% disk full
12:15:36.321 -> Creating logfile /desklens006/desklens006.999.txt
12:15:36.321 -> 
12:15:36.321 -> End of setup core: 1, Prio: 1, Internal Free Heap  96244 of 295688, SPI Free 3010360 of 4194304
12:15:36.354 -> 
12:15:36.354 -> ---  End of setup()  ---
12:15:36.354 -> 
12:15:36.354 -> starting in web_stop code
12:15:36.354 -> 
12:15:36.354 -> Start the avi ... at 19324
12:15:36.354 -> Framesize 13, quality 12, length 1800 seconds
12:15:36.393 -> 
12:15:36.393 -> Starting an avi File open: /desklens006/desklens006.001.avi
12:15:36.440 -> Recording 1800 seconds
12:15:40.255 -> Frame     50, len  25190, cam time      23,  sd time   30 -- 
12:15:45.072 -> So far: 0110 frames, in    8.7 seconds, for last 100 frames: avg frame size   25.2 kb, 12.49 fps ...
12:15:48.252 -> Frame    150, len  25700, cam time      23,  sd time   34 -- 
12:15:50.268 -> Frame    169, sd time very high  572 >>>   34 -- pos 42A0B8, Block 0, delay     0 ... 
12:15:53.537 -> So far: 0210 frames, in   17.2 seconds, for last 100 frames: avg frame size   25.3 kb, 11.80 fps ...
12:15:56.710 -> Frame    250, len  25693, cam time      16,  sd time   34 -- 
12:16:01.525 -> So far: 0310 frames, in   25.2 seconds, for last 100 frames: avg frame size   25.2 kb, 12.49 fps ...
12:16:02.353 -> Frame    315, sd time very high  461 >>>   35 -- pos 7C4BD0, Block 0, delay     0 ... 
12:16:05.112 -> Frame    350, len  25666, cam time      17,  sd time   38 -- 
12:16:09.943 -> So far: 0410 frames, in   33.6 seconds, for last 100 frames: avg frame size   25.3 kb, 11.91 fps ...
---
12:16:09.943 -> So far: 0410 frames, in   33.6 seconds, for last 100 frames: avg frame size   25.3 kb, 11.91 fps ...
12:16:13.478 -> Frame    450, len  24649, cam time      26,  sd time   31 -- 
12:16:18.280 -> So far: 0510 frames, in   41.9 seconds, for last 100 frames: avg frame size   24.3 kb, 11.95 fps ...
12:16:21.994 -> Frame    550, len  24763, cam time       0,  sd time   28 -- 
12:16:24.784 -> index_handler core: 1, Prio: 5, Internal Free Heap  85924 of 295688, SPI Free 3010552 of 4194304
12:16:25.005 -> Total captures     0, Last 30 sec: captures 0, 0.0 per second, skips 0, extras 0
12:16:25.005 -> capture core: 1, Prio: 5, Internal Free Heap  83612 of 295688, SPI Free 3010552 of 4194304
12:16:25.005 -> Socket: 58
12:16:25.005 -> family 10 Peer Client IP4: 10.227.133.201, Peer Client IP6: AE3:85C9::2004:4145:333A:3835, Client Port: 40234
12:16:25.050 -> family 10 Sock Client IP4: 10.227.133.2, Sock Client IP6: AE3:8502::2004:4145:333A:3835, Client Port: 80
12:16:25.143 -> find_handler core: 1, Prio: 5, Internal Free Heap  80012 of 295688, SPI Free 3010552 of 4194304
12:16:26.695 -> So far: 0610 frames, in   50.3 seconds, for last 100 frames: avg frame size   24.5 kb, 11.88 fps ...
12:16:29.879 -> Frame    650, len  24684, cam time      35,  sd time   35 -- 
12:16:32.315 -> stream_81_handler core: 1, Prio: 5, Internal Free Heap  84332 of 295688, SPI Free 3010744 of 4194304
12:16:33.481 -> Stream 81 at 8.834 fps
12:16:35.338 -> So far: 0710 frames, in   59.0 seconds, for last 100 frames: avg frame size   25.0 kb, 11.57 fps ...
12:16:39.044 -> Frame    750, len  36022, cam time       0,  sd time   49 -- 
12:16:41.824 -> Stream error - 81/2nd 45062
12:16:42.313 ->  stream_81 done
12:16:43.946 -> So far: 0810 frames, in   67.6 seconds, for last 100 frames: avg frame size   30.0 kb, 11.65 fps ...


```