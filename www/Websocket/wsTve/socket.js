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
var mybeImg='sampo.jpg';
var timebeg;              // время отправки сообщения
var timeend;              // время приема сообщения

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
  //showMessage('Выбираем данные по памяти!');
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
}

function resetSocket(ipath,iip)
{
  console.log('Удаляем вэбсокет сервер!');
  showMessage('Удаляем вэбсокет сервер!');
}

window.addEventListener('DOMContentLoaded', function () 
{
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
      mybeImg=event.data;
      timeend=Date.now();
      showMessage('Получено сообщение от сервера: ' + mybeImg);
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
  //                      Отправить 2 больших текста
  // --------------------------------------------------------------------------
  document.getElementById('send-file').onclick = function () 
  {
    if (socket !== undefined && socket.readyState === 1) 
    {
      //var mesfile='А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке!';
      var mesfile=
      '==1============================ А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! ***************************** '+
      '==2============================ А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! ***************************** '+
      '==3============================ А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! ***************************** '+
      '==4============================ А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! ***************************** '+
      '==5============================ А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! ***************************** '+
      '==6============================ А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! ***************************** '+
      '==7============================ А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! ***************************** '+
      '==8============================ А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! ***************************** '+
      '==9============================ А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! ***************************** '+
      "=10============================= А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! *****************************"+
      '=11============================= А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! *****************************'+
      '=12============================= А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! *****************************'+
      '=13============================= А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! *****************************'+
      '=14============================= А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! *****************************'+
      '=15============================= А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! *****************************'+
      '=16============================= А вы, пишите письма мелким почерком, поскольку места мало в рюкзаке! **************************123';
      socket.send(mesfile);
      //mesfile="Всем большой привет!";
      //socket.send(mesfile);
      //showMessage('Отправлено fil.txt: ' + mesfile);
    } 
    else 
    {
      showMessage('Невозможно отправить сообщение, нет соединения');
    }
  };
  
  // --------------------------------------------------------------------------
  //                      Отправить base64
  // --------------------------------------------------------------------------
  document.getElementById('send-img').onclick = function () 
  {
    if (socket !== undefined && socket.readyState === 1) 
    {
      var mesimg=
       'data:image/jpeg;base64,/9j/4AAQSkZJRgABAgAAZABkAAD/7AARRHVja3kAAQA'+
       'EAAAAPAAA/+4ADkFkb2JlAGTAAAAAAf/bAIQABgQEBAUEBgUFBgkGBQYJCwgGBggLD'+
       'AoKCwoKDBAMDAwMDAwQDA4PEA8ODBMTFBQTExwbGxscHx8fHx8fHx8fHwEHBwcNDA0'+
       'YEBAYGhURFRofHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fHx8fH'+
       'x8fHx8fHx8f/8AAEQgAGABkAwERAAIRAQMRAf/EAIgAAAICAwEBAAAAAAAAAAAAAAU'+
       'GAwQAAgcBCAEAAgMBAAAAAAAAAAAAAAAAAgMAAQQFEAACAQMDBAEDBAMBAAAAAAABA'+
       'gMRBAUAIRIxQRMGFFEiB2FxMkKxUjNDEQABAwIEBQIGAwAAAAAAAAABABECEgMhMUE'+
       'EUSIyExRh0fBxgZHBBaHhI//aAAwDAQACEQMRAD8A+mczm7DDWD3t65EakKqgVZnPR'+
       'FH1Ol3bsbcapIoQMiwSnL+UvNLPHisNdXfx4+csz0SMbVO45bD66wy/Ygh4RJWgbZu'+
       'osih98x8eHtby4t5lv7qNXGJhUzXQZ+i8F6V+rU21o8uIAfqOmqV2S/pxTFaTSTW8c'+
       'skLQO6qzwvQshIqVJG1R+mtMS4SypjTVqlg6aiizp+uoolLPfk313EvJCBNezxP43S'+
       '2QMA4NCvJiBt+msdzfW4mnMp8dvIh0In/ACrkI54qevXK2clALiQso5N0WvDjU9t9I'+
       'l+wkA9BZGNuHZ063GZx8F7aWM0nG8vP+UAHJqAFiWp0G1KnW6V6IkInqKziBIJ0Cu8'+
       'VJ3FfppqFYwQfcwH7nUUXtRSvb6aiiGZn1/GZlLdL+NpYraTzLEHKqzUK0YDqKHSrt'+
       'mNxqtEcJmOS5/kvZEvs7devllx/q9q5t3e0jq0kkdCwZxsqg9eINO+uZfvxq7cuW36'+
       'LVbtlqhjJOtlF6zgMLPkbNY47NUMst0h8jyU7lySzEnsTroQFu3CqPSs0qpSY5oFbe'+
       '9Zpi0s9pGpyDKmCx24nepIaSU1I8fTegr21lG9kztjLpH5KcbAfPLNEYffLVhlp3gP'+
       'wsYywi4Uk+a4NQ6IpHQHv9N9MO+iKiRhH+TwCHxyWGpQ2T2eTBRJkshFNLl88/K3xL'+
       'y0SCGLvyK0RQpqSV701Qu0CsvVLR8lKKsBkNVawnvFx7FnLuxsbY2+HtozFLkZCVk+'+
       'RIB4/GDt32B3PXbTLe57kmA5eKGVqkOc1pa4z070O2DzzSSzv93lmHmmNT1HFRx30s'+
       '9nbnHqP3Riu7lkgd/7tBnPY4Hit5zhsNby38qulBJcKKRczUgKK7dyTpMt1G4amNMM'+
       'fqiFoxDalTesPPjrW49y9sYxyScmtoyhM7NJtULuRVaJEg/rueumWgI/6zz9/jBVMv'+
       'yRRmD3PO3Gax1muLFtBevVknLGdYaV5lV/h+zakd5M3BGln+/z9FRsRESXQLJn3X23'+
       'OZfE2t1bRYGylSN3XlTlSvEuAC7Dqy9AdtDcFy9IgEUgq4mMA5GK6V4D8T4/M8vHw8'+
       'tBWvGnKnSuuksqhytndXeNntbS5NnPMvBbkLzZAdiQKjenTQ3ImUSAWKKJAOKr471r'+
       'E2OEhwyQiSzhXjSTdmY/ydj/sxJJOgFiFFBDhX3C76pVuPxaY5p7bHZOWHCX4Zb+wk'+
       'ZnI25I0THuHA69u+s3gs4iWidE3yNSMVLJ+NphPaXseWuDk4SwuL1hV2jZOASMV4px'+
       'WtNQ7IuDUatT8ZKd/RsFcu/QMclmFwh+BepKkouXLy/ctKkqzUqf86u5sYkCnlILup'+
       'HcHXEKKT8ZY65y9rlMhe3F9PEpFysrfbM1QVqAQFRf9AKHR+ICQZF/yh7xZgGUvqfo'+
       'KYYtJfXjZCQTvcwIQViSR/wD04EtWSm3I9O2rs7WguS6k7r5BlpmZr/P5K4wmOt/Ba'+
       'QEQ5LKyoQwDDk0duWG7UPXtoLtVydIDAZy9lcGiHJx4e6LXPq2Pf1iXAWg+NbPF40Y'+
       'bkHryberEkb6dPbxNugYBALhEqkKl9HvLm1s2u8xNNk7GRHtrrgviThtQQn7Saf2ap'+
       'rpXikgPI1DI/wBI+8NBgth6OoyTzR3s0VpPEIrlUZvkTEmr+SYmv3Hrxoe3TQjZNcq'+
       'EsGx4n5lWb7xZsVX9W/H9xioJbS/vvkY4XLXENlCDGjMSCpmNeT04j7a8f31dnamOB'+
       'PLwVTvA5DFOX9q79dbUhf/Z';
      timebeg=Date.now();
      socket.send(mesimg);
      
      
      //document.getElementById('imgCard').src = 'sampo.jpg';    // работает
      //document.getElementById('imgCard').src = mesimg;           // работает
      //document.getElementById('imgCard').src = mybeImg;           // работает
      //document.getElementById('imgCard').innerHTML="<img id='imgCard' src='sampo.jpg'>";
            
      delayedGreeting();
      console.log("Пока");
      
      
      
      
      
      //document.getElementById('imgCard').src = mybeImg;
      
      //document.getElementById('imgCard').innerHTML='<img id="imgCard" src="'+mybeImg+'">';

      //document.getElementById('imgCard').innerHTML='<img id="imgCard" src="sampo.jpg">';
      //                                            <img id="imgCard" src="Kwinflat.jpg">
      
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

  
  // --------------------------------------------------------------------------
  // --------------------------------------------------------------------------

function sleep(ms) 
{
  return new Promise(resolve => setTimeout(resolve, ms));
}  
  
async function delayedGreeting() 
{
  console.log("Привет");
  await sleep(2000);
  console.log("мир");
  document.getElementById('imgCard').src = mybeImg;      
  document.getElementById('rem').innerHTML='Пока!';          // работает
  console.log("Время на сообщение: "+(timeend-timebeg));
}  


// ******************************************************** wsTve/socket.js ***
