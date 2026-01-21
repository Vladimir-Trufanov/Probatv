
## wsRusakov - изучение материалов по сайтам Михаила Русакова

> ***Главное понять следующее: сервер - это непрерывно выполняющийся скрипт, который просто ждёт подключения клиентов. Затем принимающий запрос и на основании этого запроса, возвращающий клиенту ответ. ***

### [Сокеты: Сервер на PHP](https://myrusakov.ru/php-socket-server.html)

### [Сокеты: Клиент на PHP](https://myrusakov.ru/php-socket-client.html)

### rusEx1 - первый пример, как есть, из статей Михаила Русакова

Делаем 2 шага:

- запускаем скрипт сервера из консоли Windows (из командной строки):

***php -f RusakovServer.php***;

- запускаем ***index.php*** (это клиент) и смотрим результат взаимодействия клиента и сервера.

### rusSocketServer - сайт, как сервер

#### Вариант A: 

В этом каталоге сделан ***index.php*** из ***RusakovServer.php*** и запускается, как сайт. Клиент запускается, как сайт из ***rusEx1***. 

#### Вариант Б: 

Сервер запускается, как страница сайта: 
***http://localhost:88/Websocket/wsRusakov/rusSocketServer/***

Клиент запускается, как страница сайта: 
***http://localhost:88/Websocket/wsRusakov/rusEx1/***


На ***'probatv.ru'***:

***http://probatv.ru/Websocket/wsRusakov/rusSocketServer/***

***http://probatv.ru/Websocket/wsRusakov/rusEx1/***

---

***https://probatv.ru/Websocket/wsRusakov/rusSocketServer/***

***https://probatv.ru/Websocket/wsRusakov/rusEx1/***

---
