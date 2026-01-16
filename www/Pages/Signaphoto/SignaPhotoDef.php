<?php
// PHP7/HTML5, EDGE/CHROME                            *** SignaPhotoDef.php ***

// ****************************************************************************
// * SignaPhoto       Совместные определения(переменные) для модулей PHP и JS *
// ****************************************************************************

//                                                   Автор:       Труфанов В.Е.
//                                                   Дата создания:  10.07.2021
// Copyright © 2021 tve                              Посл.изменение: 02.03.2022


// Определяем полный путь каталога хранения изображений и
// его url-аналог для связывания с разметкой через кукис
define ("imgDir",               $_SERVER['DOCUMENT_ROOT'].'/Temp'); 
define ("sceDir",               $_SERVER['DOCUMENT_ROOT'].'/Signaphoto'); 

// Определения к сценариям
define ("ohInfo",               "Info");  // Id дива информационных сообщений
define ("ohLeftTop",            "Левый верхний угол");  
define ("ohRightTop",           "Правый верхний угол");  
define ("ohRightBottom",        "Правый нижний угол");  
define ("ohLeftBottom",         "Левый нижний угол");  
// Определения переключателя "сохранять пропорции подписи"
define ("ohMaintainTrue",       "Да, сохранять");  
define ("ohMaintainFalse",      "Не сохранять"); 

define ("oriLandscape", 'landscape');  // Ландшафтное расположение устройства
define ("oriPortrait",  'portrait');   // Портретное расположение устройства

// Определения сообщений для PHP
define ("ajCopyImageNotCreate", "Не создана копия оригинального изображения для подписи"); 
define ("ajFailedResizedStamp", "Не удалось сохранить штамп в измененном размере");
define ("ajImageNotBuilt",      "Не строится изображение для подписи");
define ("ajInvalidBuilt",       "Неверный тип файла для построения изображения");  
define ("ajIsFreshStamp",       "На изображение наложена свежая подпись");
define ("ajMustTransparentPng", "Изображение подписи должно быть с прозрачным фоном и типа 'png'");
define ("ajNameFileNoMatchUrl", "Имя файла изображения со штампом не соответствует url-адресу");
define ("ajPlaceIsPossible",    "Размещение штампа на изображении возможно");
define ("ajStampBeyondBottom",  "Штамп зашел за нижний край изображения");
define ("ajStampBeyondLeft",    "Штамп зашел за левый край изображения");
define ("ajStampBeyondRight",   "Штамп зашел за правый край изображения");
define ("ajStampBeyondTop",     "Штамп зашел за верхний край изображения");
define ("ajStampNotBuilt",      "Не строится изображение штампа - водяного знака");
define ("ajStampPlaceDetermin", "Размещение штампа определено успешно");
define ("ajSuccess",            "Функция/процедура выполнена успешно");     
define ("ajTransparentSuccess", "Преобразование к прозрачному виду успешно");
define ("ajUndeletionOldFiles", "Ошибка удаления старых файлов");

// ****************************************************************************
// *         Подключить межязыковые (PHP-JScript) определения внутри HTML     *
// ****************************************************************************
function DefinePHPtoJS()
{
   // Переменные JavaScript, соответствующие определениям сообщений в PHP
   $define=
   '<script>'.
   'imgDir="'              .imgDir.'";'.
   'sceDir="'              .sceDir.        '";'.

   'ajCopyImageNotCreate="'.ajCopyImageNotCreate.'";'.
   'ajFailedResizedStamp="'.ajFailedResizedStamp.'";'.
   'ajImageNotBuilt="'     .ajImageNotBuilt.     '";'.
   'ajInvalidBuilt="'      .ajInvalidBuilt.      '";'.
   'ajIsFreshStamp="'      .ajIsFreshStamp.      '";'.
   'ajMustTransparentPng="'.ajMustTransparentPng.'";'.
   'ajNameFileNoMatchUrl="'.ajNameFileNoMatchUrl.'";'.
   'ajPlaceIsPossible="'   .ajPlaceIsPossible.   '";'.
   'ajStampBeyondBottom="' .ajStampBeyondBottom. '";'.
   'ajStampBeyondLeft="'   .ajStampBeyondLeft.   '";'.
   'ajStampBeyondRight="'  .ajStampBeyondRight.  '";'.
   'ajStampBeyondTop="'    .ajStampBeyondTop.    '";'.
   'ajStampNotBuilt="'     .ajStampNotBuilt.     '";'.
   'ajStampPlaceDetermin="'.ajStampPlaceDetermin.'";'.
   'ajSuccess="'           .ajSuccess.           '";'.
   'ajTransparentSuccess="'.ajTransparentSuccess.'";'.
   'ajUndeletionOldFiles="'.ajUndeletionOldFiles.'";'.
   
   'ohMaintainTrue="'      .ohMaintainTrue.      '";'.  
   'ohMaintainFalse="'     .ohMaintainFalse.     '";'.  
   
   'oriLandscape="'        .oriLandscape. '";'.
   'oriPortrait="'         .oriPortrait.  '";'.

   '</script>';
   echo $define;

   // Переменные JavaScript, соответствующие определениям объектов HTML
   $odefine=
   '<script>'.
   'const ohInfo="'   .ohInfo. '";'. 
   '</script>';
   echo $odefine;
}   

// ****************************************************** SignaPhotoDef.php ***
