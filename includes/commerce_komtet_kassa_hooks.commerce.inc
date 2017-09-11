<?php

require __DIR__ . '/libs/komtet-kassa-php-sdk/autoload.php';

use Komtet\KassaSdk\Client;
use Komtet\KassaSdk\QueueManager;
use Komtet\KassaSdk\Check;
use Komtet\KassaSdk\Payment;
use Komtet\KassaSdk\Position;
use Komtet\KassaSdk\Vat;
use Komtet\KassaSdk\Exception\ClientException;
use Komtet\KassaSdk\Exception\SdkException;


/**
 * Implements hook_commerce_payment_order_paid_in_full().
 */
function commerce_komtet_kassa_commerce_payment_order_paid_in_full($order, $transaction) {
    komtet_kassa_fiscalize($order, $transaction);
}
  
/**
* Implements hook_commerce_order_presave().
*/
function commerce_komtet_kassa_commerce_order_presave($order) {
    if (in_array($order->status, array('checkout_complete', 'pending'))) {
        if (COMMERCE_KOMTET_KASSA_DEBUG) {
            komtet_kassa_fiscalize($order);
        }
    }
}

function get_amount($value, $get_value = True) {
    $amount = $value->amount;
    if ($get_value) {
        $amount = $amount->value();
    }
    return (float) number_format($amount / 100, 2, '.', '');
}

function get_quantity($value) {
    return (float) number_format($value->quantity->value(), 1);
}

function komtet_kassa_fiscalize($order, $transaction = NULL, $op = 'sell') {

    if (empty($order->order_id)) {
        return;
    }

    // todo: include sell_refund, sel_correction, buy, buy_refund, buy_correction.
    $wrapper = entity_metadata_wrapper('commerce_order', $order->order_id);

    $check = new Check($order->order_id, 
                       $wrapper->mail->value(), 
                       Check::INTENT_SELL, 
                       (int) variable_get('commerce_komtet_kassa_tax_system', '1'));
    
    $check->setShouldPrint(variable_get('commerce_komtet_kassa_should_print'));

    foreach ($wrapper->commerce_line_items as $key => $line_item_wrapper) {

        $line_item_type = $line_item_wrapper->getBundle();
        $item_sum = get_amount($line_item_wrapper->commerce_total);
        $item_price = get_amount($line_item_wrapper->commerce_unit_price);
        $item_quantity = get_quantity($line_item_wrapper);

        if (in_array($line_item_type, commerce_product_line_item_types())) {
            $item_name = $line_item_wrapper->commerce_product->value()->title;
        } else {
            $item_name = $line_item_wrapper->line_item_label->value();
        }

        $check->addPosition(new Position($item_name, $item_price, $item_quantity, $item_sum, 0, new Vat(Vat::RATE_NO)));
    }
  
    if ($transaction) {
        // $transaction->payment_method, array('default' => 2));
        $payment_method_type = 1;
        $check->addPayment(Payment::createCard(get_amount($transaction, false)));
    }
    else {
        // Default transaction parameters for debug reasons. todo: wipe out.
        $check->addPayment(Payment::createCard(get_amount($wrapper->commerce_order_total)));
    }

    $client = new Client(variable_get('commerce_komtet_kassa_shop_id'), 
                         variable_get('commerce_komtet_kassa_secret_key'));
    $client->setHost(variable_get('commerce_komtet_kassa_api_url', COMMERCE_KOMTET_KASSA_API_URL));
    
    $queueManager = new QueueManager($client);
    $queueManager->registerQueue('default',  variable_get('commerce_komtet_kassa_queue_id'));
    $queueManager->setDefaultQueue('default');

    $response = $error_message = NULL;
    try {
        watchdog('commerce_komtet_kassa', drupal_json_encode($check->asArray()), array(), WATCHDOG_NOTICE, $link = NULL);
        // $response = $queueManager->putCheck($check);
        $response = drupal_json_decode('{"print_queue_id": 4332, "external_id": "1", "id": 1353, "state": "new"}');
    } catch (SdkException $e) {
        $error_message = $e->getMessage();
        watchdog('commerce_komtet_kassa',  $error_message, array(), WATCHDOG_ERROR, $link = NULL);
    }

    // Report in the report
    $insert_fields = array(
        'created_at' => time(),
        'order_id' => $order->order_id,
        'operation_id' => isset($response) ? $response['id'] : NULL,
        'status' => isset($error_message) ? COMMERCE_KOMTET_KASSA_REPORT_STATUS_ERROR : COMMERCE_KOMTET_KASSA_REPORT_STATUS_NEW,
        'error' => $error_message
    );
    db_insert('commerce_komtet_kassa_reports')->fields($insert_fields)
                                              ->execute();
}
