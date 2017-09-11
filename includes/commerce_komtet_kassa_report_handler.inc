<?php

function _update_commerce_komtet_kassa_report($status, $result) {

    $report = db_select('commerce_komtet_kassa_reports', 'r')->fields('r')
                                                             ->condition('r.operation_id', $result['id'])
                                                             ->execute()
                                                             ->fetchObject();
    if ($report === false) {
        header('HTTP/1.1 404 Not Found');
        exit();
    }

    $update_fields = array(
        'updated_at' => time(),
        'result' => drupal_json_encode($result),
        'status' => $status
    );

    if ($status == 'fail') {
        $update_fields['error'] = $result['error_description'];
    }

    db_update('commerce_komtet_kassa_reports')->fields($update_fields)
                                              ->condition('id', $report->id)
                                              ->execute();
}

function komtet_kassa_report_handler($status) {

    if (!array_key_exists('HTTP_X_HMAC_SIGNATURE', $_SERVER)) {
        header('HTTP/1.1 401 Unauthorized');
        exit();
    }

    if ($_SERVER['REQUEST_METHOD'] != 'POST') {
        header('HTTP/1.1 405 Method Not Allowed');
        header('Allow: POST');
        exit();
    }

    if (empty(variable_get('commerce_komtet_kassa_secret_key', ''))) {
         watchdog('commerce_komtet_kassa',  'Unable to handle report: variable "commerce_komtet_kassa_secret_key" is not defined', array(), WATCHDOG_ERROR, $link = NULL);
         header('HTTP/1.1 500 Internal Server Error');
    }

    $scheme = array_key_exists('HTTPS', $_SERVER) && strtolower($_SERVER['HTTPS']) !== 'off' ? 'https' : 'http';
    $url = sprintf('%s://%s%s', $scheme, $_SERVER['SERVER_NAME'], $_SERVER['REQUEST_URI']);
    $data = file_get_contents('php://input');
    $signature = hash_hmac('md5', $_SERVER['REQUEST_METHOD'] . $url . $data, variable_get('commerce_komtet_kassa_secret_key'));

    if ($signature != $_SERVER['HTTP_X_HMAC_SIGNATURE']) {
        header('HTTP/1.1 403 Forbidden');
        exit();
    }

    $data = drupal_json_decode($data);

    foreach (array('external_id', 'state', 'id') as $key) {
        if (!array_key_exists($key, $data)) {
            header('HTTP/1.1 400 Bad Request');
            header('Content-Type: text/plain');
            echo $key." is required\n";
            exit();
        }
    }

    _update_commerce_komtet_kassa_report($status, $data);

    header('HTTP/1.1 200 OK');
}


