<?php

// PHP7/HTML5, EDGE/CHROME                       *** j_getLastStateMess.php ***

// ****************************************************************************
// * KwinFlat                     Получить последнее json-сообщение на State  *
// *                                                                          *
// * v1.0.3, 13.12.2025                            Автор:       Труфанов В.Е. *
// * Copyright © 2024 tve                          Дата создания:  13.11.2024 *
// ****************************************************************************
// Извлекаем пути к библиотекам прикладных функций и классов
define ("pathPhpPrown",$_POST['pathPrown']);
define ("pathPhpTools",$_POST['pathTools']);
define ("SiteHost",    $_POST['sh']);
// Подгружаем нужные модули библиотек
require_once pathPhpPrown."/CommonPrown.php";
// Подключаем объект для работы с базой данных моего хозяйства
require_once "Common.php";  
require_once "TKvizzyMaker/KvizzyMakerClass.php";
$Kvizzy=new ttools\KvizzyMaker(SiteHost);
// Подключаемся к базе данных
$pdo=$Kvizzy->BaseConnect();
// Выбираем параметры ответа
$table=$Kvizzy->SelectLastMess($pdo);
$myTime=$table['myTime']; 
$myDate=$table['myDate']; 
$ctrl=$table['idctrl']; 
$num=$table['num']; 
$cycle=$table['cycle']; 
$sjson=$table['sjson'];
// Возвращаем сообщение
$message='{"myTime":'.$myTime.',"myDate":"'.$myDate.'","ctrl":'.$ctrl.',"num":'.$num.',"cycle":'.$cycle.', "sjson":'.$sjson.'}';
$message=\prown\makeLabel($message,'ghjun5','ghjun5');
echo $message;
exit;

// ************************************************* j_getLastStateMess.php ***
