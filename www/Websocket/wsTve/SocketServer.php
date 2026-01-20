<?php
// PHP7/HTML5, EDGE/CHROME/YANDEX                *** wsTve/SocketServer.php ***

// ****************************************************************************
// * === php SocketServer.php ===                      Запустить сокет-сервер *
// ****************************************************************************

// v3.0.1, 20.01.2026                                 Автор:      Труфанов В.Е.
// Copyright © 2025 tve                               Дата создания: 18.03.2025

error_reporting(E_ALL);
set_time_limit(0);
ob_implicit_flush();

require 'WebSocketServer.php';

$iphost=$_POST['ip'];
$porthost=$_POST['pport'];

$server = new WebSocketServer($iphost, $porthost);
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

// ************************************************* wsTve/SocketServer.php ***
