## wsTve - создание управляемого эхо-сервера на сайте

### 2026-01-19 Первые опыты.

- Запускаю сервер через аякс на 100 секунд. Все это время сервер работает в окне, из которого он был запущен, а также из второго окна !!!

- По истечении 100 секунд сервер отключается, 

```
SocketServer.php:1  Failed to load resource: the server responded with a status of 500 (Internal Server Error)
```
и его можно запустить только после перезагрузки сайта !!!

- Подключаю сайт ***probatv.ru***. Удаётся подключиться к серверу, если сервер запускался с локального сайта.

- Загрузить сервер на локальной машине со станицы на ***probatv.ru*** не получается !!!

### На localhost:83 и ittve.ru

#### http://localhost:83/Websocket/wsTve/

#### http://ittve.ru/Websocket/wsTve/

> ***2026-01-23 Удачно произошла загрузка сокетсервера на ittve.ru и связь с моноблока!!!***

### "как на php проверить занят ли порт"

Проверить, занят ли порт на PHP, можно с помощью функций, связанных с работой сокетов. Однако подход зависит от протокола — TCP или UDP. 

#### Для TCP

Функция fsockopen позволяет создать сокет TCP и подключиться к порту. Если порт недоступен, провайдер TCP на удалённом компьютере отклонит запрос на подключение, и функция вернёт логическое значение failed. 

[Примеры кода 1 и 2:](https://www.binarytides.com/tcp-connect-port-scanner-php/)

```
$from = 1; $to = 255; // TCP-порты  
$host = '192.168.1.2';  
for ($port = $from; $port <= $to; $port++) 
{  
    $fp = fsockopen($host, $port);  
    if ($fp) 
    {  
        echo "port $port open \n";  
        fclose($fp);  
    }  
} 
``` 

```
/*
  Simple TCP connect port scanner in php using fsockopen
  
  В этом примере для подключения к хосту через порт используются функции 
  сокета php socket_create и socket_connector. Если соединение установлено, 
  функция socket_connect возвращает значение true , указывающее на то, что 
  порт открыт.
*/

//avoid warnings PHP Warning:  fsockopen(): unable to connect to 192.168.1.2:83 (Connection refused) in /var/www/blog/port_scanner.php on line 10
error_reporting(~E_ALL);

$from = 1;
$to = 255;

//TCP ports
$host = '192.168.1.2';

//Create a socket
$socket = socket_create(AF_INET , SOCK_STREAM , SOL_TCP);  
for($port = $from; $port <= $to ; $port++)
{
    //connect to the host and port
    $connection = socket_connect($socket , $host ,  $port);
    if ($connection)
    {
      echo "port $port open \n";
      
      //Close the socket connection
      socket_close($socket);

      //Create a new since earlier socket was closed , we need to close and recreate only when a connection is made
      //otherwise we can use the same socket
      $socket = socket_create(AF_INET , SOCK_STREAM , SOL_TCP);  
    }	 
}
```
[Пример кода 3:](https://translated.turbopages.org/proxy_u/en-ru.ru.20e4f8fa-69706994-9618986c-74722d776562/https/stackoverflow.com/questions/11955121/how-to-identify-whether-servers-port-is-accessible)

**Важно**: код проверяет, принимает ли сервер соединения по протоколу TCP, но не гарантирует, что приложение, прослушивающее этот порт, на самом деле является тем приложением, которое нужно проверить, или что оно функционирует правильно.

```
// Source - https://translated.turbopages.orghttps://translated.turbopages.org/proxy_u/en-ru.ru.20e4f8fa-69706994-9618986c-74722d776562/https/stackoverflow.com/a
// Posted by DaveRandom, modified by community. See post 'Timeline' for change history
// Retrieved 2026-01-21, License - CC BY-SA 3.0

<?php

    // Host name or IP to check
    $host = 'www.example.com';

    // Number of seconds to wait for a response from remote host
    $timeout = 2;

    // TCP port to connect to
    $port = 25565;

    // Try and connect
    if ($sock = fsockopen($host, $port, $errNo, $errStr, $timeout)) 
    {
        // Connected successfully
        $up = TRUE;
        fclose($sock); // Drop connection immediately for tidiness
    } else {
        // Connection failed
        $up = FALSE;
    }

    // Display something    
    if ($up) {
        echo "The server at $host:$port is up and running :-D";
    } else {
        echo "I couldn't connect to the server at $host:$port within $timeout seconds :-(<br>\nThe error I got was $errNo: $errStr";
    }
```
Пример 4:

Причина, по которой порт может не работать, заключается в правилах трафика,
установленных хостинг-провайдером. Они могут легко запретить исходящие
подключения к портам, отличным от 80.

Можно попробовать установить разумное для вашего случая время ожидания, д
обавив ещё один параметр, например @fsockopen($host, $port, $errno, $errstr, $timeout):

```
$host = 'theofilaktoshouse.zapto.org';
$ports = array(80, 8090, 34134);

foreach ($ports as $port)
{
    $errno  = null;
    $errstr = null;
    
    $connection = @fsockopen($host, $port, $errno, $errstr);

    if (is_resource($connection)) {
        echo '<h2>' . $host . ':' . $port . ' ' . '(' . getservbyport($port, 'tcp') . ') is open.</h2>' . "\n";
        fclose($connection);
    } else {
        echo "<h2>{$host}:{$port} is not responding. Error {$errno}: {$errstr} </h2>" . "\n";
    }
}
```

[Пример 5:](https://www.php.net/manual/ru/function.fsockopen.php)

fsockopen — открытие соединения через сокет домена Интернета или Unix. 
По умолчанию сокет открывается в блокирующем режиме. Вы можете перевести 
его в неблокирующий режим с помощью функции stream_set_blocking(). 

Функция stream_socket_client() аналогична, но предоставляет более широкий набор
опций, включая неблокирующее соединение и возможность предоставления 
контекста потока.

Поскольку fsockopen либо устанавливает соединение, либо не устанавливает (по
таймауту), это позволяет определить, доступно ли соединение («открыто») или заблокировано (брандмауэром и т. д.).

```
// Проверить связь по доменному имени веб-сайта, IP-адресу или имени хоста.

function example_pingDomain($domain)
{
  $starttime = microtime(true);
  $file = @fsockopen($domain, 80, $errno, $errstr, 10);
  $stoptime  = microtime(true);
  $status = 0;
  if (!$file) 
  {
    $status = -1;  // Site is down
  } 
  else 
  {
    fclose($file);
    $status = ($stoptime - $starttime) * 1000;
    $status = floor($status);
  }
  return $status;
}


```


#### Для UDP

 UDP протокол без подтверждения приема, поэтому отправка всегда успешна.

Функция fsockopen не позволяет создать сокет UDP, и проверка занятости порта по
этому протоколу менее надёжна. Порт считается открытым только когда за ним 
сидит сервер и отвечает на запросы. 

[Пример кода (не работает, так как UDP — протокол без подтверждения приёма):](https://phpclub.ru/talk/threads/is-udp-opened.36496/)

```
$fp = fsockopen("udp://127.0.0.1", 445);  
if ($fp) {  
    print "OK";  
}  
```

