<?php

function komtet_kassa_reports() {

    $header = array(
        array('data' => '#', 'field' => 'id'),
        array('data' => 'Номер заказа', 'field' => 'order_id'),
        array('data' => 'Дата создания задачи', 'field' => 'created_at'),
        array('data' => 'Дата обновления задачи', 'field' => 'updated_at'),
        array('data' => 'Номер операции в очереди', 'field' => 'operation_id'),
        array('data' => 'Статус задачи', 'field' => 'status'),
    );

    $select = db_select('commerce_komtet_kassa_reports', 'reports')->extend('PagerDefault')
                                                                   ->extend('TableSort');

    $select->fields('reports', array('id', 'order_id', 'created_at', 'updated_at', 'operation_id', 'status'))
           ->limit(10)
           ->orderByHeader($header)
           ->orderBy('id', 'desc');

    $results = $select->execute();

    $rows = array();

    foreach ($results as $row) {
        $rows[] = array(
            'id' => '<a href="/admin/commerce/config/komtet-kassa/reports/' . $row->id . '/show">' . $row->id . '</a>',
            'order_id' => '<a href="/admin/commerce/orders/' . $row->order_id . '">' . $row->order_id . '</a>',
            'created_at' => komtet_kassa_display_date($row->created_at),
            'updated_at' => komtet_kassa_display_date($row->updated_at),
            'operation_id' => $row->operation_id,
            'status' => komtet_kassa_display_report_status($row->status, 'Неизвестно')
        );
    }

    $table = theme('table', array('header' => $header, 'rows' => $rows));
    $table .= theme('pager');

    return $table;
}

function komtet_kassa_show_report($report_id) {

    $report = db_select('commerce_komtet_kassa_reports', 'r')->fields('r')
    ->condition('r.id', (int) $report_id)
    ->execute()
    ->fetchObject();

    $rows = array(
        array(array('data' => 'Номер заказа', 'header' => TRUE, 'width' => '300px'), $report->order_id),
        array(array('data' => 'Номер операции в очереди КОМТЕТ Кассы', 'header' => TRUE), $report->operation_id),
        array(array('data' => 'Статус', 'header' => TRUE), komtet_kassa_display_report_status($report->status, 'Неизвестно')),
        array(array('data' => 'Создан в', 'header' => TRUE), komtet_kassa_display_date($report->created_at)),
        array(array('data' => 'Обновлен в', 'header' => TRUE), komtet_kassa_display_date($report->updated_at))
    );

    if ($report->status == 'success') {
        $rows[] = array(array('data' => '<strong>Фискальные данные</strong>', 'header' => TRUE), '');

        $result = drupal_json_decode($report->result);

        if (isset($result['fiscal_data'])) {
            $fiscal_data = $result['fiscal_data'];
            array_push(
                $rows, 
                array(array('data' => 'Номер фискального накопителя', 'header' => TRUE), $fiscal_data['fn']),
                array(array('data' => 'Фискальный признак чека', 'header' => TRUE), $fiscal_data['fp']),
                array(array('data' => 'Признак расчета', 'header' => TRUE), $fiscal_data['n']),
                array(array('data' => 'Порядковый номер фискального документа', 'header' => TRUE), $fiscal_data['i']),
                array(array('data' => 'Дата и время чека', 'header' => TRUE), $fiscal_data['t']),
                array(array('data' => 'Сумма чека', 'header' => TRUE), $fiscal_data['s'])
            );
        }
    }

    if (empty($report->error) === false) {
        $rows[] = array(array('data' => 'Описание ошибки', 'header' => TRUE), $report->error);
    }

    return theme('table', array('rows' => $rows));
}
