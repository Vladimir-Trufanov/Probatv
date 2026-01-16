<?php
// PHP7/HTML5, EDGE/CHROME                              *** MainDoorTry.php ***

// ****************************************************************************
// * doortry.ru                        Отработать примеры ошибок и исключений *
// ****************************************************************************

//                                                   Автор:       Труфанов В.Е.
//                                                   Дата создания:  18.05.2019
// Copyright © 2019 tve                              Посл.изменение: 24.05.2019

?>
   <!DOCTYPE html>
   <html lang="ru">
   <head>
      <title>Коллекционер ошибок</title>
      <meta http-equiv="content-type" content="text/html; charset=utf-8"/>
      <meta name="description" content="DoorTry - коллекционер ошибок">
      <meta name="keywords" content="DoorTry - простой принцип программирования">
   </head>
   <body>

<?php
require_once "iGetAbove.php";
$SiteAbove = iGetAbove($SiteRoot);      // Надсайтовый каталог
$SiteHost = iGetAbove($SiteAbove);      // Каталог хостинга

echo 'Здравствуй, читатель!<br>';
require_once $SiteRoot."/includErrs.php";
echo 'Все 16 примеров с ошибками и исключениями просмотрены. До встречи!';

?>
   </body> 
   </html>
<?php
// ******************************************************** MainDoorTry.php ***
