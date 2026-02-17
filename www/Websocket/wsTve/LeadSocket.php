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

//$ip='90.156.240.217';

$ip='87.242.70.183';
$ws='ws';


//echo $_SERVER['HTTPS'].'<br>';
//echo $_SERVER['SERVER_PORT'].'<br>';

// Делаем левую сторону - для диалогов

?>
<div id="left">
<?php

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
        <input id="send-msg"  type="button" value="Отправить сообщение" />
        <input id="send-file" type="button" value="Отправить 2xbigtext" />
<?php
        
        
echo '<input id="send-img" type="button" value="Send IMG" />';
        
        
?>
    </div>
    <div>
        <span>Информация</span>
        <div id="socket-info"></div>
    </div>
</div>
<?php

// Делаем правую сторону - для изображений
echo '<div id="right">';
$FileName='Kwinflat.jpg';
/*
$FileName=
       "data:image/jpeg;base64,/9j/4AAQSkZJRgABAgAAZABkAAD/7AARRHVja3kAAQA".
       "EAAAAPAAA/+4ADkFkb2JlAGTAAAAAAf/bAIQABgQEBAUEBgUFBgkGBQYJCwgGBggLD".
       "AoKCwoKDBAMDAwMDAwQDA4PEA8ODBMTFBQTExwbGxscHx8fHx8fHx8fHwEHBwcNDA0".
       "YEBAYGhURFRofHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fH".
       "x8fHx8fHx8f/8AAEQgAGABkAwERAAIRAQMRAf/EAIgAAAICAwEBAAAAAAAAAAAAAAU".
       "GAwQAAgcBCAEAAgMBAAAAAAAAAAAAAAAAAgMAAQQFEAACAQMDBAEDBAMBAAAAAAABA".
       "gMRBAUAIRIxQRMGFFEiB2FxMkKxUjNDEQABAwIEBQIGAwAAAAAAAAABABECEgMhMUE".
       "EUSIyExRh0fBxgZHBBaHhI//aAAwDAQACEQMRAD8A+mczm7DDWD3t65EakKqgVZnPR".
       "FH1Ol3bsbcapIoQMiwSnL+UvNLPHisNdXfx4+csz0SMbVO45bD66wy/Ygh4RJWgbZu".
       "osih98x8eHtby4t5lv7qNXGJhUzXQZ+i8F6V+rU21o8uIAfqOmqV2S/pxTFaTSTW8c".
       "skLQO6qzwvQshIqVJG1R+mtMS4SypjTVqlg6aiizp+uoolLPfk313EvJCBNezxP43S".
       "2QMA4NCvJiBt+msdzfW4mnMp8dvIh0In/ACrkI54qevXK2clALiQso5N0WvDjU9t9I".
       "l+wkA9BZGNuHZ063GZx8F7aWM0nG8vP+UAHJqAFiWp0G1KnW6V6IkInqKziBIJ0Cu8".
       "VJ3FfppqFYwQfcwH7nUUXtRSvb6aiiGZn1/GZlLdL+NpYraTzLEHKqzUK0YDqKHSrt".
       "mNxqtEcJmOS5/kvZEvs7devllx/q9q5t3e0jq0kkdCwZxsqg9eINO+uZfvxq7cuW36".
       "LVbtlqhjJOtlF6zgMLPkbNY47NUMst0h8jyU7lySzEnsTroQFu3CqPSs0qpSY5oFbe".
       "9Zpi0s9pGpyDKmCx24nepIaSU1I8fTegr21lG9kztjLpH5KcbAfPLNEYffLVhlp3gP".
       "wsYywi4Uk+a4NQ6IpHQHv9N9MO+iKiRhH+TwCHxyWGpQ2T2eTBRJkshFNLl88/K3xL".
       "y0SCGLvyK0RQpqSV701Qu0CsvVLR8lKKsBkNVawnvFx7FnLuxsbY2+HtozFLkZCVk+".
       "RIB4/GDt32B3PXbTLe57kmA5eKGVqkOc1pa4z070O2DzzSSzv93lmHmmNT1HFRx30s".
       "9nbnHqP3Riu7lkgd/7tBnPY4Hit5zhsNby38qulBJcKKRczUgKK7dyTpMt1G4amNMM".
       "fqiFoxDalTesPPjrW49y9sYxyScmtoyhM7NJtULuRVaJEg/rueumWgI/6zz9/jBVMv".
       "yRRmD3PO3Gax1muLFtBevVknLGdYaV5lV/h+zakd5M3BGln+/z9FRsRESXQLJn3X23".
       "OZfE2t1bRYGylSN3XlTlSvEuAC7Dqy9AdtDcFy9IgEUgq4mMA5GK6V4D8T4/M8vHw8".
       "tBWvGnKnSuuksqhytndXeNntbS5NnPMvBbkLzZAdiQKjenTQ3ImUSAWKKJAOKr471r".
       "E2OEhwyQiSzhXjSTdmY/ydj/sxJJOgFiFFBDhX3C76pVuPxaY5p7bHZOWHCX4Zb+wk".
       "ZnI25I0THuHA69u+s3gs4iWidE3yNSMVLJ+NphPaXseWuDk4SwuL1hV2jZOASMV4px".
       "WtNQ7IuDUatT8ZKd/RsFcu/QMclmFwh+BepKkouXLy/ctKkqzUqf86u5sYkCnlILup".
       "HcHXEKKT8ZY65y9rlMhe3F9PEpFysrfbM1QVqAQFRf9AKHR+ICQZF/yh7xZgGUvqfo".
       "KYYtJfXjZCQTvcwIQViSR/wD04EtWSm3I9O2rs7WguS6k7r5BlpmZr/P5K4wmOt/Ba".
       "QEQ5LKyoQwDDk0duWG7UPXtoLtVydIDAZy9lcGiHJx4e6LXPq2Pf1iXAWg+NbPF40Y".
       "bkHryberEkb6dPbxNugYBALhEqkKl9HvLm1s2u8xNNk7GRHtrrgviThtQQn7Saf2ap".
       "rpXikgPI1DI/wBI+8NBgth6OoyTzR3s0VpPEIrlUZvkTEmr+SYmv3Hrxoe3TQjZNcq".
       "EsGx4n5lWb7xZsVX9W/H9xioJbS/vvkY4XLXENlCDGjMSCpmNeT04j7a8f31dnamOB".
       "PLwVTvA5DFOX9q79dbUhf/Z";
*/
echo '<span id="rem">Привет!</span><br>';
echo '<img id="imgCard" src="'.$FileName.'">';
echo '</div>';



// <!-- --> *****************************************  wsTve/LeadSocket.php ***

