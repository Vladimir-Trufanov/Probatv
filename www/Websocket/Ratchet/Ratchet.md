## Ratchet

### [Как правильно установить Ratchet с помощью Composer в Windows?](https://stackoverflow.com/questions/41790601/how-to-properly-install-ratchet-using-composer-on-windows)

### [Учебник по PHP WebSockets, ...](https://web.archive.org/web/20161122141317/http://blog.samuelattard.com/the-tutorial-for-php-websockets-that-i-wish-had-existed/)

### [Ratchet](https://github.com/ratchetphp/Ratchet)

### [Использование HTML5 WebSockets c PHP сервером](https://blog-programmista.ru/post/160-ispol-zovanie-html5-websockets-c-php-serverom.html)

---

#### rtchEx1: Ratchet, как локальный сервер (2026-01-17)

- запустил из командной строки "***composer***" и убедился, что он установлен:

```
Microsoft Windows [Version 10.0.26200.7623]
(c) Корпорация Майкрософт (Microsoft Corporation). Все права защищены.

c:\Probatv\www\Websocket\Ratchet\rtchEx1>composer
   ______
  / ____/___  ____ ___  ____  ____  ________  _____
 / /   / __ \/ __ `__ \/ __ \/ __ \/ ___/ _ \/ ___/
/ /___/ /_/ / / / / / / /_/ / /_/ (__  )  __/ /
\____/\____/_/ /_/ /_/ .___/\____/____/\___/_/
                    /_/
Composer version 2.9.3 2025-12-30 13:40:17

Usage:
  command [options] [arguments]

Options:
  -h, --help                     Display help for the given command. When no command is given display help for the list command
  -q, --quiet                    Do not output any message
  -V, --version                  Display this application version
      --ansi|--no-ansi           Force (or disable --no-ansi) ANSI output
  -n, --no-interaction           Do not ask any interactive question
      --profile                  Display timing and memory usage information
      --no-plugins               Whether to disable plugins.
      --no-scripts               Skips the execution of all scripts defined in composer.json file.
  -d, --working-dir=WORKING-DIR  If specified, use the given directory as working directory.
      --no-cache                 Prevent use of the cache
  -v|vv|vvv, --verbose           Increase the verbosity of messages: 1 for normal output, 2 for more verbose output and 3 for debug

Available commands:
  about                Shows a short information about Composer
  archive              Creates an archive of this composer package
    
  ---

  update               [u|upgrade] Updates your dependencies to the latest version according to composer.json, and updates the composer.lock file
  validate             Validates a composer.json and composer.lock

c:\Probatv\www\Websocket\Ratchet\rtchEx1>

```

- подключил к рабочему каталогу ***ratchet*** через командную строку:

***composer require cboden/ratchet***.

```
c:\Probatv\www\Websocket\Ratchet\rtchEx1>composer require cboden/ratchet
No composer.json in current directory, do you want to use the one at c:\Probatv\www? [y,n]? y
Always want to use the parent dir? Use "composer config --global use-parent-dir true" to change the default.
./composer.json has been updated
Running composer update cboden/ratchet
Loading composer repositories with package information
Updating dependencies
Nothing to modify in lock file
Writing lock file
Installing dependencies from lock file (including require-dev)
Nothing to install, update or remove
Generating autoload files
12 packages you are using are looking for funding.
Use the `composer fund` command to find out more!
No security vulnerability advisories found.
Using version ^0.4.4 for cboden/ratchet
```
Произошел интересный диалог. Я согласился на использование ранее установленного ***ratcher*** в корневом каталоге.

```
В текущем каталоге нет файла composer.json, хотите ли вы использовать файл 
по адресу c:\Probatv\www? [y,n]? y

Всегда хотите использовать родительский каталог? 
Используйте "composer config --global user-parent-dir true", чтобы изменить значение по умолчанию.

./composer.json был обновлен

- Запускаем composer update cboden/ratchet
- Загружаем в репозитории composer информацию о пакетах
- Обновляем зависимости
- Ничего не нужно изменять в файле блокировки
- Записываем файл блокировки
- Устанавливаем зависимости из файла блокировки (включая require-dev)
- Ничего не нужно устанавливать, обновлять или удалять

Создание файлов для автоматической загрузки
12 пакетам, которые вы используете, требуется финансирование.
Воспользуйтесь командой "composer fund", чтобы узнать больше!
Сообщений об уязвимостях в системе безопасности не найдено.

Используется версия ^0.4.4 для cboden/ratchet
```
- локально запустил сервер ServerForHTML5.php из командной строки:

***php ServerForHTML5.php***. Вышло сообщение:

```
c:\Probatv\www\Websocket\Ratchet\rtchEx1>php ServerForHTML5.php

Warning: require(C:\Probatv\www\Websocket\Ratchet\rtchEx1/vendor/autoload.php): failed to open stream: No such file or directory in C:\Probatv\www\Websocket\Ratchet\rtchEx1\ServerForHTML5.php on line 8

Fatal error: require(): Failed opening required 'C:\Probatv\www\Websocket\Ratchet\rtchEx1/vendor/autoload.php' (include_path='.;C:\php\pear') in C:\Probatv\www\Websocket\Ratchet\rtchEx1\ServerForHTML5.php on line 8

```
- скопировал каталог vendor из корневого каталога в текущий и снова запустил сервер. Сервер заработал.

- файл клиента ***ClientForHTML5.html*** скопировал в ***index.html*** и запустил, как сайт.

- сайт заработал. Запустил сайт еще в других браузерах. Убедился, что сообщения уходят на сервер, а оттуда ко всем клиентам.

---

#### rtchEx2: Попытка по ip-адресу на masterhost

---

#### Протокол установки Ratchet на моём моноблоке 2026-01-16:

```
Microsoft Windows [Version 10.0.26200.7623]
(c) Корпорация Майкрософт (Microsoft Corporation). Все права защищены.

C:\Probatv\www>composer require cboden/ratchet
./composer.json has been created
Running composer update cboden/ratchet
Loading composer repositories with package information
Updating dependencies
Lock file operations: 18 installs, 0 updates, 0 removals
  - Locking cboden/ratchet (v0.4.4)
  - Locking evenement/evenement (v3.0.2)
  - Locking guzzlehttp/psr7 (2.8.0)
  - Locking psr/http-factory (1.1.0)
  - Locking psr/http-message (2.0)
  - Locking ralouphie/getallheaders (3.0.3)
  - Locking ratchet/rfc6455 (v0.3.1)
  - Locking react/cache (v1.2.0)
  - Locking react/dns (v1.14.0)
  - Locking react/event-loop (v1.6.0)
  - Locking react/promise (v3.3.0)
  - Locking react/socket (v1.17.0)
  - Locking react/stream (v1.4.0)
  - Locking symfony/deprecation-contracts (v2.5.4)
  - Locking symfony/http-foundation (v5.4.50)
  - Locking symfony/polyfill-mbstring (v1.33.0)
  - Locking symfony/polyfill-php80 (v1.33.0)
  - Locking symfony/routing (v5.4.48)
Writing lock file
Installing dependencies from lock file (including require-dev)
Package operations: 18 installs, 0 updates, 0 removals
  - Downloading symfony/polyfill-php80 (v1.33.0)
  - Downloading symfony/deprecation-contracts (v2.5.4)
  - Downloading symfony/routing (v5.4.48)
  - Downloading symfony/polyfill-mbstring (v1.33.0)
  - Downloading symfony/http-foundation (v5.4.50)
  - Downloading react/event-loop (v1.6.0)
  - Downloading evenement/evenement (v3.0.2)
  - Downloading react/stream (v1.4.0)
  - Downloading react/promise (v3.3.0)
  - Downloading react/cache (v1.2.0)
  - Downloading react/dns (v1.14.0)
  - Downloading react/socket (v1.17.0)
  - Downloading ralouphie/getallheaders (3.0.3)
  - Downloading psr/http-message (2.0)
  - Downloading psr/http-factory (1.1.0)
  - Downloading guzzlehttp/psr7 (2.8.0)
  - Downloading ratchet/rfc6455 (v0.3.1)
  - Downloading cboden/ratchet (v0.4.4)
  - Installing symfony/polyfill-php80 (v1.33.0): Extracting archive
  - Installing symfony/deprecation-contracts (v2.5.4): Extracting archive
  - Installing symfony/routing (v5.4.48): Extracting archive
  - Installing symfony/polyfill-mbstring (v1.33.0): Extracting archive
  - Installing symfony/http-foundation (v5.4.50): Extracting archive
  - Installing react/event-loop (v1.6.0): Extracting archive
  - Installing evenement/evenement (v3.0.2): Extracting archive
  - Installing react/stream (v1.4.0): Extracting archive
  - Installing react/promise (v3.3.0): Extracting archive
  - Installing react/cache (v1.2.0): Extracting archive
  - Installing react/dns (v1.14.0): Extracting archive
  - Installing react/socket (v1.17.0): Extracting archive
  - Installing ralouphie/getallheaders (3.0.3): Extracting archive
  - Installing psr/http-message (2.0): Extracting archive
  - Installing psr/http-factory (1.1.0): Extracting archive
  - Installing guzzlehttp/psr7 (2.8.0): Extracting archive
  - Installing ratchet/rfc6455 (v0.3.1): Extracting archive
  - Installing cboden/ratchet (v0.4.4): Extracting archive
6 package suggestions were added by new dependencies, use `composer suggest` to see details.
Generating autoload files
12 packages you are using are looking for funding.
Use the `composer fund` command to find out more!
No security vulnerability advisories found.
Using version ^0.4.4 for cboden/ratchet

C:\Probatv\www>
```