<?php
/**
 * [tables_users_view description]
 * @return [table] [a table containing id,name,sex and role of users]
 */
function tables_users_view() {
	$form_value = (drupal_get_form('tables_form'));
	dpm($form_value);
	$fname = $form_value['filter']['fname']['#value'];
	$lname = $form_value['filter']['lname']['#value'];
	$sex = $form_value['filter']['sex']['#value'];
	dpm($rows);
	dpm($fname);
	dpm($lname);
	dpm($sex);
	$query = new EntityFieldQuery();
	$query->entityCondition('entity_type', 'user');
	if (!empty($fname))
		{	dpm($fname);
			$query->fieldCondition('field_first_name', 'value', "$fname", 'STARTS_WITH');
		}
		if (!empty($lname)) {
			$query->fieldCondition('field_last_name', 'value', "$lname", 'STARTS_WITH');
		}
		if (!empty($sex)) {
			$query->fieldCondition('field_sex', 'value', "$sex", 'STARTS_WITH');
		}
		$result = $query->execute();
		$uid = array_keys($result['user']);
		$users = user_load_multiple($uid);
		dpm($users);
		$rows = array();
		foreach ($users as $key => $value) {
			$row = array();
			$row[] = $value->uid;
			$alias = drupal_get_path_alias('user/'.$value->uid);
			$user_link = l(t($value->field_first_name['und'][0]['value']), $alias);
			$row[] = $user_link;
			$row[] = $value->field_last_name['und'][0]['value'];
			$row[] = $value->field_sex['und'][0]['value'];
			foreach ($value->roles as $key1 => $value1) {
				if ($value1 != 'authenticated user') {
					$role = $value1;
				}
			}
			$row[] = $role;
			$rows[] = $row;
		}
		$header = array('UID','FIRST NAME','LAST NAME','SEX','ROLE');
		$out = drupal_render(drupal_get_form('tables_form'));
		$out .= theme('table', array('header' => $header, 'rows' => $rows));
		return $out;
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
