<?php

/**
 * Menu callback for admin/content/xml_exchange.
 */
function xml_exchange_admin_content() {
    $exchanges = db_select('xml_exchange_loadings', 't')
            ->fields('t')
            ->execute()
            ->fetchAllAssoc('id');
    return theme('xml_exchange_admin_content', array('ex' => $exchanges));
}

function xml_exchange_admin_add_feed() {
    $types = node_type_get_types();
    foreach ($types AS $type) {
        $options[$type->type] = $type->name;
    }
    $form['xml_exchange_node_type'] = array(
        '#type' => 'select',
        '#title' => t('Type of content we creating with feed'),
        '#options' => $options,
    );
    $form['xml_exchange_link'] = array(
        '#type' => 'textfield',
        '#title' => t('Feed address link'),
    );
    $form['xml_exchange_itemtag'] = array(
        '#type' => 'textfield',
        '#title' => t('Feer item tag'),
    );
    $form['#submit'][] = 'xml_exchange_admin_add_feed_submit';
    return system_settings_form($form);
}

function xml_exchange_admin_edit_feed_form($form, $form_state, $feed) {
    $xml = simplexml_load_file($feed->link);
    $struc = array('0' => t('Default of empty'), 'def' => t('Predefinded'));
    for ($i = 0; $i < 15; $i++) {
        $rand = rand(0, count($xml->{$feed->itemtag}) - 1);
        $struc = (!empty($struc)) ? array_merge($struc, _xml_exchange_get_item_fields(get_object_vars($xml->{$feed->itemtag}[$rand]))) : _xml_exchange_get_item_fields(get_object_vars($xml->{$feed->itemtag}[$rand]));
    };
    $form = array(
        '#tree' => TRUE,
        '#feed' => $feed,
    );
    $form['nid'] = array(
        '#type' => 'value',
        '#value' => $feed->id,
    );
    $node_fields = field_info_instances('node', $feed->type);
    $rel_types = array(
        'plain' => t('Plain'),
        'php' => t('PHP Code'),
        'texttoterm' => t('Text to taxonomy term'),
        'texttodropdown' => t('Text to item in select field'),
        'linktoimg' => t('Link to image'),
    );
    $form['#attached']['css'] = array(
        drupal_get_path('module', 'xml_exchange') . '/includes/xml_exchange.css',
    );
    $form['#attached']['js'] = array(
        drupal_get_path('module', 'xml_exchange') . '/includes/xml_exchange.js',
    );
    $data = $feed->scheme ? unserialize($feed->scheme) : array();
    $voc_types = array();
    foreach (taxonomy_get_vocabularies() AS $voc) {
        $voc_types[$voc->vid] = $voc->name;
    }
    $fields = array();
    foreach ($node_fields AS $name => $field) {
        $fields[$name] = $field['label'];
        $form[$name . '_set'] = array(
            '#type' => 'fieldset',
            '#title' => $field['label'],
            '#collapsible' => FALSE,
            '#weight' => $field['widget']['weight'],
            'field' => array(
                '#type' => 'select',
                '#title' => t('Data from'),
                '#options' => $struc,
                '#default_value' => (isset($data[$name . '_set']['field'])) ? $data[$name . '_set']['field'] : '0',
            ),
            'predefined' => array(
                '#type' => 'textfield',
                '#title' => t('Predefined'),
                '#default_value' => (isset($data[$name . '_set']['predefined'])) ? $data[$name . '_set']['predefined'] : '',
            ),
            'type' => array(
                '#type' => 'select',
                '#title' => t('Relation type'),
                '#options' => $rel_types,
                '#default_value' => (isset($data[$name . '_set']['type'])) ? $data[$name . '_set']['type'] : 'plain',
            ),
            'php' => array(
                '#type' => 'textarea',
                '#title' => t('Relation description'),
                '#description' => t('Type a php snippet, returning a value that you need. Use `$value` to get value from this field. Use `{field.name}` to get values from other fields\r\n Use with care, errors may crush the exchange process'),
                '#default_value' => (isset($data[$name . '_set']['php'])) ? $data[$name . '_set']['php'] : '',
                '#attributes' => array('class' => array('hideit')),
                '#cols' => '30',
                '#rows' => '2',
            ),
            'texttoterm' => array(
                '#type' => 'select',
                '#title' => t('Vocabulary'),
                '#default_value' => (isset($data[$name . '_set']['texttoterm'])) ? $data[$name . '_set']['texttoterm'] : '',
                '#attributes' => array('class' => array('hideit')),
                '#options' => $voc_types,
            ),
        );
    };
    $form['actuality_check_set'] = array(
        '#type' => 'fieldset',
        '#title' => t('Actuality check'),
        '#collapsible' => FALSE,
        '#weigth' => -100,
        'check' => array(
            '#type' => 'checkbox',
            '#title' => 'Check',
            '#default_value' => (isset($data['actuality_check_set']['check'])) ? $data['actuality_check_set']['check'] : '0',
        ),
        'field' => array(
            '#type' => 'select',
            '#title' => 'with field',
            '#options' => $fields,
            '#default_value' => (isset($data['actuality_check_set']['field'])) ? $data['actuality_check_set']['field'] : false,
        ),
    );
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save'), '#weight' => '1000');
    return $form;
}

function xml_exchange_admin_exec_feed_batch_form($form, $form_state, $feed) {
    $form = array(
        '#tree' => TRUE,
        '#feed' => $feed,
    );

    $form['nid'] = array(
        '#type' => 'value',
        '#value' => $feed->id,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Начать',
    );
//    $question = t('Are you sure that you want to manually execute feed uploading: %link?', array('%link' => $feed->link));
//    $description = t('This will load data from remote feed to database');
//    return confirm_form($form, $question, 'admin/content/xml_exchange/', $description, t('Execute'));
    return $form;
}

function xml_exchange_admin_delete_feed_form($form, $form_state, $feed) {
    $form = array(
        '#tree' => TRUE,
        '#feed' => $feed,
    );

    $form['nid'] = array(
        '#type' => 'value',
        '#value' => $feed->id,
    );
    $question = t('Are you truly want to delete feed %link?', array('%link' => $feed->link));
    $description = t('This will remove feed and all data about it from database');
    return confirm_form($form, $question, 'admin/content/xml_exchange/' . $feed->id . '/edit', $description, t('Delete'));
}

function xml_exchange_admin_exec_feed_batch_form_submit($form, $form_state) {
    $feed = db_select('xml_exchange_loadings', 'x')
            ->fields('x')
            ->condition('id', $form_state['values']['nid'], '=')
            ->execute()
            ->fetchObject();
    if (!$feed) {
        drupal_set_message(t('Feed with this id doesn`t exists'), 'error', false);
        drupal_goto('admin/content/xml_exchange/');
    } else {
        $scheme = unserialize($feed->scheme);
        $node_fields = field_info_instances('node', $feed->type);
        $xml = simplexml_load_file($feed->link);
        if(isset($xml->{'generation-date'}) && $xml->{'generation-date'} == variable_get('xml_exchage_feed_gendate_'.$feed->id, 0)){
            drupal_set_message(t('Feed is up to date'));
            return 0;
        }
        $date = (string)$xml->{'generation-date'};
        variable_set('xml_exchage_feed_gendate_'.$feed->id, $date);
        foreach ($xml->{$feed->itemtag} AS $item) {
            $operations[] = array('xml_exchange_exec_item', array(json_encode($item), $scheme, $node_fields, $feed));
        }
        $batch = array(
            // Массив операций и их параметров
            'operations' => $operations,
            // Функция, которая будет выполнена после окончания всех операций
            'finished' => 'xml_exchange_admin_exec_feed_batch_form_submit_finished',
        );
        batch_set($batch);
    }
}

function xml_exchange_admin_edit_feed_form_submit($form, $form_state) {
    $check = db_select('xml_exchange_loadings', 'x')
            ->fields('x', array('id'))
            ->condition('id', $form_state['values']['nid'], '=')
            ->execute()
            ->fetchObject();
    if (!$check) {
        drupal_set_message(t('Feed with this id doesn`t exists'), 'error', false);
    } else {
        $data = serialize($form_state['values']);
        $updated = db_update('xml_exchange_loadings')
                ->fields(array('scheme' => $data))
                ->condition('id', $form_state['values']['nid'], '=')
                ->execute();
        if (!$updated)
            drupal_set_message(t('Something gone wrong. Please, connect site administrator'), 'error', false);
        else {
            drupal_set_message(t('Feed data updated'));
            drupal_goto('admin/content/xml_exchange/' . $form_state['values']['nid']);
        }
    }
}

function xml_exchange_admin_delete_feed_form_submit($form, $form_state) {
    $check = db_select('xml_exchange_loadings', 'x')
            ->fields('x', array('id'))
            ->condition('id', $form_state['values']['nid'], '=')
            ->execute()
            ->fetchObject();
    if (!$check) {
        drupal_set_message(t('Feed with this id doesn`t exists'), 'error', false);
    } else {
        db_delete('xml_exchange_loadings')
                ->condition('id', $check->id)
                ->execute();
        drupal_set_message(t('Feed deleted from db'));
        drupal_goto('admin/content/xml_exchange');
    }
    exit();
}

function xml_exchange_admin_add_feed_submit(&$form, $form_state) {
    $check = db_select('xml_exchange_loadings', 'x')
            ->fields('x', array('link'))
            ->condition('link', $form_state['values']['xml_exchange_link'], '=')
            ->execute()
            ->fetchCol('link');
    if (!$check) {
        $id = db_insert('xml_exchange_loadings')
                ->fields(array(
                    'uid' => $GLOBALS['user']->uid,
                    'link' => $form_state['values']['xml_exchange_link'],
                    'itemtag' => $form_state['values']['xml_exchange_itemtag'],
                    'type' => $form_state['values']['xml_exchange_node_type']
                ))
                ->execute();
        drupal_set_message(t('Feed added'));
        drupal_goto('admin/content/xml_exchange/' . $id . '/edit');
    } else {
        drupal_set_message(t('Feed with this link already exists'), 'error', false);
    }
}

function theme_xml_exchange_admin_content($vars) {
    $ex = $vars['ex'];
    if ($ex) {
        $header = array(
            t('Link'),
            array('data' => t('Operations'), 'colspan' => 3)
        );
        $rows = array();
        foreach ($ex AS $item) {
            $rows[] = array(
                $item->link,
                l(t('Execute'), 'admin/content/xml_exchange/' . $item->id),
                l(t('Edit'), 'admin/content/xml_exchange/' . $item->id . '/edit'),
                l(t('Delete'), 'admin/content/xml_exchange/' . $item->id . '/delete'),
            );
        }
        $res = theme('table', array('header' => $header, 'rows' => $rows));
    }
    else
        $res = t('There is nothing to render');
    return '<div id="action-links">' . l(t('Add xml feed'), 'admin/content/xml_exchange/add') . '</div>' . $res;
}