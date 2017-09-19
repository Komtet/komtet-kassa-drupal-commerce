<?php

function komtet_kassa_display_report_status($status, $default = '') {
    $available = array(
        COMMERCE_KOMTET_KASSA_REPORT_STATUS_NEW => 'Ожидает ответа, новый',
        COMMERCE_KOMTET_KASSA_REPORT_STATUS_ERROR => 'Ошибка',
        COMMERCE_KOMTET_KASSA_REPORT_STATUS_SUCCESS => 'Ответ получен, успешно',
        COMMERCE_KOMTET_KASSA_REPORT_STATUS_FAIL => 'Ответ получен, ошибка',
    );
    return isset($available[$status]) ? $available[$status] : $default;
}

function komtet_kassa_display_date($time) {
    return isset($time) ? format_date($time, 'custom', COMMERCE_KOMTET_KASSA_FORMAT_DATE) : '';
}

function komtet_kassa_tax_systems() {
    return array(
        '0' => 'ОСН',
        '1' => 'УСН доход',
        '2' => 'УСН доход - расход',
        '3' => 'ЕНВД',
        '4' => 'ЕСН',
        '5' => 'Патент'
    );
}

function komtet_kassa_report_detail_link($id) {
    return '<a href="/admin/commerce/komtet-kassa/reports/' . $id . '/show">' . $id . '</a>';
}

function komtet_kassa_order_link($id) {
    return '<a href="/admin/commerce/orders/' . $id . '">' . $id . '</a>';
}