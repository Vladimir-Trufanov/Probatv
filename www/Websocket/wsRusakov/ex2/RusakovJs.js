// *** RusakovJs.js ***

$(document).ready(function() 
{

});

// Загрузить RusakovServer
function RusakovServer()
{
  console.log('RusakovServer()');
  // Выполняем запрос
  let pathserver='Rusakov/RusakovServer.php';
  // Делаем запрос на отправку изображения 
  $.ajax({
    url: pathserver,
    type: 'POST',
    data: {ip:'iip'},
    // Выводим ошибки при выполнении запроса в PHP-сценарии
    error: function (jqXHR,exception) {console.log('Ошибка');},
    // Обрабатываем ответное сообщение
    success: function(message)
    {
      console.log(message);
    }
  });
}

// Загрузить RusakovClient
function RusakovClient()
{
  console.log('RusakovClient()');
  // Выполняем запрос
  let pathserver='Rusakov/RusakovClient.php';
  // Делаем запрос на отправку изображения 
  $.ajax({
    url: pathserver,
    type: 'POST',
    data: {ip:'iip'},
    // Выводим ошибки при выполнении запроса в PHP-сценарии
    error: function (jqXHR,exception) {console.log('Ошибка');},
    // Обрабатываем ответное сообщение
    success: function(message)
    {
      console.log(message);
    }
  });
}

var socket;

    // показать сообщение в #socket-info
    function showMessage(message) {
        var div = document.createElement('div');
        div.appendChild(document.createTextNode(message));
        document.getElementById('socket-info').appendChild(div);
    }


// Загрузить JavaScriptClient
function JavaScriptClient()
{
        if (socket === undefined || socket.readyState !== 1) {
            socket = new WebSocket('ws://127.0.0.1:7775');
        } else {
            showMessage('Надо закрыть уже имеющееся соединение');
        }

        // четыре функции обратного вызова: одна при получении данных и три – при изменениях в состоянии соединения
        socket.onmessage = function (event) { // при получении данных от сервера
            showMessage('Получено сообщение от сервера: ' + event.data);
        }
        socket.onopen = function () { // при установке соединения с сервером
            showMessage('Соединение с сервером установлено');
        }
        socket.onerror = function(error) { // если произошла какая-то ошибка
            showMessage('Произошла ошибка: ' + error.message);
        };
        socket.onclose = function(event) { // при закрытии соединения с сервером
            showMessage('Соединение с сервером закрыто');
            if (event.wasClean) {
                showMessage('Соединение закрыто чисто');
            } else {
                showMessage('Обрыв соединения'); // например, «убит» процесс сервера
            }
            showMessage('Код: ' + event.code + ', причина: ' + event.reason);
        };
}



/*
window.addEventListener('DOMContentLoaded', function () {

    var socket;

    // показать сообщение в #socket-info
    function showMessage(message) {
        var div = document.createElement('div');
        div.appendChild(document.createTextNode(message));
        document.getElementById('socket-info').appendChild(div);
    }

    // Установить соединение с сервером и назначить обработчики событий
    document.getElementById('connect').onclick = function () {
        // новое соединение открываем, если старое соединение закрыто
        if (socket === undefined || socket.readyState !== 1) {
            socket = new WebSocket(document.getElementById('server').value);
        } else {
            showMessage('Надо закрыть уже имеющееся соединение');
        }

        // четыре функции обратного вызова: одна при получении данных и три – при изменениях в состоянии соединения
        socket.onmessage = function (event) { // при получении данных от сервера
            showMessage('Получено сообщение от сервера: ' + event.data);
        }
        socket.onopen = function () { // при установке соединения с сервером
            showMessage('Соединение с сервером установлено');
        }
        socket.onerror = function(error) { // если произошла какая-то ошибка
            showMessage('Произошла ошибка: ' + error.message);
        };
        socket.onclose = function(event) { // при закрытии соединения с сервером
            showMessage('Соединение с сервером закрыто');
            if (event.wasClean) {
                showMessage('Соединение закрыто чисто');
            } else {
                showMessage('Обрыв соединения'); // например, «убит» процесс сервера
            }
            showMessage('Код: ' + event.code + ', причина: ' + event.reason);
        };
    };

    // Отправка сообщения серверу
    document.getElementById('send-msg').onclick = function () {
        if (socket !== undefined && socket.readyState === 1) {
            var message = document.getElementById('message').value;
            socket.send(message);
            showMessage('Отправлено сообщение серверу: ' + message);
        } else {
            showMessage('Невозможно отправить сообщение, нет соединения');
        }
    };

    // Закрыть соединение с сервером
    document.getElementById('disconnect').onclick = function () {
        if (socket !== undefined && socket.readyState === 1) {
            socket.close();
        } else {
            showMessage('Соединение с сервером уже было закрыто');
        }
    };

});
*/

