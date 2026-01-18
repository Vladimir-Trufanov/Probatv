<?php
// PHP7/HTML5, EDGE/CHROME/YANDEX                        *** LeadSocket.php ***

// ****************************************************************************
// *                                  Управлять сокетом                       *
// ****************************************************************************

// v1.0.1, 18.03.2025                                 Автор:      Труфанов В.Е.
// Copyright © 2024 tve                               Дата создания: 18.03.2025

echo '
   <p><button onclick="isSocket(\''.$echoserver.'\','.$port.')">Загрузить Websocket Server</button></p>
   <div>
       <span>Сервер</span>
       <input id="server" type="text" value="ws://'.$ip.':'.$port.'" />
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

// <!-- --> ************************************************ LeadSocket.php ***

