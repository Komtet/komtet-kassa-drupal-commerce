# Модуль КОМТЕТ Кассы для Drupal Commerce 7.x

## Описание работы модуля

Данное решение позволяет подключить Ваш интернет-магазин к облачному сервису КОМТЕТ Касса с целью соответствия требованиям 54-ФЗ для регистрации расчетов с использованием электронного средства платежа в сети Интернет.

Возможности плагина

  - автоматическая фискализация платежей;
  - фискализация в ручном режиме.

Описание работы

Плагин реагирует на событие оплаты заказа, которое возникает, когда клиент совершает оплату через один из подключенных модулей приема платежей, например, PayPal или Robokassa.

После оплаты заказа информации о заказе формируется и отправляется запрос на фискализацию платежа в систему КОМТЕТ Касса.

Как только данные по заказу появляются в системе КОМТЕТ Касса, формируется чек, который записывается на фискальный накопитель кассового аппарата и он же отправляется в ОФД (Оператор Фискальных Данных). Если указано в настройках, аппарат может распечатать бланк чека.

Важно! 54-ФЗ обязует выдать электронный чек клиенту, для того чтобы электронный чек был выслан клиенту на электронную почту необходимо сделать обязательным поле email на форме оформления заказа.

Также фискализировать заказ можно в ручном режиме. Для этого нужно перейти на вкладку «Фискализация» заказа в управлении заказом и нажать кнопку «Фискализировать».

Список запросов на фискализацию можно посмотреть на вкладке «КОМТЕТ Касса» — «Отчеты» в разделе администратора магазина.

Таким образом вы можете использовать раздел администратора магазина для печати чека по любому заказу.

## Установка

Для установки модуля нужно выполнить слудующие действия:

1. Войти в админ панель и перейтив в раздел «Модули» (Modules).
2. Вверху нажать кнопку «Установить новый модуль».
3. Кликнуть Обзор (Browse) и выбрать файл name.tar.gz на компьютере или указать URL-адрес откуда будет загружен моуль. Затем нажать кнопку «Установить» после чего начнется процесс установки которого стоит дождаться. 
4. После успешного завершения установки модуль нужно активировать, кликнув по ссылке «Включить добавленный модуль» (Enable newly added modules) и попадаем на страницу списка модулей.
5. Находим модуль КОМТЕТ Кассы в списке, для включения модуля нужно поставить галочку в колонке «Включено» и внизу страницы нажать кнопку «Сохранить настройки».

## Настройка

Прежде чем приступить к настройке модуля вам потребуется зарегистрировать в [личном кабинете на сайте КОМТЕТ Касса][kassa_komtet_signup].

Для настройки плагин нужно перейти в раздел «Магазин» — «КОМТЕТ Касса» — «Вклдака Настройки»

В настройках модуля необходимо указать:
1. URL по которому система КОМТЕТ Касса принимает данные для фискализации. Актуальный адрес указан по умолчанию.
2. Идентификатор магазина. В личном кабинете на сайте КОМТЕТ Касса зайдите в меню «Магазины» (слева), далее выберете нужный магазин и зайдите в его настройки, там вы и найдете необходимое значение (ShopId).
3. Секретный ключ магазина. Аналогично предыдущему (Secret).
4. Включить или отключить печать бумажного чека.
5. Указать систему налогообложения вашей компании. Данные о системе налогообложения будут использованы при формировании чека.
6. Скопировать в настройках плагина данные из полей Success url (http://your_domain/komtet-kassa/success)  и Failure url (http://your_domain/komtet-kassa/fail), и записать их в соответствующие поля в настройках магазина в личном кабинете КОМТЕТ Касса.


[kassa_komtet_signup]: https://kassa.komtet.ru/signup