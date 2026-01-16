<?php

  // php.exe -f RusakovServer.php

  header('Content-Type: text/plain;');  // Мы будем выводить простой текст
  set_time_limit(0);                    // Скрипт должен работать постоянно
  ob_implicit_flush();                  // Все echo должны сразу же отправляться клиенту

  // $address = '127.0.0.1';            // Адрес работы сервера
  // $address = '87.242.70.183';        // Адрес работы сервера
  
  $address = '127.0.0.1';               // Адрес работы сервера
  $port = 7774;   // 1985;              // Порт работы сервера (лучше какой-нибудь редкоиспользуемый)
  
  if (($sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) < 0) 
  {
    // AF_INET - семейство протоколов
    // SOCK_STREAM - тип сокета
    // SOL_TCP - протокол
    echo "Server, ошибка создания сокета ".$address.':'.$port;
  }
  else 
  {
    echo "Server, сокет создан ".$address.':'.$port."\n";
  }
  // Связываем дескриптор сокета с указанным адресом и портом
  if (($ret = socket_bind($sock, $address, $port)) < 0) 
  {
    echo "Server: Ошибка связи сокета с адресом и портом";
  }
  else 
  {
    echo "Server: Сокет успешно связан с адресом и портом\n";
  }
  // Начинаем прослушивание сокета (максимум 5 одновременных соединений)
  if (($ret = socket_listen($sock, 5)) < 0) 
  {
    echo "Server: Ошибка при попытке прослушивания сокета";
  }
  else 
  {
    echo "Server: Ждём подключение клиента\n";
  }
  do 
  {
    //Принимаем соединение с сокетом
    if (($msgsock = socket_accept($sock)) < 0) 
    {
      echo "Server: Ошибка при старте соединений с сокетом";
    } 
    else 
    {
      echo "Server: Сокет готов к приёму сообщений\n";
    }
    $msg = "Hello!";                            // Сообщение клиенту
    echo "Server-cообщение: $msg\n";
    socket_write($msgsock, $msg, strlen($msg)); // Запись в сокет
    // Бесконечный цикл ожидания клиентов
    do 
    {
      echo 'Server: Сообщение от клиента: ';
      if (false === ($buf = socket_read($msgsock, 1024))) 
      {
        echo "Server: Ошибка при чтении сообщения от клиента";
      }
      else 
      {
        echo $buf."\n"; // Сообщение от клиента
      }
      // Если клиент передал exit, то отключаем соединение
      if ($buf == 'exit') 
      {
        socket_close($msgsock);
        break 2;
      }
      if (!is_numeric($buf)) echo "Server: передано НЕ число\n";
      else 
      {
        $buf = $buf * $buf;
        echo "Сервер-сообщение: ($buf)\n";
      }
      socket_write($msgsock, $buf, strlen($buf));
    } while (true);
  } while (true);
  // Останавливаем работу с сокетом
  if (isset($sock)) 
  {
    socket_close($sock);
    echo "Сервер: Сокет успешно закрыт";
  }
?>
