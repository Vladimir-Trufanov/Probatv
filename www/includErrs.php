<?php
// PHP7/HTML5, EDGE/CHROME                               *** includErrs.php ***

// ****************************************************************************
// * doortry.ru                        Вывести сообщение об ошибке/исключении *
// ****************************************************************************

//                                                   Автор:       Труфанов В.Е.
//                                                   Дата создания:  10.04.2019
// Copyright © 2019 tve                              Посл.изменение: 12.05.2019


// Разработка 
// 
// Для того, чтобы видеть все возможные ошибки во время разработки, 
// настройте следующие параметры в вашем php.ini: 
// display_errors = On
// display_startup_errors = On
// error_reporting = -1
// log_errors = On

// Продакшн 
//
// Чтобы спрятать все ошибки вашей среды во время продакшна, 
// настройте ваш php.ini следующим образом: 
// display_errors = Off
// display_startup_errors = Off
// error_reporting = E_ALL
// log_errors = On
//
// С этими настройками в продакшне, ошибки всё также будут записываться в лог 
// ошибок веб сервера, но не будут показаны пользователю. Для подробной 
// информации об этих настройках, смотрите руководство PHP: 

// *** E_ERROR *** остановка программы
// ---------------------------------------------------------------------- 1 ---
//$str='try'; $str[]=4;
// [7] Fatal error: Uncaught Error: 
//     [] operator not supported for strings in C:\DoorTry\www\includErrs.php:99 
//     Stack trace: #0 C:\DoorTry\www\index.php(99): require_once() #1 {main} thrown in C:\DoorTry\www\includErrs.php on line 99
/*
-----------------------------
[] operator not supported for strings

File: C:\DoorTry\www\includErrs.php
Line: 28

Error:
#0 C:\DoorTry\www\Main.php(4): require_once()
#1 C:\DoorTry\www\index.php(24): require_once('C:\\DoorTry\\www\\...')
#2 {main}
-----------------------------
*/
// [5] Fatal error: 
//     [] operator not supported for strings in Z:\home\Proba\www\includErrs.php on line 99
// ---------------------------------------------------------------------- 2 ---
//split(',', 'a,b');
// [7] Fatal error: Uncaught Error: 
//     Call to undefined function split() in C:\DoorTry\www\includErrs.php:99
//     Stack trace: #0 C:\DoorTry\www\index.php(99): require_once() #1 {main} thrown in C:\DoorTry\www\includErrs.php on line 99
// [5] Deprecated: Function split() is deprecated in Z:\home\Proba\www\includErrs.php on line 99
 
// *** E_WARNING *** продолжение работы
// ---------------------------------------------------------------------- 3 ---
//fopen("spoon", "r");
// [7] Warning: fopen(spoon):                  
//     failed to open stream: No such file or directory in C:\DoorTry\www\includErrs.php on line 99
// [5] Warning: fopen(spoon) [function.fopen]: 
//     failed to open stream: No such file or directory in Z:\home\Proba\www\includErrs.php on line 99
// ---------------------------------------------------------------------- 4 ---
//include_once 'not-exists.php';
// [7] Warning: include_once(not-exists.php): failed to open stream: 
//     No such file or directory in C:\DoorTry\www\includErrs.php on line 99
//     Warning: include_once(): Failed opening 'not-exists.php' for inclusion 
//     (include_path='.;C:\php\pear') in C:\DoorTry\www\includErrs.php on line 99
// [5] Warning: include_once(not-exists.php) [function.include-once]: failed to open stream: 
//     No such file or directory in Z:\home\Proba\www\includErrs.php on line 99
//     Warning: include_once() [function.include]: Failed opening 'not-exists.php' for inclusion 
//     (include_path='.;C:\php\pear') in Z:\home\Proba\www\includErrs.php on line 99
// ---------------------------------------------------------------------- 5 ---
$a=0; $a=1/$a;
// [7] Warning: Division by zero in C:\DoorTry\www\includErrs.php on line 99
// [5] Warning: Division by zero in Z:\home\Proba\www\includErrs.php on line 99
// ---------------------------------------------------------------------- 6 ---
//$a='три'; $a=1+$a;
// [7] Warning: A non-numeric value encountered in C:\DoorTry\www\includErrs.php on line 99
// [5] Ошибок не нашел, просто сложил один с нулём и получил 1

// *** E_PARSE *** остановка программы
// ---------------------------------------------------------------------- 7 ---
//echo "Hi" echo "Hello";
// [7] Parse error: syntax error, 
//     unexpected 'echo' (T_ECHO), 
//     expecting ',' or ';' in C:\DoorTry\www\includErrs.php on line 99
// [5] Parse error: syntax error, 
//     unexpected T_ECHO, 
//     expecting ',' or ';' in Z:\home\Proba\www\includErrs.php on line 99

// *** E_NOTICE *** продолжение работы 
// ---------------------------------------------------------------------- 8 ---
//echo '***'.$a.'***';
// [7] Notice: Undefined variable: a in C:\DoorTry\www\includErrs.php on line 99
// [5] Ошибок не нашел, просто вывел пустое значение переменной
// ---------------------------------------------------------------------- 9 ---
//echo UNKNOWN_CONSTANT;
// [7] Notice: Use of undefined constant UNKNOWN_CONSTANT - 
//     assumed 'UNKNOWN_CONSTANT' in C:\DoorTry\www\includErrs.php on line 99
// [5] Ошибок не нашел, просто вывел наименование константы

// *** E_CORE_ERROR *** 

// *** E_CORE_WARNING *** 

// *** E_COMPILE_ERROR *** остановка программы 
// --------------------------------------------------------------------- 10 ---
//require_once 'not-exists.php';
// [7] Warning: require_once(not-exists.php): failed to open stream: 
//     No such file or directory in C:\DoorTry\www\includErrs.php on line 99
//     Fatal error: require_once(): Failed opening required 'not-exists.php' 
//     (include_path='.;C:\php\pear') in C:\DoorTry\www\includErrs.php on line 99
// [5] Warning: require_once(not-exists.php) [function.require-once]: failed to open stream: 
//     No such file or directory in Z:\home\Proba\www\includErrs.php on line 99
//     Fatal error: require_once() [function.require]: Failed opening required 'not-exists.php' 
//     (include_path='.;C:\php\pear') in Z:\home\Proba\www\includErrs.php on line 99

// *** E_COMPILE_WARNING *** 

// *** E_USER_ERROR *** остановка программы 
// --------------------------------------------------------------------- 11 ---
function ReadFileConfig($FileName)
{
   $TypeExp='E_USER_ERROR';
   if (!file_exists($FileName))
   {
      throw new $TypeExp("[ReadFileConfig] Конфигурационный файл не найден");
   }
}
//ReadFileConfig("config.php");

// *** E_USER_WARNING *** продолжение работы 
// --------------------------------------------------------------------- 12 ---
function PrintAge($age)
{
   $age = intval($age);
   if ($age < 0)
   {
      trigger_error(
         "Функция PrintAge(): ".
         "возраст не может быть ".
         "отрицательным", 
      E_USER_WARNING);
   }
   echo "Возраст составляет: $age<br>";
}
//PrintAge(-10);

// *** E_USER_NOTICE *** 

// *** E_STRICT *** 
//    Это ошибки, которые научат вас писать код правильно, чтобы 
// не было стыдно, тем более IDE вам эти ошибки сразу показывают. Вот например, 
// если вызвали не статический метод как статику, то код будет работать, 
// но это как-то неправильно, и возможно появление серьёзных ошибок, 
// если в дальнейшем метод класса будет изменён, и появится обращение к $this:

// *** E_RECOVERABLE_ERROR *** 

// *** E_DEPRECATED *** продолжение работы 
// --------------------------------------------------------------------- 13 ---
// PHP будет ругаться, если вы используете устаревшие функции (т.е. те, что 
// помечены как deprecated, и в следующем мажорном релизе их не будет):
//split(',', 'a,b');
// [7] Fatal error: Uncaught Error: 
//     Call to undefined function split() in C:\DoorTry\www\includErrs.php:99
//     Stack trace: #0 C:\DoorTry\www\index.php(99): require_once() #1 {main} thrown in C:\DoorTry\www\includErrs.php on line 99
// [5] Deprecated: Function split() is deprecated in Z:\home\Proba\www\includErrs.php on line 99

// *** Пример, где есть трассировка и в PHP7, и в PHP5 ***
// --------------------------------------------------------------------- 14 ---

class A
{
   protected $_b;
   public function __construct() {$this->_b = new B();}
   public function run() {$this->_b->doSomething();}
}
class B
{
   protected $_c;
   public function __construct() {$this->_c = new C();}
   public function doSomething() {$this->_c->doException();}
}
class C
{
   public function doException()
   {throw new Exception('Error in method ' . __METHOD__ . ' !');}
}
//$a=new A(); $a->run();

// [7] Fatal error: Uncaught Exception: Error in method C::doException ! 
//     in C:\DoorTry\www\includErrs.php:99 
//     Stack trace: #0 C:\DoorTry\www\includErrs.php(99): C->doException() 
//     #1 C:\DoorTry\www\includErrs.php(99): B->doSomething() 
//     #2 C:\DoorTry\www\includErrs.php(99): A->run() 
//     #3 C:\DoorTry\www\index.php(99): require_once('C:\\DoorTry\\www\\...') 
//     #4 {main} thrown in C:\DoorTry\www\includErrs.php on line 99
// [5] Fatal error: Uncaught exception 'Exception' with message 'Error in method C::doException !'
//     in Z:\home\Proba\www\includErrs.php:99
//     Stack trace: #0 Z:\home\Proba\www\includErrs.php(99): C->doException() 
//     #1 Z:\home\Proba\www\includErrs.php(99): B->doSomething() 
//     #2 Z:\home\Proba\www\includErrs.php(99): A->run() 
//     #3 Z:\home\Proba\www\index.php(99): require_once('Z:\home\Proba\w...') 
//     #4 {main} thrown in Z:\home\Proba\www\includErrs.php on line 99

// *** Пример, где есть блокировка трассироваи ***
// --------------------------------------------------------------------- 15 ---
//filemtime("spoon");
//@filemtime("spoon");

// *** E_USER_DEPRECATED *** продолжение работы 
// --------------------------------------------------------------------- 16 ---
require_once $SiteHost."/TPhpPrown/MakeRegExp.php";
$UserAgent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) ".
    "Chrome/72.0.3626.96 Chrome/72.0.3626.96 Safari/537.36";
$pattern="/Chrome/u";
//$value=\prown\MakeRegExp($pattern,$UserAgent,$matches,false);

// ********************************************************* includErrs.php ***
