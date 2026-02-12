<?php
// PHP7/HTML5, EDGE/CHROME/YANDEX             *** wsTve/WebSocketServer.php ***

// ****************************************************************************
// *                        Построить класс сокет-сервера                     *
// ****************************************************************************

// v3.0.3, 12.02.2026                                 Автор:      Труфанов В.Е.
// Copyright © 2025 tve                               Дата создания: 18.03.2025

/** 
 * Для информации:
 * 
 * Протокол WebSocket работает над TCP. Это означает, что при соединении браузер 
 * отправляет по HTTP специальные заголовки, спрашивая: «Поддерживает ли сервер WebSocket?». 
 * Если сервер в ответных заголовках отвечает «Да, поддерживаю», 
 * то дальше HTTP прекращается и общение идёт на специальном протоколе WebSocket, 
 * который уже не имеет с HTTP ничего общего.
 * 
 * GET /chat HTTP/1.1
 * Host: websocket.server.com
 * Upgrade: websocket
 * Connection: Upgrade
 * Origin: http://www.example.com
 * Sec-WebSocket-Key: Iv8io/9s+lYFgZWcXczP8Q==
 * Sec-WebSocket-Version: 13
 * 
 * Здесь GET и Host — стандартные HTTP-заголовки, а Upgrade и Connection указывают, 
 * что браузер хочет перейти на WebSocket.
 * 
 * Сервер может проанализировать эти заголовки и решить, разрешает ли он WebSocket 
 * с данного домена Origin. 
 * 
 * Ответ сервера, если он понимает и разрешает WebSocket-подключение:
 * 
 * HTTP/1.1 101 Switching Protocols
 * Upgrade: websocket
 * Connection: Upgrade
 * Sec-WebSocket-Accept: hsBlbuDTkk24srzEOTBUlZAlC2g=
 *  
**/
class WebSocketServer 
{
  public  $handler;              // Указатель функции-обработчика сообщений от клиентов

  private $mess;                 // Текущее сообщение
  private $ip;                   // IP адрес сервера
  private $port;                 // Порт сервера
  private $connection;           // Сокет для принятия новых соединений, прослушивает указанный порт
  private $connects;             // Для хранения всех подключений, принятых слушающим сокетом
  private $timeLimit = 0;        // Ограничение по времени работы сервера
  private $startTime;            // Время начала работы сервера
  private $verbose=false;        // true - выводить сообщения в консоль (в командную строку, если локальный запуск php "SocketServer.php")
  private $logging=true;         // true - записывать сообщения в log-файл
  private $logFile='ws-log.txt'; // Имя log-файла
  private $resource;             // Ресурс log-файла

  // **************************************************************************
  // *                          Создать экземпляр класса                      *
  // **************************************************************************
  public function __construct($ip = '127.0.0.1', $port = 7777) 
  {
    $this->ip = $ip;
    $this->port = $port;
    // Определяем функцию по умолчанию для приема сообщения от клиента
    // (эта функция вызывается, когда получено сообщение от клиента;
    // при создании экземпляра класса должна быть переопределена)
    $this->handler = function($connection, $data) 
    {
      $message='['.date('r').'] Получено сообщение от клиента: '.$data.PHP_EOL;
      if ($this->verbose) 
      {
        echo $message;
      }
      if ($this->logging) 
      {
        fwrite($this->resource, $message);
      }
    };
  }
  public function __destruct() 
  {
    $this->debug('__destruct()');
    if (is_resource($this->connection)) 
    {
      $this->stopServer();
    }
    if ($this->logging) 
    {
      fclose($this->resource);
    }
  }
  // **************************************************************************
  // *                   Установить дополнительные настройки                  *
  // **************************************************************************
  public function settings($timeLimit = 0, $verbose = false, $logging = false, $logFile = 'ws-log.txt') 
  {
    $this->timeLimit = $timeLimit;
    $this->verbose = $verbose;
    $this->logging = $logging;
    $this->logFile = $logFile;
    if ($this->logging) 
    {
      $this->resource = fopen($this->logFile,'a');
    }
  }
  // **************************************************************************
  // *         Вывести сообщение в консоль и/или записать в лог-файл          *
  // **************************************************************************
  private function debug($message) 
  {
    $mess = '[' . date('r') . '] ' . $message . PHP_EOL;
    if ($this->verbose) 
    {
      echo $mess;
    }
    if ($this->logging) 
    {
      fwrite($this->resource, $mess);
    }
  }  
  // **************************************************************************
  // *                         Отправить сообщение клиенту                    *
  // **************************************************************************
  public static function response($connect, $data) 
  {
    // $connect (обязательный параметр) — ресурс сокета, полученный из socket_create или socket_accept;
    // $data (обязательный параметр) — строка с данными для отправки через сокет;
    // $length (необязательный) — максимальное количество байт для записи. Если параметр не указан, функция записывает все данные из строки.
    //   Важно: функция не обязана записывать каждый байт буфера data — это нормально, 
    // когда функция записывает только конкретное количество байтов или даже один байт, 
    // хотя размер буфера больше. На поведение функции влияют сетевые буферы и другие настройки.
    //   Функция возвращает количество успешно записанных байт или false в случае ошибки.
    // Допускается возврат нулевого значения, когда функция не записала ни одного байта.
    // Значение, которое вернула функция, проверяют на равенство значению false оператором ===.
    //   Код ошибки можно получить функцией socket_last_error(). Этот код можно 
    // передать функции socket_strerror() для получения текстового описания ошибки. 
    socket_write($connect, self::encode($data));
  }
  // **************************************************************************
  // *                          Запустить сервер в работу                     *
  // **************************************************************************
  public function startServer() 
  {
    $this->debug('Сервер запускается в работу ...');
    // Создаём новый сокет для сетевого взаимодействия. 
    //   AF_INET — домен сокета, возможные значения: AF_INET (IPv4), AF_INET6 (IPv6), 
    // AF_UNIX (локальные сокеты).
    //   SOCK_STREAM — тип сокета, основные значения: SOCK_STREAM (TCP), SOCK_DGRAM (UDP), 
    // SOCK_RAW (сырые сокеты).
    //   SOL_TCP — протокол, обычно SOL_TCP для TCP или SOL_UDP для UDP, либо 0 
    // для автоматического выбора.
    //   Важно: если создан сокет с AF_UNIX, в файловой системе создаётся файл, 
    // который не удаляется при вызове socket_close — после закрытия сокета файл 
    // нужно удалить. 
    $this->connection = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    if (false === $this->connection) 
    {
      $this->debug('Ошибка создания сокета: ' . socket_strerror(socket_last_error()));
      return;
    }
    // Связываем сокет с IP-адресом и портом, на которых сокет будет принимать подключения.
    // Это обязательная операция для серверных сокетов перед началом прослушивания входящих соединений. 
    // Параметры: 
    //   socket (обязательный) — ресурс сокета, созданный функцией socket_create;
    //   address (обязательный): если сокет принадлежит семейству AF_INET, адрес — IP-адрес 
    // в 4-точечной нотации (например, 127.0.0.1). Если сокет принадлежит семейству 
    // AF_UNIX, адрес — путь сокета (например, /tmp/my.sock);
    //   port (необязательный) — номер порта для привязки. Для клиентских сокетов 
    // обычно не указывается (или указывается 0). 
    //   Важно: функцию вызывают на сокете перед подключением к нему функцией socket_connect() 
    // или socket_listen(). 
    //   Функция возвращает true, если выполнилась успешно, или false, если возникла ошибка.
    // Код ошибки можно запросить функцией socket_last_error(). Этот код можно передавать 
    // в socket_strerror() для получения текстового объяснения ошибки. 
    $bind = socket_bind($this->connection, $this->ip, $this->port); 
    if (false === $bind) 
    {
      $this->debug('Ошибка связывания сокета: ' . socket_strerror(socket_last_error()));
      return;
    }
    // Выполняем настройку сокета (здесь разрешаем использовать один порт для нескольких соединений).
    // Параметры:
    //   socket (тип Socket) — ресурс сокета, созданный функцией socket_create;
    //   level (тип int) — уровень, на котором устанавливается опция. Распространённые значения: 
    // SOL_SOCKET, IPPROTO_TCP, IPPROTO_IP, IPPROTO_IPV6;
    // Доп.информация:
    //   SOL_SOCKET, IPPROTO_TCP, IPPROTO_IP и IPPROTO_IPV6 — это уровни параметров сокетов, 
    // которые указывают на уровень протокола, для которого предназначены опции. Эти параметры 
    // связаны с разными уровнями стека сетевой коммуникации: уровнем сокета, 
    // уровнем TCP, уровнем IP и уровнем IPv6. 
    //   SOL_SOCKET — уровень, на котором находятся общие опции, применимые ко всем типам сокетов. 
    // Такие опции не обязательно специфичны для конкретного протокола. Например:
    // SO_REUSEADDR — позволяет локальному сокету использовать локальный адрес, который может быть уже занят.
    // SO_KEEPALIVE — позволяет приложению закрывать подключённый сокет, если удалённый 
    // конец закрывается без уведомления локального сокета. SO_SNDBUF и SO_RECVBUF 
    // позволяют контролировать локальный буфер сокета для отправки и приёма данных.
    // Однако не все протоколы реализуют каждый параметр уровня SOL_SOCKET. Например, 
    // SO_BROADCAST переводит сокет в режим широковещательной передачи, но не все 
    // протоколы поддерживают эту опцию. 
    //   IPPROTO_TCP — уровень, на котором находятся опции, специфические для протокола 
    // Transmission Control Protocol (TCP). Например: TCP_NODELAY — отключает (1) или включает (0) 
    // алгоритм Nagle: если 1, небольшие сегменты TCP отправляются немедленно, без ожидания объединения.
    // TCP_KEEPALIVE — задаёт интервал времени в секундах между проверками активности 
    // (значение по умолчанию — 2 часа). Опции уровня IPPROTO_TCP интерпретируются внутренним кодом TCP/IP. 
    //   IPPROTO_IP — уровень, на котором находятся опции, специфические для протокола 
    // Internet Protocol (IPv4). Некоторые из них: IP_MULTICAST_LOOP — для сокета, 
    // присоединённого к одной или нескольким многоадресным группам, контролирует, 
    // будет ли сокет получать копию исходящих пакетов, отправленных в эти группы. 
    // По умолчанию опция включена (значение 1/TRUE), но если установить 0/FALSE, 
    // сокет не будет получать многоадресные пакеты, отправленные с локальной машины.
    // IP_OPTIONS — указывает опции IP, которые будут вставляться в исходящие пакеты. 
    // Установка новых опций перезаписывает все ранее указанные. Параметры уровня 
    // IPPROTO_IP применяются к сокетам, созданным для семейства адресов IPv4 (AF_INET). 
    //   IPPROTO_IPV6 — уровень, на котором находятся опции, специфические для протокола 
    // IPv6. Некоторые из них: IPV6_ADDRFORM — преобразует сокет AF_INET6 в сокет 
    // другого семейства адресов (поддерживается только AF_INET). IPV6_ADD_MEMBERSHIP, IPV6_DROP_MEMBERSHIP 
    // определяет членство в многоадресных (multicast) группах. IPV6_UNICAST_HOPS —
    // определяет лимит однозначных (unicast) пересылок (hop) для сокета. Значение -1 
    // означает использование маршрута по умолчанию, в противном случае значение должно 
    // быть в диапазоне от 0 до 255. IPV6_V6ONLY — если этот флаг установлен (не равен 0), 
    // то сокет отправляет и принимает только пакеты IPv6. Если флаг сброшен (равен 0), 
    // то сокет можно использовать для отправки и приёма пакетов с и на адрес IPv6 или адрес IPv4-преобразованный-в-IPv6.
    //   option (тип int) — идентификатор устанавливаемой опции. Примеры: SO_REUSEADDR, SO_RCVTIMEO, TCP_NODELAY.
    //   value (тип mixed) — значение опции. Может быть массивом, целым числом, булевым значением 
    // или другой структурой в зависимости от опции.
    //   SO_REUSEADDR — предопределённая константа в расширении sockets PHP, 
    // которая позволяет повторно использовать локальный адрес для сокета. Это позволяет 
    // избежать ошибки «Адрес уже используется» (Address already in use) при привязывании 
    // сокета к порту, даже если он недавно использовался другим процессом.
    //   Константа доступна только если расширение sockets скомпилировано с PHP или 
    // динамически загружено на этапе прогона. SO_REUSEADDR сообщает ядру, что даже если 
    // порт занят (в состоянии TIME_WAIT), можно продолжать использовать его. Это полезно 
    // в некоторых ситуациях: перезапуск сервера — после остановки сервера нужно дождаться, 
    // пока сокет выйдет из состояния TIME_WAIT, прежде чем другой сервер может привязать 
    // к тому же порту. С SO_REUSEADDR можно привязать сокет немедленно, даже если 
    // он находится в состоянии TIME_WAIT.
    //   Привязка к адресу, который используется другим процессом — SO_REUSEADDR позволяет 
    // повторно использовать адрес, но привязка к порту разрешена только для одного процесса. 
    // Например, если клиент отключается и порт переходит в состояние TIME_WAIT, 
    // SO_REUSEADDR позволяет использовать этот порт в любом подключении, за исключением 
    // подключения к другому процессу.
    //   Важно: SO_REUSEADDR не позволяет запустить два сокета на одном и том же порте одновременно. 
    //   Функция возвращает true при успехе и false при ошибке.
    //   Некоторые типичные ошибки:
    // Установка опции на несуществующем сокете — попытка установить опцию на ресурсе, 
    // который не является сокетом или уже закрыт.
    // Некорректный уровень или опция — использование неверной комбинации уровня и опции.
    // Неправильный формат значения — передача значения в неверном формате, например, числа вместо массива для таймаута.    
    $option = socket_set_option($this->connection, SOL_SOCKET, SO_REUSEADDR, 1);
    if (false === $option) 
    {
      $this->debug('Ошибка установки опций: ' . socket_strerror(socket_last_error()));
      return;
    }
    // Начинаем прослушивать сокет
    $listen = socket_listen($this->connection); 
    if (false === $listen) 
    {
      $this->debug('Ошибка прослушивания сокета: ' . socket_strerror(socket_last_error()));
      return;
    }
    $this->debug('Сервер начал прослушивание ...');

    $this->connects = array($this->connection);
    $this->startTime = time();

    while (true) 
    {
      $this->debug('Ожидается соединение ...');
      // Создаем копию массива, так что массив $this->connects не будет изменен функцией socket_select()
      $read = $this->connects;
      $write = $except = null;

      /**
       * Сокет $this->connection только прослушивает порт на предмет новых соединений. Как только поступило
       * новое соединение, мы создаем новый ресурс сокета с помощью socket_accept() и помещаем его в массив
       * $this->connects для дальнейшего чтения из него.
      **/

      // Ожидаем сокеты, доступные для чтения (без таймаута)      
      if (!socket_select($read, $write, $except, null)) 
      { 
        break;
      }

      // Если слушающий сокет есть в массиве чтения, значит было новое соединение
      if (in_array($this->connection, $read)) 
      {
        // Принимаем новое соединение и производим рукопожатие
        if (($connect = socket_accept($this->connection)) && $this->handshake($connect)) 
        {
          $this->debug('Принято новое соединение');
          // Добавляем новое соединение в список необходимых для обработки
          $this->connects[] = $connect; 
        }
        // Удаляем слушающий сокет из массива для чтения
        unset($read[ array_search($this->connection, $read) ]);
      }  
      // Обрабатываем все соединения, в которых есть данные для чтения             
      foreach ($read as $connect) 
      { 
        $data = socket_read($connect, 100000);
        $decoded = self::decode($data);
        // Если клиент не прислал данных или хочет разорвать соединение
        if (false === $decoded || 'close' === $decoded['type']) 
        {
          $this->debug('Соединение закрывается');
          socket_write($connect, self::encode('Closed on client demand', 'close'));
          socket_shutdown($connect);
          socket_close($connect);
          unset($this->connects[ array_search($connect, $this->connects) ]);
          $this->debug('Соединение успешно закрыто');
          continue;
        }
        // Получено сообщение от клиента, вызываем пользовательскую
        // функцию, чтобы обработать полученные данные
        if (is_callable($this->handler)) 
        {
          $mess=$decoded['payload'];
          //$this->debug('***'.$mess.'***');
          //call_user_func($this->handler, $connect, $decoded['payload']);
                  
          if ($mess=='wsStop')
          {
            $this->debug('Команда wsStop');
            $this->stopServer();
            return;
          }
          else call_user_func($this->handler, $connect, $mess);
        }
      }
      // Если истекло ограничение по времени, останавливаем сервер
      if ($this->timeLimit && time() - $this->startTime > $this->timeLimit) 
      {
        $this->debug('Лимит времени исчерпан. Сервер остановлен.');
        $this->stopServer();
        return;
      }
    }
  }
  // **************************************************************************
  // *                        Остановить работу сервера                       *
  // **************************************************************************
  public function stopServer() 
  {
    $this->debug('stopServer()');
    // sleep(5);
    // Закрываем слушающий сокет
    socket_close($this->connection);
    // Отправляем всем клиентам сообщение о разрыве соединения
    if (!empty($this->connects)) 
    { 
      foreach ($this->connects as $connect) 
      {
        if (is_resource($connect)) 
        {
          socket_write($connect, self::encode('  Closed on server demand', 'close'));
          socket_shutdown($connect);
          socket_close($connect);
        }
      }
    }
  }

    /**
     * Для кодирования сообщений перед отправкой клиенту
     */
    private static function encode($payload, $type = 'text', $masked = false) {
        $frameHead = array();
        $payloadLength = strlen($payload);

        switch ($type) {
            case 'text':
                // first byte indicates FIN, Text-Frame (10000001):
                $frameHead[0] = 129;
                break;
            case 'close':
                // first byte indicates FIN, Close Frame(10001000):
                $frameHead[0] = 136;
                break;
            case 'ping':
                // first byte indicates FIN, Ping frame (10001001):
                $frameHead[0] = 137;
                break;
            case 'pong':
                // first byte indicates FIN, Pong frame (10001010):
                $frameHead[0] = 138;
                break;
        }

        // set mask and payload length (using 1, 3 or 9 bytes)
        if ($payloadLength > 65535) {
            $payloadLengthBin = str_split(sprintf('%064b', $payloadLength), 8);
            $frameHead[1] = ($masked === true) ? 255 : 127;
            for ($i = 0; $i < 8; $i++) {
                $frameHead[$i + 2] = bindec($payloadLengthBin[$i]);
            }
            // most significant bit MUST be 0
            if ($frameHead[2] > 127) {
                return array('type' => '', 'payload' => '', 'error' => 'frame too large (1004)');
            }
        } elseif ($payloadLength > 125) {
            $payloadLengthBin = str_split(sprintf('%016b', $payloadLength), 8);
            $frameHead[1] = ($masked === true) ? 254 : 126;
            $frameHead[2] = bindec($payloadLengthBin[0]);
            $frameHead[3] = bindec($payloadLengthBin[1]);
        } else {
            $frameHead[1] = ($masked === true) ? $payloadLength + 128 : $payloadLength;
        }

        // convert frame-head to string:
        foreach (array_keys($frameHead) as $i) {
            $frameHead[$i] = chr($frameHead[$i]);
        }
        if ($masked === true) {
            // generate a random mask:
            $mask = array();
            for ($i = 0; $i < 4; $i++) {
                $mask[$i] = chr(rand(0, 255));
            }
            $frameHead = array_merge($frameHead, $mask);
        }
        $frame = implode('', $frameHead);

        // append payload to frame:
        for ($i = 0; $i < $payloadLength; $i++) {
            $frame .= ($masked === true) ? $payload[$i] ^ $mask[$i % 4] : $payload[$i];
        }

        return $frame;
    }

    /**
     * Для декодирования сообщений, полученных от клиента
     */
    private static function decode($data) {
        if ( ! strlen($data)) {
            return false;
        }

        $unmaskedPayload = '';
        $decodedData = array();

        // estimate frame type:
        $firstByteBinary = sprintf('%08b', ord($data[0]));
        $secondByteBinary = sprintf('%08b', ord($data[1]));
        $opcode = bindec(substr($firstByteBinary, 4, 4));
        $isMasked = ($secondByteBinary[0] == '1') ? true : false;
        $payloadLength = ord($data[1]) & 127;

        // unmasked frame is received:
        if (!$isMasked) {
            return array('type' => '', 'payload' => '', 'error' => 'protocol error (1002)');
        }

        switch ($opcode) {
            // text frame:
            case 1:
                $decodedData['type'] = 'text';
                break;
            case 2:
                $decodedData['type'] = 'binary';
                break;
            // connection close frame:
            case 8:
                $decodedData['type'] = 'close';
                break;
            // ping frame:
            case 9:
                $decodedData['type'] = 'ping';
                break;
            // pong frame:
            case 10:
                $decodedData['type'] = 'pong';
                break;
            default:
                return array('type' => '', 'payload' => '', 'error' => 'unknown opcode (1003)');
        }

        if ($payloadLength === 126) {
            $mask = substr($data, 4, 4);
            $payloadOffset = 8;
            $dataLength = bindec(sprintf('%08b', ord($data[2])) . sprintf('%08b', ord($data[3]))) + $payloadOffset;
        } elseif ($payloadLength === 127) {
            $mask = substr($data, 10, 4);
            $payloadOffset = 14;
            $tmp = '';
            for ($i = 0; $i < 8; $i++) {
                $tmp .= sprintf('%08b', ord($data[$i + 2]));
            }
            $dataLength = bindec($tmp) + $payloadOffset;
            unset($tmp);
        } else {
            $mask = substr($data, 2, 4);
            $payloadOffset = 6;
            $dataLength = $payloadLength + $payloadOffset;
        }

        /**
         * We have to check for large frames here. socket_recv cuts at 1024 bytes
         * so if websocket-frame is > 1024 bytes we have to wait until whole
         * data is transferd.
         */
        if (strlen($data) < $dataLength) {
            return false;
        }

        if ($isMasked) {
            for ($i = $payloadOffset; $i < $dataLength; $i++) {
                $j = $i - $payloadOffset;
                if (isset($data[$i])) {
                    $unmaskedPayload .= $data[$i] ^ $mask[$j % 4];
                }
            }
            $decodedData['payload'] = $unmaskedPayload;
        } else {
            $payloadOffset = $payloadOffset - 4;
            $decodedData['payload'] = substr($data, $payloadOffset);
        }

        return $decodedData;
    }

    /**
     * «Рукопожатие», т.е. отправка заголовков согласно протоколу WebSocket
     */
    private function handshake($connect) {

        $info = array();

        $data = socket_read($connect, 1000);
        $lines = explode("\r\n", $data);
        foreach ($lines as $i => $line) {
            if ($i) {
                if (preg_match('/\A(\S+): (.*)\z/', $line, $matches)) {
                    $info[$matches[1]] = $matches[2];
                }
            } else {
                $header = explode(' ', $line);
                $info['method'] = $header[0];
                $info['uri'] = $header[1];
            }
            if (empty(trim($line))) break;
        }

        // получаем адрес клиента
        $ip = $port = null;
        if ( ! socket_getpeername($connect, $ip, $port)) {
            return false;
        }
        $info['ip'] = $ip;
        $info['port'] = $port;

        if (empty($info['Sec-WebSocket-Key'])) {
            return false;
        }

        // отправляем заголовок согласно протоколу вебсокета
        $SecWebSocketAccept = 
            base64_encode(pack('H*', sha1($info['Sec-WebSocket-Key'] . '258EAFA5-E914-47DA-95CA-C5AB0DC85B11')));
        $upgrade = "HTTP/1.1 101 Web Socket Protocol Handshake\r\n" .
                   "Upgrade: websocket\r\n" .
                   "Connection: Upgrade\r\n" .
                   "Sec-WebSocket-Accept:".$SecWebSocketAccept."\r\n\r\n";
        socket_write($connect, $upgrade);

        return true;

    }

}

// ********************************************** wsTve/WebSocketServer.php ***
