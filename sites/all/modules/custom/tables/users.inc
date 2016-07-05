<?php
/**
 * [tables_users_view description]
 * @return [table] [a table containing id,name,sex and role of users]
 */
function tables_users_view() {
	$path_curr = drupal_get_path_alias();
	// sortable first name
  $sort_order_value = isset($_GET['fname']) ? $_GET['fname'] : NULL;
  if (isset($sort_order_value)) {
    $sort_order = $sort_order_value == 'ASC' ? 'DESC' : 'ASC';
  }
  else {
    $sort_order = 'DESC';
  }
  //sortable last name
  $sort_order_value_lname = isset($_GET['lname']) ? $_GET['lname'] : NULL;
  if (isset($sort_order_value_lname)) {
    $sort_order_lname = $sort_order_value_lname == 'ASC' ? 'DESC' : 'ASC';
  }
  else {
    $sort_order_lname = 'ASC';
  }
  //sortable sex
  $sort_order_value_sex = isset($_GET['sex']) ? $_GET['sex'] : NULL;
  if (isset($sort_order_value_sex)) {
    $sort_order_sex = $sort_order_value_sex == 'ASC' ? 'DESC' : 'ASC';
  }
  else {
    $sort_order_sex = 'ASC';
  }
  $header = array('UID',l(t('FIRST NAME'), $path_curr, array('query' => array('fname' => $sort_order))),l(t('LAST NAME'), $path_curr, array('query' => array('lname' => $sort_order_lname))),l(t('SEX'), $path_curr, array('query' => array('sex' => $sort_order_sex))),'ROLE');
	$form_value = (drupal_get_form('tables_form'));
	$fname = $form_value['filter']['fname']['#value'];
	$lname = $form_value['filter']['lname']['#value'];
	$sex = $form_value['filter']['sex']['#value'];
	$query = new EntityFieldQuery();
	$query->entityCondition('entity_type', 'user');
	if (!empty($fname)) {
			$query->fieldCondition('field_first_name', 'value', "$fname", 'STARTS_WITH');
		}
		if (!empty($lname)) {
			$query->fieldCondition('field_last_name', 'value', "$lname", 'STARTS_WITH');
		}
		if (!empty($sex)) {
			$query->fieldCondition('field_sex', 'value', "$sex", 'STARTS_WITH');
		}
		if (!empty($sort_order_value_lname)) {
    $query->fieldOrderBy('field_last_name', 'value', "$sort_order_value_lname");
  	}
  	if (!empty($sort_order_value_sex)) {
    $query->fieldOrderBy('field_sex', 'value', "$sort_order_value_sex");
  	}
		if (empty($sort_order_value) && empty($sort_order_value_lname) && empty($sort_order_value_sex)) {
    $query->fieldOrderBy('field_first_name', 'value', "ASC");
  	}
    else {
    $query->fieldOrderBy('field_first_name', 'value', "$sort_order_value");
  	}
		$result = $query->execute();
		$uid = array_keys($result['user']);
		$users = user_load_multiple($uid);
		dpm($users);
		$rows = array();
		// $r = user_role_load_multiple($uid);
		// dpm($r);
		foreach ($users as $key => $value) {
			$row = array();
			$row[] = $value->uid;
			$alias = drupal_get_path_alias('user/'.$value->uid);
			$user_link = l(t($value->field_first_name['und'][0]['value']), $alias);
			$row[] = $user_link;
			$user_last_name = isset($value->field_last_name['und'][0]['value']) ? $value->field_last_name['und'][0]['value'] : ' ';
			$row[] = $user_last_name;
			$row[] = $value->field_sex['und'][0]['value'];
			foreach ($value->roles as $key1 => $value1) {
				if ($value1 != 'authenticated user') {
					$role = $value1;
				}
			}
			$row[] = $role;
			$rows[] = $row;
		}
		$form = drupal_get_form('tables_form');
    $output = drupal_render($form);
		$output .= theme('table', array('header' => $header, 'rows' => $rows));
		return $output;
	}
	function tables_form($form, &$form_state) {
		$form['filter'] = array(
			'#type' => 'fieldset',
			'#title' => t('FILTER YOUR RESULT'),
    '#collapsible' => TRUE, // Added
    '#collapsed' => FALSE,  // Added
    );
		$form['filter']['fname'] = array(
			'#type' => 'textfield',
			'#title' => t('First name'),
			'#description' => 'first name of user.',
			'#size' => 25,
			'#maxlength' => 15,
			'#minlenght' => 2,
    //'#required' => TRUE, // Added
			);
		$form['filter']['lname'] = array(
			'#type' => 'textfield',
			'#title' => t('Last name'),
			'#description' => 'last name of user.',
			'#size' => 25,
			'#maxlength' => 15,
			'#minlenght' => 2,
    //'#required' => TRUE, // Added
			);
	# the values for the dropdown box
		$form['type_options'] = array(
			'#type' => 'value',
			'#value' => array(0 => t('--SELECT--'),
				'm' => t('Male'),
				'f' => t('Female'),
				'o' => t('Other'))
			);
		$form['filter']['sex'] = array(
			'#title' => t('Sex'),
			'#type' => 'select',
			'#description' => "Select the sex of user.",
			'#options' => $form['type_options']['#value'],
			);
		$form['filter']['submit_button'] = array(
			'#type' => 'submit',
			'#value' => t('SUBMIT'),
			);
		return $form;
	}
	function tables_form_validate($form, &$form_state) {
	}

	function tables_form_submit($form, &$form_state) {
		$form_state['rebuild'] = TRUE;
	}


// drupal_goto("path_here", array("param1" => $fname, "param2" => $lname));
