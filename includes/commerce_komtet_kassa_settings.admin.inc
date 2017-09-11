<?php

function komtet_kassa_settings_form() {

    $form = array();

    $form['commerce_komtet_kassa_api_url'] = array(
        '#type' => 'textfield',
        '#title' => 'URL API КОМТЕТ Касса',
        '#description' => 'Значение по умолчанию: <b>' . COMMERCE_KOMTET_KASSA_API_URL . '</b>',
        '#default_value' => variable_get('commerce_komtet_kassa_api_url', COMMERCE_KOMTET_KASSA_API_URL),
        '#required' => TRUE,
    );

    $form['commerce_komtet_kassa_shop_id'] = array(
        '#type' => 'textfield',
        '#title' => 'Идентификатор магазина',
        '#description' => 'Идентификатор вы найдете в личном кабинете КОМТЕТ Кассы: <a href="https://kassa.komtet.ru/manage/shops">Магазины</a>',
        '#default_value' => variable_get('commerce_komtet_kassa_shop_id', ''),
        '#required' => TRUE,
    );

    $form['commerce_komtet_kassa_secret_key'] = array(
        '#type' => 'textfield',
        '#title' => 'Секретный ключ магазина',
        '#description' => 'Ключ вы найдете в личном кабинете КОМТЕТ Кассы, в настройках выбранного магазина: <a href="https://kassa.komtet.ru/manage/shops">Магазины</a>',
        '#default_value' => variable_get('commerce_komtet_kassa_secret_key', ''),
        '#required' => TRUE,
    );

    $form['commerce_komtet_kassa_queue_id'] = array(
        '#type' => 'textfield',
        '#title' => 'Идентификатор очереди',
        '#default_value' => variable_get('commerce_komtet_kassa_queue_id', ''),
        '#required' => TRUE,
    );

    $form['commerce_komtet_kassa_should_print'] = array(
        '#type' => 'checkbox',
        '#title' => 'Печатать бумажный чек?',
        '#description' => 'Вы можете включить или выключить печать бумажного чека.',
        '#default_value' => variable_get('commerce_komtet_kassa_should_print', 1)
    );

    $form['commerce_komtet_kassa_tax_system'] = array(
        '#type' => 'select',
        '#title' => 'Система налогооблажения',
        '#options' => komtet_kassa_tax_systems(),
        '#default_value' => variable_get('commerce_komtet_kassa_tax_system', '1'),
        '#required' => TRUE,
    );
    
    return system_settings_form($form);
}