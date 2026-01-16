<?php
// PHP7/HTML5, EDGE/CHROME                               *** ViewEnviron.php ***

// ****************************************************************************
// *                     Вывести технологическую информацию                   *
// ****************************************************************************

//                                                   Автор:       Труфанов В.Е.
//                                                   Дата создания:  09.11.2021
// Copyright © 2021 tve                              Посл.изменение: 09.11.2021

function EnviView()
{
   echo $_SERVER['DOCUMENT_ROOT'].'/iniWorkSpace.php'.'<br>';
   global $SiteRoot;   if (isset($SiteRoot))   echo '$SiteRoot='.$SiteRoot.'<br>';
   global $SiteAbove;  if (isset($SiteAbove))  echo '$SiteAbove='.$SiteAbove.'<br>';
   global $SiteHost;   if (isset($SiteHost))   echo '$SiteHost='.$SiteHost.'<br>';
   global $SiteDevice; if (isset($SiteDevice)) echo '$SiteDevice='.$SiteDevice.'<br>';
   echo '$_SERVER["SERVER_NAME"]='.$_SERVER["SERVER_NAME"].'<br>';
   // Показываем, какие графические форматы поддерживаются
   echo '<pre>';
   print_r(gd_info());
   echo '</pre>';
}

function DebugView($s_Orient)
{
   echo '***<br>';
   echo 'Всем привет!<br>';
   EnviView();
   echo 'Ориентация: '.$s_Orient.'<br>';
   echo "Вы обновили эту страницу ".$_SESSION['Counter']." раз. ";
   prown\ViewGlobal(avgCOOKIE);
   prown\ViewGlobal(avgSESSION);
   echo '<br>';
   echo '<pre>';
   //var_dump(realpath_cache_get());
   print_r(realpath_cache_get());
   echo '</pre>';
}
// *** <!-- --> ******************************************* ViewEnviron.php ***
