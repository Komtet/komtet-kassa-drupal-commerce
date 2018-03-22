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


// Add Position into check with discount
function AddPositionDiscount($wrapper,$discount,&$check)
{
    echo "<pre>".$discount.'- discount';
    
    foreach ($wrapper->commerce_line_items as $key => $line_item_wrapper) // looking for producks in order
    {
        $line_item_type = $line_item_wrapper->getBundle();
        $item_sum = get_amount($line_item_wrapper->commerce_total);
        $item_price = get_amount($line_item_wrapper->commerce_unit_price);
        $item_quantity = get_quantity($line_item_wrapper);

        if (in_array($line_item_type, commerce_product_line_item_types())) //taking out everything that is not a product
        {
            $item_name = $line_item_wrapper->commerce_product->value()->title;
            $check->addPosition(new Position($item_name, $item_price, $item_quantity, round($item_sum-($item_sum*$discount),2), round($item_sum*$discount,2) ,new Vat(Vat::RATE_NO)));
            
        }
        if (!in_array($line_item_type, commerce_product_line_item_types()) && $line_item_type =='shipping' ) //looking for shipping
        {
            $item_name = $line_item_wrapper->commerce_product->value()->title;
            $check->addPosition(new Position($item_name, $item_price, $item_quantity, $item_sum, 0, new Vat(Vat::RATE_NO)));
        }        
    
    }
    
} // end of AddPositionDiscount


// Add Position into check without discount
function AddPosition($wrapper,&$check)
{
    echo "<pre> no- discount";
    
    foreach ($wrapper->commerce_line_items as $key => $line_item_wrapper)  // looking for producks in order
    {
        $line_item_type = $line_item_wrapper->getBundle();
        $item_sum = get_amount($line_item_wrapper->commerce_total);
        $item_price = get_amount($line_item_wrapper->commerce_unit_price);
        $item_quantity = get_quantity($line_item_wrapper);
        
        if (in_array($line_item_type, commerce_product_line_item_types())) // just in case again taking out everything that is not a product
        {
            $item_name = $line_item_wrapper->commerce_product->value()->title;
            $check->addPosition(new Position($item_name, $item_price, $item_quantity, $item_sum, 0, new Vat(Vat::RATE_NO)));
        }         
        if (!in_array($line_item_type, commerce_product_line_item_types()) && $line_item_type =='shipping' ) //looking for shipping
        {
            $item_name = $line_item_wrapper->commerce_product->value()->title;
            $check->addPosition(new Position($item_name, $item_price, $item_quantity, $item_sum, 0, new Vat(Vat::RATE_NO)));
        }        
    }    
    
}//end of AddPosition

//is there order discount
function IsOrderDiscount($wrapper)
{
    $isOrderDiscount = false;      
    foreach ($wrapper->commerce_line_items as $key => $line_item_wrapper)  // looking for discount 
    {
        $line_item_type = $line_item_wrapper->getBundle();
        if (!in_array($line_item_type, commerce_product_line_item_types())) // if it's not product 
        {
            $isOrderDiscount = true;
        }
    }
    
    return $isOrderDiscount;
}// end of IsOrderDiscount

// get discount 
 function GetDiscountOne($wrapper)
 {
    $discount_summ = 0;
    $line_items = $wrapper->commerce_line_items;
    $total = commerce_line_items_total($line_items);
    $summ_without_discount = $total['amount']/100; 
	 
    foreach ($wrapper->commerce_line_items as $key => $line_item_wrapper) 
    {
        $line_item_type = $line_item_wrapper->getBundle();
        if (!in_array($line_item_type, commerce_product_line_item_types()) && $line_item_type =='shipping' ) //looking for shipping
        {
            $shipping_sum = get_amount($line_item_wrapper->commerce_total);
            $summ_without_discount = $summ_without_discount - $shipping_sum;           
        }

        if (!in_array($line_item_type, commerce_product_line_item_types()) && $line_item_type =='commerce_discount') 
        {
             $discount_summ = abs(get_amount($line_item_wrapper->commerce_total)); 
        }
    } 
    $discount = round($discount_summ/($discount_summ + $summ_without_discount),2);
    return $discount;
 }//end of GetDiscountOne


/**
 * Implements hook_commerce_payment_order_paid_in_full().
 */
function commerce_komtet_kassa_commerce_payment_order_paid_in_full($order, $transaction) {
    
    komtet_kassa_fiscalize($order, $transaction);
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

// main func
function komtet_kassa_fiscalize($order, $transaction = NULL, $op = 'sell') {

    if (empty($order->order_id)) {
        return;
    }

    // todo: include sell_refund, sel_correction, buy, buy_refund, buy_correction.
    $wrapper = entity_metadata_wrapper('commerce_order', $order->order_id);
    //$line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);   
        
    $check = new Check($order->order_id, 
            $wrapper->mail->value(), 
            Check::INTENT_SELL, 
            (int) variable_get('commerce_komtet_kassa_tax_system', '1'));
    
    $check->setShouldPrint(variable_get('commerce_komtet_kassa_should_print'));
	
    //is there order discount
    if(IsOrderDiscount($wrapper))
    { 
        AddPositionDiscount($wrapper, GetDiscountOne($wrapper),$check); // add position into the check with discount
    }
    else
    {
        AddPosition($wrapper,$check); // add position into check without discount
    }     
  
    if ($transaction) {
        $check->addPayment(Payment::createCard(get_amount($transaction, false)));
    }
    else {
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
        $response = $queueManager->putCheck($check);
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
    
    return $error_message === NULL;
}

function commerce_fiscalize_form($form, &$form_state, $order) {
    $form = array();
    $form_state['order'] = $order;

    $form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Фискализировать'),
	);

    return $form;
}

function commerce_fiscalize_form_submit($form, &$form_state) {
    $order = commerce_order_load($form_state['order']->order_id);
    if (komtet_kassa_fiscalize($order)) {
        drupal_set_message('Заказ отправлен на фискализацию в сервис KOMTET Касса');
    } else {
        drupal_set_message('При отправке на фискализацию в сервис KOMTET Касса произошла ошибка', 'error');
    }
    watchdog('commerce_komtet_kassa', 'Заказ #%order_id был отправлен на фискализацию в систему КОМТЕТ Касса вручную', array('%order_id' => $order->order_id), WATCHDOG_NOTICE, $link = NULL);
}

function commerce_fiscalize_ui_order_tab($order) {

    $header = array(
        array('data' => '#', 'field' => 'id'),
        array('data' => 'Дата создания задачи', 'field' => 'created_at'),
        array('data' => 'Дата обновления задачи', 'field' => 'updated_at'),
        array('data' => 'Номер операции в очереди', 'field' => 'operation_id'),
        array('data' => 'Статус задачи', 'field' => 'status'),
    );

    $select = db_select('commerce_komtet_kassa_reports', 'reports');

    $select->fields('reports', array('id', 'order_id', 'created_at', 'updated_at', 'operation_id', 'status'))
           ->condition('order_id', $order->order_number)
           ->orderBy('id', 'desc');

    $db_result = $select->execute();

    $rows = array();

    if ($db_result->rowCount() > 0) {
        $table = '<p><strong>Список задач для фискализации заказа:</strong></p>';
        foreach ($db_result as $row) {
            $rows[] = array(
                'id' => komtet_kassa_report_detail_link($row->id),
                'created_at' => komtet_kassa_display_date($row->created_at),
                'updated_at' => komtet_kassa_display_date($row->updated_at),
                'operation_id' => $row->operation_id,
                'status' => komtet_kassa_display_report_status($row->status, 'Неизвестно')
            );
        }
        $table .= theme('table', array('header' => $header, 'rows' => $rows));
    } else {
        $table = theme('status_messages', drupal_set_message('Для заказа нет отправленных задач на фискализацию в сервис КОМТЕТ Касса', 'warning'));
    }

    $form = drupal_get_form('commerce_fiscalize_form', $order);

    return $table . '<br/>' . drupal_render($form); 
}
