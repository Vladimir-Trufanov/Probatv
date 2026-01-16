<?php
/**
 * Created by PhpStorm.
 * User: maestroprog
 * Date: 21.08.17
 * Time: 22:31
 * 
 * by tve on 2026-01-16
 */

require_once 'WebSocketServer.php';

$server = new WebSocketServer(8898);

// Слушаем входящие соединения
while (false !== ($activity = $server->listen()))
{
  // Каждую секунду читаем поступающие данные от клиентов
  foreach (array_keys($server->clients) as $address) 
  {
    if ($data = $server->readFrom($address))
    {
      // Эмулируем работу эхо-сервера, отправляем полученные данные клиенту
      $server->sendTo($address, var_export($data, true));
    }
  }
  sleep(1);
}
