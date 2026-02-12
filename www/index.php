<?php

// PHP7/HTML5, EDGE/CHROME                                    *** index.php ***

// ****************************************************************************
// * probatv.ru                   Решение всяких-разных проблем для PHP5-PHP7 *
// ****************************************************************************

// sla6en9edged                                      Автор:       Труфанов В.Е.
//                                                   Дата создания:  12.12.2024
// Copyright © 2024 tve                              Посл.изменение: 12.12.2024

// Инициализируем рабочее пространство: корневой каталог сайта и т.д.
require_once 'iniWorkSpace.php';
$_WORKSPACE=iniWorkSpace();

$SiteRoot     = $_WORKSPACE[wsSiteRoot];     // Корневой каталог сайта
$SiteAbove    = $_WORKSPACE[wsSiteAbove];    // Надсайтовый каталог
$SiteHost     = $_WORKSPACE[wsSiteHost];     // Каталог хостинга
$SiteDevice   = $_WORKSPACE[wsSiteDevice];   // 'Computer' | 'Mobile' | 'Tablet'
$SiteProtocol = $_WORKSPACE[wsSiteProtocol]; // 'HTTP' | 'HTTPS'

$urltxt=$_SERVER['REQUEST_URI'];

// Подключаем сайт сбора сообщений об ошибках/исключениях и формирования 
// страницы с выводом сообщений, а также комментариев для PHP5-PHP7
require_once $SiteHost."/TDoorTryer/DoorTryerPage.php";

try 
{
   // Проверяем информацию о текущей установленной GD библиотеке
   /*
   ?>
   <pre>
   <?php
   var_dump(gd_info());
   ?>
   </pre>
   <?php
   */
   
   // Проверяем, работает ли get-browser (2024-12-08 не работал в мастерхост)
   // $browser = get_browser(null, true);
   // print_r($browser);
   
   // Смотрим параметры PHP
   // phpinfo();
   
   // Показываем некоторые настройки PHP.INI
   /*
   echo '<pre>';
   echo 'display_errors        = '.ini_get('display_errors')."\n";
   echo 'error_reporting       = '.ini_get('error_reporting')."\n";
   echo 'register_globals      = '.(int) ini_get('register_globals')."\n";
   echo '---'."\n";
   echo 'memory_limit          = '.ini_get('memory_limit ')."\n";
   echo 'post_max_size         = '.ini_get('post_max_size')."\n";
   echo 'upload_max_filesize   = '.ini_get('upload_max_filesize')."\n";
   echo '---'."\n";
   echo 'post_max_size+1       = '.(rtrim(ini_get('post_max_size'),'KMG')+1)."\n";
   echo 'post_max_size (bytes) = '.return_bytes(ini_get('post_max_size'));
   echo '</pre>';
   */

   // Запускаем сценарий сайта
   // echo "Пробуем!";
   
   $page='/Pages/Signaphoto/SignaPhoto.php';
   Header("Location: http://".$_SERVER['HTTP_HOST'].$page,true);

   // Запускаем примеры ошибок и исключений
   // require_once $_SERVER['DOCUMENT_ROOT']."/MainDoorTry.php";
}
catch (E_EXCEPTION $e) 
{
   /**
    * ПОМНИТЬ(16.02.2019)! Если в коде сайта включается своя обработка исключений,
    * то управление выводом ошибок display_errors на сайте NIC.RU отключается и
    * работает только error_reporting (нужно разрешить обработку всех ошибок)
   **/
   // Подключаем обработку исключений верхнего уровня
   DoorTryPage($e);
}

function return_bytes($val)
{
   $val = trim($val);
   $num = (int) rtrim($val, 'KMG');
   $last = strtolower($val[strlen($val) - 1]);

   switch ($last) {
      // Модификатор 'G' доступен
      case 'g':
         $num = $num * 1024 * 1024 * 1024;
         break;
      case 'm':
         $num = $num * 1024 * 1024;
         break;
      case 'k':
         $num *= 1024;
         break;
   }
   return $num;
}


// ************************************************************** index.php ***
