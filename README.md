# Проект TCP Communication

## Описание

В рамках данного проекта разрабатываются два приложения на языке программирования C. Лучше всего, если файлы будут в форматах _.c и _.h, сопровождаемые make-файлом. Проект ориентирован на работу с компилятором gcc (mingw или Linux).

### Приложение 1: TCP-сервер

TCP-сервер принимает запросы от клиентов (приложение 2) и реагирует на них в соответствии с логикой, описанной ниже.

### Приложение 2: TCP-клиент

TCP-клиент посылает запросы серверу (приложению 1) и выводит результаты в консоль в реальном времени.

## Формат Запроса

Запрос представляет собой последовательность байт, имеющую следующий формат:

- Байты 0 и 1: ID клиента (например, ID процесса)
- Байты 2, 3, 4: Адрес слова в восьмеричном виде (диапазон от 0 до 255 в десятичном формате)
- Байты 5 и 6: Целое число со знаком (тип short)
- Байты 7 и 8: Цена старшего разряда – целое число (тип unsigned short)
- Байт 9: Количество значащих разрядов – целое число типа uchar
- Байты 10 и 11: Контрольная сумма запроса (алгоритм CRC16)

## Логика Обработки Запроса

Сервер обрабатывает запрос следующим образом:

1. Проверка контрольной суммы запроса. Если принятая CRC16 совпала с рассчитанной сервером CRC16, то переход к следующему шагу, иначе – формирование ответа "invalid request".
2. Формирование ответа клиенту. Ответ представляет собой 32-разрядное слово, в котором закодировано значение, переданное в запросе от клиента в байтах 5 и 6.

   - Разряды с 27 по 13 хранят отрицательное число в дополнительном коде, если S == 1.
   - Разряды с 28 по 13 заполняются в зависимости от количества значащих разрядов, переданных в запросе от клиента.
   - Ответ сервера передается клиенту в виде последовательности из четырех байт, содержащих 32-разрядное слово.

3. Формирование ответа для клиента вида "invalid request", если контрольная сумма не совпала.

## Количество Клиентов

Количество запущенных клиентов может быть от одного до десяти.

## Сборка и Запуск

### Компиляция

gcc server.c -o server
gcc client.c -o client

### Запуск

./server
./client
