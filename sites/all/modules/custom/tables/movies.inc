<?php
/**
 * [tables_movies_view description]
 * @return [type] [description]
 */
function tables_movies_view() {
  $path_curr = drupal_get_path_alias();
  $sort_order_value = isset($_GET['title']) ? $_GET['title'] : NULL;
  if (isset($sort_order_value)) {
    $sort_order = $sort_order_value == 'ASC' ? 'DESC' : 'ASC';
  }
  else {
    $sort_order = 'DESC';
  }
  // $sort_order_value_rate = isset($_GET['rating']) ? $_GET['rating'] : NULL;
  // if (isset($sort_order_value_rate)) {
  //   $sort_order_rate = $sort_order_value_rate == 'ASC' ? 'DESC' : 'ASC';
  // }
  // else {
  //   $sort_order_rate = 'DESC';
  // }
  $header = array('NID', l(t('TITLE'), $path_curr, array('query' => array('title' => $sort_order))), 'DESCRIPTION', 'PRODUCER', 'DIRECTOR', 'ACTOR', 'ACTRESS', 'GENRE', 'LANGUAGE', l(t('MOVIE RATING'), $path_curr, array('query' => array('rating' => '$sort_order_rate'))),);
  $form_value = (drupal_get_form('tables_form'));
  $mov_genre = $form_value['filter']['m_genre']['#value'];
  $mov_sort = $form_value['filter']['m_sort']['#value'];
  $mov_name = $form_value['filter']['m_name']['#value'];
  $names = $form_value['filter']['names']['#value'];
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
  ->entityCondition('bundle', 'movie_info');
  if(!empty($mov_genre)) {
    $query->fieldCondition('field_genre', 'tid', "$mov_genre");
  }
  if(!empty($mov_sort)) {
    $query->propertyOrderBy('title', "$mov_sort");
  }
  if (empty($sort_order_value)) {
    $query->propertyOrderBy('title', "ASC");
  }
  else {
    $query->propertyOrderBy('title', "$sort_order_value");
  }
  // if (empty($sort_order_value_rate)) {
  //   $query->fieldOrderBy('field_rating', 'value', "ASC");
  // }
  // else {
  //   $query->fieldOrderBy('field_rating', 'value', "$sort_order_value_rate");
  // }
  if(!empty($names) && !empty($mov_name)) {
    if ($names != 'movie') {
      $name = user_load_by_name($mov_name);
      $user_id = $name->uid;
    }
    if ($names == 'movie') {
      $query->propertyCondition('title', "$mov_name", 'STARTS_WITH');
    }
    elseif ($names == 'actor') {
$query->fieldCondition('field_actor', 'target_id', "$user_id");
    }
    elseif ($names == 'actress') {
$query->fieldCondition('field_actress', 'target_id', "$user_id");
    }
    elseif ($names == 'director') {
$query->fieldCondition('field_director', 'target_id', "$user_id");
    }
    elseif ($names == 'producer') {
$query->fieldCondition('field_producer', 'target_id', "$user_id");
    }
  }
  $result = $query->execute();
  if (isset($result['node'])) {
    $nids = array_keys($result['node']);
    $nodes = node_load_multiple($nids);
  }
  dpm($nodes);
  $rows = array();
  foreach ($nodes as $key => $value) {
    $row = array();
    $row[] = $value->nid;
    $alias = drupal_get_path_alias('node/'.$value->nid);
    $movie_link = l(t($value->title), $alias);
    $row[] = $movie_link;
    $movie_full = l(t('....See More'), $alias);
    $row[] = substr($value->body['und'][0]['value'], 0, 150).'&nbsp'.$movie_full;
    $pro = $value->field_producer['und'][0]['target_id'];
    $producer = user_load($pro);
    $alias = drupal_get_path_alias('user/'.$pro);
    $producer_last_name = isset($producer->field_last_name['und'][0]['value']) ? $producer->field_last_name['und'][0]['value'] : '';
    $producer_link = l(t($producer->field_first_name['und'][0]['value'].' '.$producer_last_name), $alias);
    $row[] = $producer_link;
    $dir = $value->field_director['und'][0]['target_id'];
    $director = user_load($dir);
    $alias = drupal_get_path_alias('user/'.$dir);
    $director_last_name = isset($director->field_last_name['und'][0]['value']) ? $director->field_last_name['und'][0]['value'] : '';
    $director_link = l(t($director->field_first_name['und'][0]['value'].' '.$director_last_name), $alias);
    $row[] = $director_link;
    $actor = $value->field_actor['und'][0]['target_id'];
    $actor1 = user_load($actor);
    $alias = drupal_get_path_alias('user/'.$actor);
    $actor_last_name = isset($actor1->field_last_name['und'][0]['value']) ? $actor1->field_last_name['und'][0]['value'] : '';
    $actor_link = l(t($actor1->field_first_name['und'][0]['value'].' '.$actor_last_name), $alias);
    $row[] = $actor_link;
    $actress = $value->field_actress['und'][0]['target_id'];
    $actress1 = user_load($actress);
    $alias = drupal_get_path_alias('user/'.$actress);
    $actress_last_name = isset($actress1->field_last_name['und'][0]['value']) ? $actress1->field_last_name['und'][0]['value'] : '';
    $actress_link = l(t($actress1->field_first_name['und'][0]['value'].' '.$actress_last_name), $alias);
    $row[] = $actress_link;
    $genre = $value->field_genre['und'][0]['tid'];
    $alias = drupal_get_path_alias('taxonomy/term/'.$genre);
    $terms = taxonomy_term_load($genre);
    $genre_link = l(t($terms->name), $alias);
    $row[] = $genre_link;
    $lang = $value->field_language['und'][0]['tid'];
    $language = taxonomy_term_load($lang);
    $row[] = $language->name;
    $row[] = $value->field_rating['und'][0]['value'];
    $rows[$value->nid] = $row;
  }
  $form = drupal_get_form('tables_form');
  $output = drupal_render($form);
  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}
function tables_form($form, &$form_state) {
  $query1 = new EntityFieldQuery();
  $query1->entityCondition('entity_type', 'taxonomy_term')
  ->entityCondition('bundle', array('movie_genres'))
  ->propertyOrderBy('weight');
  $result1 = $query1->execute();
  $tids = array_keys($result1['taxonomy_term']);
  $terms = taxonomy_term_load_multiple($tids);
  $arr = array();
  $arr[0] = '--SELECT--';
  foreach ($terms as $v) {
    $arr[$v->tid] = $v->name;
  }
  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('FILTER YOUR RESULT'),
    '#collapsible' => TRUE, // Added
    '#collapsed' => FALSE,  // Added
    );
  $form['filter']['m_genre'] = array(
    '#type' => 'select',
    '#title' => t('Movie Genre'),
    //'#multiple' => true,
    '#options' => $arr,
    '#description' => t('Select the movie genre'),
    );
  $form['filter']['names'] = array(
   '#title' => t('Search By Name'),
   '#type' => 'select',
   '#description' => 'search by movie or actor or actress or producer or director name',
   '#options' => array(0 => t('--- SELECT ---'), 'movie' => t('Movie'), 'actor' => t('Actor'), 'actress' => t('Actress'), 'director' => t('Director'), 'producer' => t('Producer')),
   );
  $form['filter']['m_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => 'Select a Particular Movie or user',
    '#size' => 25,
    '#maxlength' => 15,
    '#minlenght' => 2,
    //'#required' => TRUE, // Added
    );
  $form['filter']['m_sort'] = array(
   '#title' => t('Sort By Movie'),
   '#type' => 'select',
   '#description' => 'sort asc or desc by movie name',
   '#options' => array(0 => t('--- SELECT ---'), 'ASC' => t('Ascending'), 'DESC' => t('Descending')),
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






