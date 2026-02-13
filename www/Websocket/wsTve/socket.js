// PHP7/HTML5, EDGE/CHROME/YANDEX                      ***  wsTve/socket.js ***

// ****************************************************************************
// *             Обеспечить взаимодействие клиента с сокет-сервером           *
// ****************************************************************************

// v3.0.4, 13.02.2026                                 Автор:      Труфанов В.Е.
// Copyright © 2024 tve                               Дата создания: 18.03.2025

/**
 * Состояние соединения. Чтобы получить состояние соединения, существует 
 * дополнительное свойство socket.readyState со значениями:
 * 
 * 0 – «CONNECTING»: соединение ещё не установлено,
 * 1 – «OPEN»: обмен данными,
 * 2 – «CLOSING»: соединение закрывается,
 * 3 – «CLOSED»: соединение закрыто.
 * 
**/ 

var socket;

// ****************************************************************************
// *                       Показать сообщение в #socket-info                  *
// ****************************************************************************
function showMessage(message) 
{
  // Cоздаём новый элемент div в DOM (Document Object Model). В качестве параметра 
  // передаём имя тега, которое указывает тип создаваемого элемента (div).
  // Важно: метод создаёт элемент, но не добавляет его в DOM. После создания элемента 
  // можно настроить его свойства, добавить стили и атрибуты, а затем вставить в 
  // нужное место в DOM-дереве. Для добавления элемента в DOM используются методы 
  // appendChild() или insertBefore(). 
  let div = document.createElement('div');
  // Создаём и возвращаем текстовый узел с указанным текстом. Метод полезен, если 
  // нужно создать только текстовую часть, без HTML. Здесь message — строка, содержащая 
  // данные, которые будут помещены в текстовый узел. Обязательный параметр.
  // Важно: одного создания текстового узла недостаточно — его нужно добавить на 
  // веб-страницу. Для этого используют методы объекта Node, например appendChild 
  // (добавляет новый узел в конец коллекции дочерних узлов) или insertBefore 
  // (добавляет новый узел перед узлом). 
  div.appendChild(document.createTextNode(message));
  document.getElementById('socket-info').appendChild(div);
}
// ****************************************************************************
// *                           Загрузить Websocket Server                     *
// ****************************************************************************
function isSocket(ipath,iip,iport)
{
  getMems();
  console.log(iip+': '+ipath+': '+iport);
  showMessage('Включаем вэбсокет сервер!');
  // Указываем путь к вэбсокет серверу
  pathsocket=ipath;
  // Делаем запрос на запуск вэбсокет сервера 
  $.ajax({
    url: pathsocket,
    type: 'POST',
    data: {ip:iip,pport:iport},
    // Выводим ошибки при выполнении запроса в PHP-сценарии
    error: function (jqXHR,exception) 
    {
      console.log('Cокет-сервер завершил работу!');
    },
    // Обрабатываем ответное сообщение
    success: function(message)
    {
      console.log('message');
      console.log(message);
    }
  });
  getMems();
}
// ****************************************************************************
// *                           Загрузить Websocket Server                     *
// ****************************************************************************
function getMems()
{
  showMessage('Выбираем данные по памяти!');
  // Указываем путь к вэбсокет серверу
  // Выполняем запрос
  pathphp="j_getMems.php";
  // Делаем запрос на запуск вэбсокет сервера 
  $.ajax({
    url: pathphp,
    type: 'POST',
    data: {},
    // Выводим ошибки при выполнении запроса в PHP-сценарии
    error: function (jqXHR,exception) 
    {
      console.log('Ошибка запроса памяти!');
    },
    // Обрабатываем ответное сообщение
    success: function(message)
    {
      console.log(message);
      console.log('Данные по памяти выбраны');
    }
  });
  
// ****************************************************************************
// *                   Получить последнее json-сообщение на State             *
// ****************************************************************************
/*
function getLastStateMess(tickers)
{
  // Выводим в диалог предварительный результат выполнения запроса
  htmlText="Выбрать json-сообщение на State не удалось!";
  // Выполняем запрос
  pathphp="j_getLastStateMess.php";
  // Делаем запрос последнего json-сообщения на State 
  $.ajax({
    url: pathphp,
    type: 'POST',
    data: {pathTools:pathPhpTools,pathPrown:pathPhpPrown,sh:SiteHost},
    // Выводим ошибки при выполнении запроса в PHP-сценарии
    error: function (jqXHR,exception) {DialogWind(SmarttodoError(jqXHR,exception))},
    // Обрабатываем ответное сообщение
    success: function(message)
    {
      // Трассируем полный json-ответ
      //DialogWind(message);
      //console.log(message)
      //tickers.render('JSON.stringify(sjson)');
      
      // Вырезаем из запроса чистое сообщение
      let Fresh=FreshLabel(message);
      // Если чистое сообщение не вырезалось, считаем, что это ошибка и
      // диагностируем её
      if (Fresh=='NoFresh')
      {
        console.log(message);
        DialogWind(message);
      }
      // Иначе считаем, что ответ на запрос пришел и можно
      // парсить сообщение
      else 
      {
        messa=Fresh;
        // DialogWind(messa);
        // console.log(messa)
        
        // Строим try catch, чтобы поймать ошибку в JSON-ответе
        try 
        {
          parm=JSON.parse(messa);
          // Если ошибка SQL-запроса
          if (parm.cycle<0) 
          {
            if (parm.cycle==-1) 
              DialogWind(
              "Пересоздана таблица базы данных State.<br>"+
              "Сообщений от контроллера ещё не поступало!<br>"+
              "Можно проверить виртуальный контроллер.");
            else
              DialogWind(parm.cycle+': '+parm.sjson);
          }
          // Выводим результаты выполнения (параметры ответа)
          // (отрабатываем распарсенный ответ)
          else
          {
            // Трассируем чистое сообщение, без метки
            // {"myTime":1736962888,"myDate":"25-01-15 08:41:28","cycle":195, "sjson":{"led33":[{"status":"inLOW"}]}}
            // DialogWind(messa);
            ctrl=parm.ctrl;
            $('#ctrl').html("ctrl: "+ctrl.toString());
            num=parm.num;
            $('#num').html("num: "+num.toString());
            cycle=parm.cycle;
            $('#cycle').html("cycle: "+cycle.toString());
            sjson=parm.sjson;
            $('#sjson').html ("sjson: "+JSON.stringify(sjson));
            let myTime=parm.myTime;
            $('#myTime').html("myTime: "+myTime.toString());
            let myDate=parm.myDate;
            $('#myDate').html("myDate: "+myDate);
            / *    
            // Парсим и обрабатываем sjson
            if ((JSON.stringify(sjson)==s33_LOW)||(JSON.stringify(sjson)==s33_HIGH))
            {
              parm=JSON.parse(JSON.stringify(sjson));
              // Выделяем json-подстроку по led33
              let led33=parm.led33[0];
              // Парсим led33
              parm=JSON.parse(JSON.stringify(led33));
              // Выделяем состояние led33 (горит - не горит)
              let status=parm.status;
              // Высвечиваем led33 в соответствии с состоянием
              //$('#status').html(status);
              //if (status=="inHIGH") $('#spot').css('background','SandyBrown');
              //else $('#spot').css('background','LightCyan');
            }
            else if (JSON.stringify(sjson)==s33_MODE0)
            {
              console.log('s33_MODE0: '+s33_MODE0);
              //ram.set("LmpMode",0);  // 0 - выключен режим 
            }
            else
            {
              console.log('sjson: '+JSON.stringify(sjson));
            }
            * /
            //alert(JSON.stringify(sjson));
            tickers.render(JSON.stringify(sjson));
          }
        }
        // Обрабатываем ошибку в JSON-ответе 
        catch (err) 
        {
          console.log("Ошибка в JSON-ответе\n"+Error(err)+":\n"+messa);
          DialogWind("Ошибка в JSON-ответе<br>"+Error(err)+":<br>"+messa);
        }
        
      }
    }
  });
  
*/  
  
  
  
  
  
}

function resetSocket(ipath,iip)
{
  console.log('Удаляем вэбсокет сервер!');
  showMessage('Удаляем вэбсокет сервер!');
}

window.addEventListener('DOMContentLoaded', function () 
{
  var socket;
  // --------------------------------------------------------------------------
  //      Установить соединение с сервером и назначить обработчики событий                   
  // --------------------------------------------------------------------------
  document.getElementById('connect').onclick = function () 
  {
    // Новое соединение открываем, если старое соединение закрыто
    if (socket === undefined || socket.readyState !== 1) 
    {
      socket = new WebSocket(document.getElementById('server').value);
    } 
    else 
    {
      showMessage('Надо закрыть уже имеющееся соединение');
    }
    // Определяем четыре функции обратного вызова: одну при получении данных и 
    // три – при изменениях в состоянии соединения
    
    // При получении данных от сервера
    socket.onmessage = function (event) 
    { 
      showMessage('Получено сообщение от сервера: ' + event.data);
    }
    // При установке соединения с сервером
    socket.onopen = function () 
    {
      showMessage('Соединение с сервером установлено');
    }
    // Если произошла какая-то ошибка на сокете
    socket.onerror = function(error) 
    { 
      showMessage('Произошла ошибка: ' + error.message);
    };
    // При закрытии соединения с сервером    
    socket.onclose = function(event) 
    { 
      showMessage('Соединение с сервером закрыто');
      if (event.wasClean) 
      {
        showMessage('Соединение закрыто чисто');
      } 
      else 
      {
        showMessage('Обрыв соединения'); // например, «убит» процесс сервера
      }
      showMessage('Код: ' + event.code + ', причина: ' + event.reason);
    };
  };
  // --------------------------------------------------------------------------
  //                      Отправить сообщения серверу
  // --------------------------------------------------------------------------
  document.getElementById('send-msg').onclick = function () 
  {
    if (socket !== undefined && socket.readyState === 1) 
    {
      var message = document.getElementById('message').value;
      socket.send(message);
      showMessage('Отправлено сообщение серверу: ' + message);
    } 
    else 
    {
      showMessage('Невозможно отправить сообщение, нет соединения');
    }
  };
  // --------------------------------------------------------------------------
  //                      Закрыть соединение с сервером
  // --------------------------------------------------------------------------
  document.getElementById('disconnect').onclick = function () 
  {
    if (socket !== undefined && socket.readyState === 1) 
    {
      socket.close();
    } 
    else 
    {
      showMessage('Соединение с сервером уже было закрыто');
    }
  };
});

// ******************************************************** wsTve/socket.js ***
