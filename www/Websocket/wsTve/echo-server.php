<?php 
//
//           Загрузка сервера из командной строки:   php.exe -f echo-server.php
//           Подключение к серверу:                         ws://127.0.0.1:7777

$iip=$_POST['ip'];

error_reporting(E_ALL);
set_time_limit(0);
ob_implicit_flush();

require 'WebSocketServer_class.php';
echo "112\n";

$server = new WebSocketServer('127.0.0.1', $iip);
// максимальное время работы 100 секунд, выводить сообщения в консоль
$server->settings(100, true);

// эта функция вызывается, когда получено сообщение от клиента
$server->handler = function($connect, $data) 
{
    // полученные от клиента данные отправляем обратно
    WebSocketServer::response($connect, $data);
};

$server->startServer();

echo "221";


