<?php
// PHP7/HTML5, EDGE/CHROME/YANDEX                 ***  wsTve/LeadSocket.php ***

// ****************************************************************************
// *                                  Управлять сокетом                       *
// ****************************************************************************

// v3.0.2, 20.01.2026                                 Автор:      Труфанов В.Е.
// Copyright © 2024 tve                               Дата создания: 18.03.2025

/*
$sockserver=$_SERVER['HTTP_HOST'].'/SockServer';
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL,$sockserver);
curl_setopt($ch, CURLOPT_POST, 1);
сurl_setopt($ch, CURLOPT_POSTFIELDS, "name=value");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
$output = curl_exec($ch);
curl_close($ch);
echo $output;
*/

$hostname=$_SERVER['SERVER_NAME'];
$hostprotocol=$_SERVER['SERVER_PROTOCOL'];
echo $hostprotocol.': '.$hostname.':'.$_SERVER['SERVER_PORT'].' ['.$_SERVER['SERVER_SOFTWARE'].'] '.'from '. $_SERVER['REMOTE_ADDR'].':'.$_SERVER['REMOTE_PORT'].'<br>';

if ($hostname=='probatv.ru') $ip='87.242.70.183';
else if ($hostname=='localhost') $ip='127.0.0.1';
else if ($hostname=='ittve.ru') $ip='90.156.240.217';
else $ip='0.0.0.0';

if ($_SERVER['HTTPS']==1) $ws='wss';
else $ws='ws';

$port=7774;
$echoserver='SocketServer.php';

//echo $_SERVER['HTTPS'].'<br>';
//echo $_SERVER['SERVER_PORT'].'<br>';

echo '
   <p><button onclick="isSocket(\''.$echoserver.'\',\''.$ip.'\','.$port.')">Загрузить Websocket Server</button></p>
   <p><button onclick="resetSocket()">Удалить Websocket Server</button></p>
   <div>
       <span>Сервер</span>
       <input id="server" type="text" value="'.$ws.'://'.$ip.':'.$port.'" />
   </div>
';

?>
    <!--
    <p><button onclick="isSocket('Kvizzy30/echo-server.php',7776)">Загрузить Websocket Server</button></p>
    <div>
        <span>Сервер</span>
        <input id="server" type="text" value="ws://127.0.0.1:7776" />
    </div>
     -->
     
    <div>
        <input id="connect" type="button" value="Установить соединение" />
        <input id="disconnect" type="button" value="Разорвать соединение" />
    </div>
    <div>
        <span>Сообщение</span>
        <input id="message" type="text" value="" />
        <input id="send-msg" type="button" value="Отправить сообщение" />
    </div>
    <div>
        <span>Информация</span>
        <div id="socket-info"></div>
    </div>

<?php

// <!-- --> *****************************************  wsTve/LeadSocket.php ***

