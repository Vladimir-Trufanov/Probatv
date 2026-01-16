// PHP7/HTML5, EDGE/CHROME                                   *** DoorTry.js ***

// ****************************************************************************
// * doortry.ru    Сайт сбора сообщений об ошибках/исключениях и формирования *
// *         страницы с выводом сообщений, а также комментариев для PHP5-PHP7 *
// *                          (функциональные модули обеспечения сайта на js) *
// ****************************************************************************

//                                                   Автор:       Труфанов В.Е.
//                                                   Дата создания:  03.06.2019
// Copyright © 2019 tve                              Посл.изменение: 29.11.2019

// Обеспечить горизонтальную прокрутку кодов и сверху, и снизу
// https://www.it-swarm.net/ru/javascript/gorizontalnaya-polosa-prokrutki-sverhu-i-snizu-tablicy/970615983/
// https://github.com/avianey/jqDoubleScroll

$(document).ready(function()
{
   // Инициируем двойную прокрутку с css-классом CodeText
   $('.CodeText').doubleScroll({resetOnWindowResize:true});
   // Инициируем реакцию кнопки c идентификатором button
   $("#button").button();

   
   /*
   //$.getJSON('get-info.php').success(function(data)
   //$.get('get-info.php',function(data)
   { // ajax-запрос, данные с сервера запишутся в переменную 
      
      
      arr=JSON.parse(data);
      htmlstr='<table>';
      for (var i=0; i<arr.length; i++)
      {  // цикл по сотрудникам
         htmlstr+='<tr>';
         htmlstr+='<td>'+arr[i].fio+'</td>';      // первая колонка - ФИО
         htmlstr+='<td>'+arr[i].birthday+'</td>'; // вторая колонка - Дата рождения
         htmlstr+='</tr>';
      }
      htmlstr='</table>';
      $('div.info').html(htmlstr); // в div с классом info выводим получившуюся таблицу с данными
      console.log('data: '+data);
      console.log('data: '+arr[1].fio+arr[i].birthday);
      
   });
   */


   // закрыто до разборки 27.01.2020
   /*
   $.getJSON('get-info.php','contact=12',function(data)
   { // ajax-запрос, данные с сервера запишутся в переменную
      console.log('data: '+data.brat.fio);
   });
   
   $.get('get-info.php',function(data)
   { // ajax-запрос, данные с сервера запишутся в переменную 
      console.log('data: '+data);
      var arr=JSON.parse(data);
      //alert(arr.brat.fio);
      console.log('brat: '+arr.brat.fio);
      
      var htmlstr='<table>'; i=0;
      $.each(arr,function(rod,rodinfo)
      {  // цикл по сотрудникам
         console.log('i='+i+': '+rod+'-'+rodinfo.fio+'-'+rodinfo.birthday);
         htmlstr+='<tr>';
         htmlstr+='<td>'+rodinfo.fio+'</td>';      // первая колонка - ФИО
         htmlstr+='<td>'+rodinfo.birthday+'</td>'; // вторая колонка - Дата рождения
         htmlstr+='</tr>';
         i++;
      });
      htmlstr+='</table>';
      //htmlstr='table';
      $('div.info').html(htmlstr); // в div с классом info выводим получившуюся таблицу с данными
   });
   */
});

// ************************************************************* DoorTry.js ***
                             