<?php

/**
 * [tables_songs_view description]
 * @return [type] [description]
 */
function tables_songs_view() {
  $path_curr = drupal_get_path_alias();
  $sort_order_value = isset($_GET['title']) ? $_GET['title'] : NULL;
 $sort_order_value_rate = isset($_GET['rating']) ? $_GET['rating'] : NULL;
  if (isset($sort_order_value)) {
    $sort_order = $sort_order_value == 'ASC' ? 'DESC' : 'ASC';
  }
  else {
    $sort_order = 'DESC';
  }
  if (isset($sort_order_value_rate)) {
    $sort_order_rate = $sort_order_value_rate == 'ASC' ? 'DESC' : 'ASC';
  }
  else {
    $sort_order_rate = 'ASC';
  }
  $header = array('NID', l(t('SONG NAME'), $path_curr, array('query' => array('title' => $sort_order))),'GENRE','MOVIE NAME','MOVIE DESCRIPTION','SINGER',l(t('SONG RATING'), $path_curr, array('query' => array('rating' => $sort_order_rate))),);
  $form_value = (drupal_get_form('tables_form'));
  $sng_genre = $form_value['filter']['s_genre']['#value'];
  $sng_sort = $form_value['filter']['s_sort']['#value'];
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
  if (!empty($sort_order_value_rate)  && empty($sort_order_value_rate)) {
    $query->fieldOrderBy('field_rating', 'value', "$sort_order_value_rate");
  }
  $result = $query->execute();
  if (isset($result['node'])) {
    $nids = array_keys($result['node']);
    $nodes = node_load_multiple($nids);
  }
  $rows = array();
  foreach ($nodes as $key => $value) {
    $row = array();
    //get node id
    $row[] = $value->nid;
    //get song name
    $alias = drupal_get_path_alias('node/'.$value->nid);
    $song_link = l(t($value->title), $alias);
    $row[] = $song_link;
    //get song genre by taxonomy songs_genre
    $genre = $value->field_song_genre['und'][0]['tid'];
    $alias = drupal_get_path_alias('taxonomy/term/'.$genre);
    $genre1 = taxonomy_term_load($genre);
    $genre_link = l(t($genre1->name), $alias);
    $row[] = $genre_link;
    //get movie title of song
    $movie = $value->field_movie['und'][0]['target_id'];
    $movie1 = node_load($movie);
    $alias = drupal_get_path_alias('node/'.$movie);
    $movie_link = l(t($movie1->title), $alias);
    $row[] = $movie_link;
    //get movie description
    $movie_full = l(t('....See More'), $alias);
    $row[] = substr($movie1->body['und'][0]['value'], 0, 150).'&nbsp'.$movie_full;
    //get singers' names
    $singer_link_arr = array();
    $singer_array = isset($value->field_singer['und']) ? $value->field_singer['und'] : NULL;
    foreach ($singer_array as $key1 => $value1) {
      $singer = user_load($value1['target_id']);
      $alias = drupal_get_path_alias('user/'.$value1['target_id']);
      $singer_last_name = isset($singer->field_last_name['und'][0]['value']) ? $singer->field_last_name['und'][0]['value'] : '';
      $singer_link = l(t($singer->field_first_name['und'][0]['value'].' '.$singer_last_name), $alias);
      $singer_link_arr[] = $singer_link;
    }
    $multi_val_singer = implode(', ', $singer_link_arr);
    $row[] = $multi_val_singer;
    //get song rating
    $row[] = $value->field_rating['und'][0]['value'];
    $rows[] = $row;
  }
  $form = drupal_get_form('tables_form');
  $output = drupal_render($form);
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
