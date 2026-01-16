<?php
// PHP7/HTML5, EDGE/CHROME                            *** SignaPhotoImg.php ***

// ****************************************************************************
// * SignaPhoto                          Подготовить и обработать изображения *
// ****************************************************************************

//                                                   Автор:       Труфанов В.Е.
//                                                   Дата создания:  25.11.2021
// Copyright © 2021 tve                              Посл.изменение: 11.02.2022

// ****************************************************************************
// *                      Подключиться к файлам изображений                   *
// ****************************************************************************
function ConnectImgFiles(&$c_FileImg,&$c_FileStamp,&$c_FileProba)
{
   // Обеспечиваем инициацию изображений при первом запуске в браузере
   $c_FileImg=prown\MakeCookie('FileImg','images/iphoto.jpg',tStr,true);
   $c_FileStamp=prown\MakeCookie('FileStamp','images/istamp.png',tStr,true);
   $c_FileProba=prown\MakeCookie('FileProba','images/iproba.jpeg',tStr,true);
   // Проверяем существование изображений, инициируем при отсутствии
   if (@!fopen($c_FileImg,"r"))   $c_FileImg=prown\MakeCookie('FileImg','images/iphoto.jpg',tStr); 
   if (@!fopen($c_FileStamp,"r")) $c_FileStamp=prown\MakeCookie('FileStamp','images/istamp.png',tStr); 
   if (@!fopen($c_FileProba,"r")) $c_FileProba=prown\MakeCookie('FileProba','images/iproba.jpeg',tStr); 
}
// ****************************************************************************
// *                       Выделить расширение в имени файла                  *
// ****************************************************************************
function get_file_extension($file_name)
{
   return substr(strrchr($file_name,'.'),1);
}
// ****************************************************************************
// *            Загрузить фотографию для подписи и сделать её копию,          *
// *                    на которой будет размещена подпись                    *
// ****************************************************************************
function LoadImg()
{ 
   echo '
      <div id="InfoLead">
      <form action="SignaPhoto.php" method="POST" enctype="multipart/form-data"> 
      <input type="hidden" name="MAX_FILE_SIZE" value="3000024"/> 
      <input type="file"   id="my_hidden_file" 
         accept="image/jpeg,image/png,image/gif" 
         name="loadimg" onchange="alf2LoadFile();"/>  
      <input type="submit" id="my_hidden_load" value="">  
      </form>
      </div>
   ';
   echo '
      <button id="bLoadImg"  class="navButtons" onclick="alf1FindFile()"  
      title="Загрузить изображение">
      <i id="iLoadImg" class="fa fa-file-image-o fa-3x" aria-hidden="true"></i>
      </button>
   ';
}
// ****************************************************************************
// *            Загрузить образец для подписания фотографии                   *
// ****************************************************************************
function LoadStamp()
{ 
   echo '
      <div id="StampLead">
      <form action="SignaPhoto.php" method="POST" enctype="multipart/form-data"> 
      <input type="hidden" name="MAX_FILE_SIZE" value="3000024"/> 
      <input type="file"   id="my_shidden_file" accept="image/png" 
      name="loadstamp" onchange="alf2sLoadFile();"/>  
      <input type="submit" id="my_shidden_load" value="">  
      </form>
      </div>
   ';
   echo '
      <button id="bLoadStamp" class="navButtons" onclick="alf1sFindFile()"  
      title="Загрузить образец подписи">
      <i id="iLoadStamp" class="fa fa-pencil-square-o fa-3x" aria-hidden="true"></i>
      </button>
  ';
}
// ****************************************************************************
// *     Разместить изображение по центру дива: cDiv - идентификатор дива,    *
// *                   cImg - идентификатор изображения,                      *
// *  wImg - реальная ширина изображения, hImg - реальная высота изображения  *
// *        mAligne - первичное выравнивание ('по ширине','по высоте'),       *
// *    perWidth - процент ширины изображения от ширины дива (или высоты),    *
// *
// ****************************************************************************
function MakeImgOnDiv($cDiv,$cImg,$c_FileImg,$perWidth)
{
   // Определяем реальную ширину и высоту изображения
   $a=getimagesize($c_FileImg);
   $wImg=$a[0]; $hImg=$a[1];
   ?> <script>
   cDiv="<?php echo $cDiv; ?>";
   cImg="<?php echo $cImg; ?>";
   wImg="<?php echo $wImg; ?>";
   hImg="<?php echo $hImg; ?>";
   perWidth="<?php echo $perWidth; ?>";
   // Определяем способ выравнивания изображения диву
   // ('по ширине','по высоте')
   alignPhoto=getAlignImg(cDiv,cImg,wImg,hImg);
   // Расчитываем выравнивание и устанавливаем CSS
   aCalcPicOnDiv=CalcPicOnDiv(cDiv,cImg,wImg,hImg,alignPhoto,perWidth)
   $("#"+cImg).css("width",String(aCalcPicOnDiv.widthImg)+'px');
   $("#"+cImg).css("height",String(aCalcPicOnDiv.heightImg)+'px');
   $("#"+cImg).css("margin-left",String(aCalcPicOnDiv.nLeft)+'px');
   $("#"+cImg).css("margin-top",String(aCalcPicOnDiv.nTop)+'px');
   </script> <?php
}
// ****************************************************************************
// *   Наложить изображения штампа на фотографию с учетом ширины фотографии   *
// *                 и смещения штампа от точка привязки                      *
// ****************************************************************************
function MakeStamp()
{
   echo '
     <div id="StampDo">
     <form action="SignaPhoto.php" method="POST">
     <input type="submit" id="my_Stamp_Do" name="Stamp" value="Do">
     </form>
     </div>
   ';
   echo '
     <button id="bSubscribe" class="navButtons" onclick="alf1MakeStamp()"   
     title="Подписать">
     <i id="iSubscribe" class="fa fa-user-plus fa-3x" aria-hidden="true"></i>
     </button>
    ';
}
// ****************************************************************************
// *                  Изменить настройки подписания фотографии                *
// ****************************************************************************
function Tunein()
{ 
   echo '
     <div id="TuneIn">
     <form action="SignaPhoto.php" method="POST">
     <input type="submit" id="my_Tune_In" name="Tune" value="In">
     </form>
     </div>
   ';
   echo '
     <button id="bTunein" class="navButtons" onclick="alf1Tunein()"   
     title="Выполнить настройки">
     <i id="iTunein" class="fa fa-cog fa-3x" aria-hidden="true"></i>
     </button>
    ';
}
// ****************************************************************************
// *                      Выйти на главную страницу сайта                     *
// ****************************************************************************
function Home()
{
   echo '
     <div> <form> <input id="my_Home"> </form> </div>
     <button id="bHome" class="navButtons" onclick="alf1Home()"   
     title="Выйти на главную страницу">
     <i id="iTunein" class="fa fa-home fa-3x" aria-hidden="true"></i>
     </button>
    ';
}
// ****************************************************************************
// *                  Вывести загруженное изображение для подписи             *
// ****************************************************************************
function ViewPhoto($c_FileImg)
{
   echo '<img id="pic" src="'.$c_FileImg.'"'.' alt="'.$c_FileImg.'"'.
     ' title="Загруженное изображение">';
   MakeImgOnDiv('Photo','pic',$c_FileImg,94);
}
// ****************************************************************************
// *            Вывести изображение для подписи или уже с подписью            *
// ****************************************************************************
function ViewProba($c_FileProba,$RemoteAddr,
   $c_PointCorner,$c_PerSizeImg,$c_PerMargeWidth,$c_PerMargeHight,$c_MaintainProp,
   $c_FileImg,$c_FileStamp,$c_Orient)
{  
   /*
   clearstatcache(true,'C:\TPhpTools/TPhpTools/TDeviceOrientater/DeviceOrientaterClass.php');    
   echo '<pre>';
   var_dump(realpath_cache_get());
   echo "</pre>";
   */
   
   echo '<img id="picProba" src="'.$c_FileProba.'"'.' alt="'.$c_FileProba.'"'.
     ' title="Подписанное изображение">';
   MakeImgOnDiv('Proba','picProba',$c_FileProba,94);
   //stat($c_FileProba);
   //clearstatcache(true); 
   /* 
   if (file_exists($c_FileProba)) {
      echo "Файл $c_FileProba существует";
   } else {
      echo "Файл $c_FileProba не существует";
   }
   */
     
      
   /*
   echo '$c_FileImg='.$c_FileImg.'<br>';
   echo '$c_FileStamp='.$c_FileStamp.'<br>';
   echo '$c_FileProba='.$c_FileProba.'<br>';
   echo '$c_OrientIC='.$c_Orient.'<br>';
   */
   /*
   prown\ViewGlobal(avgCOOKIE);
   echo '<pre>';
   echo '*** browscap='.ini_get('browscap').' ***<br>';
   $browser = get_browser(null,true);
   print_r($browser);
   echo "</pre>";
   */
}
// ****************************************************************************
// *                     Вывести загруженный образец подписи                  *
// ****************************************************************************
function ViewStamp($c_FileStamp)
{
   echo '<img id="picStamp" src="'.$c_FileStamp.'"'.' alt="'.$c_FileStamp.'"'.
     ' title="Образец подписи">';
   MakeImgOnDiv('Stamp','picStamp',$c_FileStamp,50);
}
// ****************************************************************************
// *                     Изменить настройки для подписи                       *
// ****************************************************************************
function ischecked($c_PointCorner,$isPointCorner)
{
   $Result='';
   if ($c_PointCorner==$isPointCorner) $Result=' checked';
   return $Result;
}
function isMaintainProp($c_MaintainProp)
{
   if ($c_MaintainProp==ohMaintainTrue) $Result=' value="'.ohMaintainTrue.'" checked ';
   else $Result=' value="'.ohMaintainFalse.'" ';
   return $Result;
}

function ViewTuneIn($c_PerSizeImg,$c_PointCorner,$c_PerMargeWidth,$c_PerMargeHight,$c_MaintainProp,$_Orient,$SiteDevice)
{
   if (($_Orient==oriLandscape)and($SiteDevice==Mobile))
      ViewTuneInML($c_PerSizeImg,$c_PointCorner,$c_PerMargeWidth,$c_PerMargeHight,$c_MaintainProp);
   else 
      ViewTuneInPP($c_PerSizeImg,$c_PointCorner,$c_PerMargeWidth,$c_PerMargeHight,$c_MaintainProp);
}

function ViewTuneInPP($c_PerSizeImg,$c_PointCorner,$c_PerMargeWidth,$c_PerMargeHight,$c_MaintainProp)
{
   // Выводим форму в разметку
   echo '<div  id="ViewTuneIn">';
   echo '
      <form name="test" method="POST" action="SignaPhoto.php">
      
      <span class="PerSizeImg" id="PerSizeImg"><b>Процент размера подписи к изображению:</b></span><br>
      <input name="PerSizeImg" class="Infield" value="'.$c_PerSizeImg.'"
      id="PerSizeInput" type="number" min="1" max="99" step="1">
      <span class="PerSizeImg">%</span><br><br>

      <b>Точка привязки подписи:</b>

      <input name="PointCorner" class="checkbox" id="r1" type="radio" value="'.ohLeftTop.'"'.
      ischecked($c_PointCorner,ohLeftTop).'>'.
      '<label for="r1" class="checkbox-label"> 
      <span class="on">'.ohLeftTop.'</span>
      <span class="off">'.ohLeftTop.'</span>
      </label>
                                                                             
      <input name="PointCorner" class="checkbox" id="r2" type="radio" value="'.ohRightTop.'"'.
      ischecked($c_PointCorner,ohRightTop).'>'.
      '<label for="r2" class="checkbox-label"> 
      <span class="on">'.ohRightTop.'</span>
      <span class="off">'.ohRightTop.'</span>
      </label>

      <input name="PointCorner" class="checkbox" id="r3" type="radio" value="'.ohRightBottom.'"'.
      ischecked($c_PointCorner,ohRightBottom).'>'.
      '<label for="r3" class="checkbox-label"> 
      <span class="on">'.ohRightBottom.'</span>
      <span class="off">'.ohRightBottom.'</span>
      </label>

      <input name="PointCorner" class="checkbox" id="r4" type="radio" value="'.ohLeftBottom.'"'.
      ischecked($c_PointCorner,ohLeftBottom).'>'.
      '<label for="r4" class="checkbox-label"> 
      <span class="on">'.ohLeftBottom.'</span>
      <span class="off">'.ohLeftBottom.'</span>
      </label>

      <b>Процент смещения подписи по ширине от точки привязки:</b><br>
      <input name="PerMargeWidth" class="Infield" value="'.$c_PerMargeWidth.'" 
         type="number" min="1" max="99" step="1">
      %<br>      

      <span class="PerMargeSpan"><b>Процент смещения подписи по высоте от точки привязки:</b></span><br>
      <input name="PerMargeHight" class="Infield" value="'.$c_PerMargeHight.'" 
         id="PerMargeHight" type="number" min="1" max="99" step="1">
      <span class="PerMargeSpan">%</span><br>      

      <b>Сохранять пропорции подписи:</b>

       <input name="MaintainCtrl" class="checkboxctrl"  id="MaintainCtrl" type="checkbox">'. 
      '<input name="MaintainProp" class="checkbox blue" id="MaintainProp" type="checkbox"'. 
         isMaintainProp($c_MaintainProp).
         'onchange="alf1MaintainProp()">'.
      '<label for="MaintainProp" class="checkbox-label">
      <span class="on">'.ohMaintainTrue.'</span>
      <span class="off">'.ohMaintainFalse.'</span>
      </label>

      <input type="submit" id="my_Tune_Submit"><br>
      <button id="btnTune" onclick="alfSubmitTunein()">Изменить настройки</button>
      </form>
   ';
   echo '</div>';
   ?> <script>
   alf1MaintainProp();
   </script> <?php
}
function ViewTuneInML($c_PerSizeImg,$c_PointCorner,$c_PerMargeWidth,$c_PerMargeHight,$c_MaintainProp)
{
   // Выводим форму в разметку
   echo '<div  id="ViewTuneIn">';
   echo '
      <form name="test" method="POST" action="SignaPhoto.php">
      
      <div id=percML>
      
      <span class="PerSizeImg" id="PerSizeImg"><b>Процент размера подписи к изображению:</b></span><br>
      <input name="PerSizeImg" class="Infield" value="'.$c_PerSizeImg.'"
      id="PerSizeInput" type="number" min="1" max="99" step="1">
      <span class="PerSizeImg">%</span><br><br>
 
      <b>Процент смещения подписи по ширине от точки привязки:</b><br>
      <input name="PerMargeWidth" class="Infield" value="'.$c_PerMargeWidth.'" 
         type="number" min="1" max="99" step="1">
      %<br>      

      <span class="PerMargeSpan"><b>Процент смещения подписи по высоте от точки привязки:</b></span><br>
      <input name="PerMargeHight" class="Infield" value="'.$c_PerMargeHight.'" 
         id="PerMargeHight" type="number" min="1" max="99" step="1">
      <span class="PerMargeSpan">%</span><br>      

      <input type="submit" id="my_Tune_Submit"><br>
      <button id="btnTune" onclick="alfSubmitTunein()">Изменить настройки</button>
      
      </div>
      
      <div id=checkML>

      <b>Точка привязки подписи:</b>
      <input name="PointCorner" class="checkbox" id="r1" type="radio" value="'.ohLeftTop.'"'.
      ischecked($c_PointCorner,ohLeftTop).'>'.
      '<label for="r1" class="checkbox-label"> 
      <span class="on">'.ohLeftTop.'</span>
      <span class="off">'.ohLeftTop.'</span>
      </label>

      <input name="PointCorner" class="checkbox" id="r2" type="radio" value="'.ohRightTop.'"'.
      ischecked($c_PointCorner,ohRightTop).'>'.
      '<label for="r2" class="checkbox-label"> 
      <span class="on">'.ohRightTop.'</span>
      <span class="off">'.ohRightTop.'</span>
      </label>

      <input name="PointCorner" class="checkbox" id="r3" type="radio" value="'.ohRightBottom.'"'.
      ischecked($c_PointCorner,ohRightBottom).'>'.
      '<label for="r3" class="checkbox-label"> 
      <span class="on">'.ohRightBottom.'</span>
      <span class="off">'.ohRightBottom.'</span>
      </label>

      <input name="PointCorner" class="checkbox" id="r4" type="radio" value="'.ohLeftBottom.'"'.
      ischecked($c_PointCorner,ohLeftBottom).'>'.
      '<label for="r4" class="checkbox-label"> 
      <span class="on">'.ohLeftBottom.'</span>
      <span class="off">'.ohLeftBottom.'</span>
      </label>

      <b>Сохранять пропорции подписи:</b>
       <input name="MaintainCtrl" class="checkboxctrl"  id="MaintainCtrl" type="checkbox">'. 
      '<input name="MaintainProp" class="checkbox blue" id="MaintainProp" type="checkbox"'. 
         isMaintainProp($c_MaintainProp).
         'onchange="alf1MaintainProp()">'.
      '<label for="MaintainProp" class="checkbox-label">
      <span class="on">'.ohMaintainTrue.'</span>
      <span class="off">'.ohMaintainFalse.'</span>
      </label>

      </div>
      </form>
   ';
   echo '</div>';
   ?> <script>
   alf1MaintainProp();
   </script> <?php
}
// ****************************************************** SignaPhotoImg.php ***
