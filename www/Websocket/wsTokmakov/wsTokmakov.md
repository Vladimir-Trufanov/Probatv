## [wsTokmakov - изучение материалов по сайтам Токмакова](wsTokmakov/wsTokmakov.md)

Примеры клиента и сокет-сервера на PHP по материалам сайтов:

### [WebSocket сервер на PHP](https://tokmakov.msk.ru/blog/item/202)

### [GitHub: web-socket-server](https://github.com/tokmakov/web-socket-server)

### tokEx1 - первый пример, как есть, из материалов Токмакова

- по ***simple.php*** создаём файл сервера ***tokEx1SocketServer.php***;

- запускаем его в работу в каталоге из командной строки:

***php.exe -f tokEx1SocketServer.php***

```
Microsoft Windows [Version 10.0.26200.7623]
(c) Корпорация Майкрософт (Microsoft Corporation). Все права защищены.

c:\Probatv\www\Websocket\wsTokmakov\tokEx1>php.exe -f tokEx1SocketServer.php
SERVER START
Socket create...
Socket bind...
Set options...
Listening socket...
Waiting for connections...

```

- по ***Win+R*** загружаем другую командную строку, запускаем ***telnet*** и видим его подключение:

```
Добро пожаловать в программу-клиент Microsoft Telnet

Символ переключения режима: 'CTRL+]'

Microsoft Telnet>

```

- Получив приглашение telnet, даем команду: ***open 127.0.0.1 7777***. Убеждаемся, что получили ответ от сервера и завершили его работу:

```
Hello, Client!

Подключение к узлу утеряно.

Нажмите любую клавишу...

```

### tokEx2 - второй пример с эхо-сервером из материалов Токмакова

- создаем класс WebSocketServer в файле ***tokEx2WebSocketServer.php***;

- для тестирования напишем PHP-скрипт, который запускает в работу сервер и все сообщения клиента отправляет обратно ***tokEx2echo-server.php***;

- для тестирования работы сервера создаем клиента ***tokEx2SocketClient.html***,
js-скрипт ***tokEx2socket.js*** и файл стилей ***tokEx2style.css***;

- tokEx2SocketClient.html в index.html и запускаем, как сайт;

- ***php.exe -f tokEx2echo-server.php***;
- ***php.exe -f echo-server.php***;





