<?php 
// wsTve          === php SocketServer.php ===         *** SocketServer.php ***
 
error_reporting(E_ALL);
set_time_limit(0);
ob_implicit_flush();

require 'WebSocketServer.php';

$server = new WebSocketServer('127.0.0.1', 7777);
// максимальное время работы 100 секунд, выводить сообщения в консоль, ВЫВОДИТЬ В ФАЙЛ
$server->settings(100, true, true);

// Переопределяем функцию приёма сообщения по умолчанию, 
// на заданную для текущего сервера
$server->handler = function($connect, $data) 
{
  // полученные от клиента данные отправляем обратно
  WebSocketServer::response($connect, $data);
};

$server->startServer();
echo "ZHDEM Ждем 5 сек";
sleep(5);
$server->stopServer();
