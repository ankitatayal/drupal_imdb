<?php

/**
 * [tables_songs_view description]
 * @return [type] [description]
 */
function tables_songs_view() {
  $path_curr = drupal_get_path_alias();
  $sort_order_value = isset($_GET['title']) ? $_GET['title'] : NULL;
 // $sort_order_value_rate = isset($_GET['rating']) ? $_GET['rating'] : NULL;
  if (isset($sort_order_value)) {
    $sort_order = $sort_order_value == 'ASC' ? 'DESC' : 'ASC';
  }
  else {
    $sort_order = 'DESC';
  }
  // if (isset($sort_order_value_rate)) {
  //   $sort_order_rate = $sort_order_value_rate == 'ASC' ? 'DESC' : 'ASC';
  // }
  // else {
  //   $sort_order_rate = 'DESC';
  // }
  $header = array('NID', l(t('SONG NAME'), $path_curr, array('query' => array('title' => $sort_order))),'GENRE','MOVIE NAME','MOVIE DESCRIPTION','SINGER',l(t('SONG RATING'), $path_curr, array('query' => array('rating' => '$sort_order_rate'))),);
  $form_value = (drupal_get_form('tables_form'));
  dpm($form_value);
  $sng_genre = $form_value['filter']['s_genre']['#value'];
  $sng_sort = $form_value['filter']['m_sort']['#value'];
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
  ->entityCondition('bundle', 'songs_info');
  if(!empty($sng_genre)) {
      $query->fieldCondition('field_genre', 'tid', "$sng_genre");
    }
  if(!empty($sng_sort)) {
      $query->propertyOrderBy('title',"$sng_sort");
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
  $result = $query->execute();
  if (isset($result['node'])) {
    $nids = array_keys($result['node']);
    $nodes = node_load_multiple($nids);
  }
  $rows = array();
  foreach ($nodes as $key => $value) {
    $row = array();
    $row[] = $value->nid;
    $alias = drupal_get_path_alias('node/'.$value->nid);
    $song_link = l(t($value->title), $alias);
    $row[] = $song_link;
    $genre = $value->field_song_genre['und'][0]['tid'];
    $alias = drupal_get_path_alias('taxonomy/term/'.$genre);
    $genre1 = taxonomy_term_load($genre);
    $genre_link = l(t($genre1->name), $alias);
    $row[] = $genre_link;
    $movie = $value->field_movie['und'][0]['target_id'];
    $movie1 = node_load($movie);
    $alias = drupal_get_path_alias('node/'.$movie);
    $movie_link = l(t($movie1->title), $alias);
    $row[] = $movie_link;
    $movie_full = l(t('....See More'), $alias);
    $row[] = substr($movie1->body['und'][0]['value'], 0, 150).'&nbsp'.$movie_full;
    $singer = $value->field_singer['und'][0]['target_id'];
    $alias = drupal_get_path_alias('user/'.$singer);
    $singer_link = l(t(user_load($singer)->name), $alias);
    $row[] = $singer_link;
    $row[] = $value->field_rating['und'][0]['value'];
    $rows[] = $row;
  }
  $output = drupal_render(drupal_get_form('tables_form'));
  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}
function tables_form($form, &$form_state) {
  $query1 = new EntityFieldQuery();
  $query1->entityCondition('entity_type', 'taxonomy_term')
  ->entityCondition('bundle', array('songs_genre'))
  ->propertyOrderBy('weight');
  $result1 = $query1->execute();
  $tids = array_keys($result1['taxonomy_term']);
  $terms = taxonomy_term_load_multiple($tids);
  $arr = array();
  $arr[0] = '--SELECT--';
  foreach ($terms as $v) {
    $arr[$v->tid] = $v->name;
  }
  dpm($arr);
  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('FILTER YOUR RESULT'),
    '#collapsible' => TRUE, // Added
    '#collapsed' => FALSE,  // Added
    );
    $form['filter']['s_genre'] = array(
    '#type' => 'select',
    '#title' => t('Song Genre'),
    //'#multiple' => true,
    '#options' => $arr,
    '#description' => t('select the song genre'),
  );
   $form['filter']['s_sort'] = array(
   '#title' => t('Sort By Song'),
   '#type' => 'select',
   '#description' => 'sort asc or desc by song name',
   '#options' => array(0 => t('-- SELECT --'), 'ASC' => t('Ascending'), 'DESC' => t('Descending')),
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
