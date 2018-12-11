<?php

/**
 * Implements hook_views_default_views().
 */
function student_views_default_views() {
$view = new view();
$view->name = 'students';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'student';
$view->human_name = 'Students';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Students';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'create student entities';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['query_comment'] = FALSE;
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'student_id' => 'student_id',
  'student_first_name' => 'student_first_name',
  'student_last_name' => 'student_last_name',
  'student_email' => 'student_email',
  'field_student_phone' => 'field_student_phone',
  'name' => 'name',
  'created' => 'created',
  'nothing' => 'nothing',
  'student_pursuit_reference' => 'student_pursuit_reference',
  'student_teacher_references' => 'student_teacher_references',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'student_id' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'student_first_name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'student_last_name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'student_email' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_student_phone' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'created' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'nothing' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'student_pursuit_reference' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'student_teacher_references' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Header: Global: Text area */
$handler->display->display_options['header']['area']['id'] = 'area';
$handler->display->display_options['header']['area']['table'] = 'views';
$handler->display->display_options['header']['area']['field'] = 'area';
$handler->display->display_options['header']['area']['content'] = '<a href="<?php print url(\'student/add\'); ?>>Add Student</a>';
$handler->display->display_options['header']['area']['format'] = 'full_html';
/* Relationship: Student: Author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'student';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Field: Student: Student ID */
$handler->display->display_options['fields']['student_id']['id'] = 'student_id';
$handler->display->display_options['fields']['student_id']['table'] = 'student';
$handler->display->display_options['fields']['student_id']['field'] = 'student_id';
/* Field: Student: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'student';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = 'Author';
/* Field: Student: Date created */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'student';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Created';
$handler->display->display_options['fields']['created']['date_format'] = 'short';
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = 'Edit';
$handler->display->display_options['fields']['nothing']['alter']['text'] = 'Edit';
$handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['nothing']['alter']['path'] = 'student/[student_id]/edit';
/* Field: Student: Student email */
$handler->display->display_options['fields']['student_email']['id'] = 'student_email';
$handler->display->display_options['fields']['student_email']['table'] = 'field_data_student_email';
$handler->display->display_options['fields']['student_email']['field'] = 'student_email';
/* Field: Student: Student first name */
$handler->display->display_options['fields']['student_first_name']['id'] = 'student_first_name';
$handler->display->display_options['fields']['student_first_name']['table'] = 'field_data_student_first_name';
$handler->display->display_options['fields']['student_first_name']['field'] = 'student_first_name';
/* Field: Student: Student last name */
$handler->display->display_options['fields']['student_last_name']['id'] = 'student_last_name';
$handler->display->display_options['fields']['student_last_name']['table'] = 'field_data_student_last_name';
$handler->display->display_options['fields']['student_last_name']['field'] = 'student_last_name';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'student';
$handler->display->display_options['menu']['type'] = 'normal';
$handler->display->display_options['menu']['title'] = 'Students';
$handler->display->display_options['menu']['weight'] = '0';

/* Display: Student admin */
$handler = $view->new_display('page', 'Student admin', 'page_1');
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'views_aggregator';
$handler->display->display_options['style_options']['columns'] = array(
  'student_id' => 'student_id',
  'student_first_name' => 'student_first_name',
  'student_last_name' => 'student_last_name',
  'field_student_family' => 'field_student_family',
  'teacher_first_name' => 'teacher_first_name',
  'teacher_last_name' => 'teacher_last_name',
  'student_email' => 'student_email',
  'field_student_phone' => 'field_student_phone',
  'student_total_points' => 'student_total_points',
  'nothing' => 'nothing',
);
$handler->display->display_options['style_options']['default'] = 'student_last_name';
$handler->display->display_options['style_options']['info'] = array(
  'student_id' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'has_aggr' => 1,
    'aggr' => array(
      'views_aggregator_group_and_compress' => 'views_aggregator_group_and_compress',
    ),
    'aggr_par' => '',
    'has_aggr_column' => 0,
    'aggr_column' => 'views_aggregator_sum',
    'aggr_par_column' => '',
  ),
  'student_first_name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'has_aggr' => 0,
    'aggr' => array(
      'views_aggregator_first' => 'views_aggregator_first',
    ),
    'aggr_par' => '',
    'has_aggr_column' => 0,
    'aggr_column' => 'views_aggregator_sum',
    'aggr_par_column' => '',
  ),
  'student_last_name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'has_aggr' => 0,
    'aggr' => array(
      'views_aggregator_first' => 'views_aggregator_first',
    ),
    'aggr_par' => '',
    'has_aggr_column' => 0,
    'aggr_column' => 'views_aggregator_sum',
    'aggr_par_column' => '',
  ),
  'field_student_family' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'has_aggr' => 0,
    'aggr' => array(
      'views_aggregator_first' => 'views_aggregator_first',
    ),
    'aggr_par' => '',
    'has_aggr_column' => 0,
    'aggr_column' => 'views_aggregator_sum',
    'aggr_par_column' => '',
  ),
  'teacher_first_name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'has_aggr' => 0,
    'aggr' => array(
      'views_aggregator_first' => 'views_aggregator_first',
    ),
    'aggr_par' => '',
    'has_aggr_column' => 0,
    'aggr_column' => 'views_aggregator_sum',
    'aggr_par_column' => '',
  ),
  'teacher_last_name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'has_aggr' => 0,
    'aggr' => array(
      'views_aggregator_first' => 'views_aggregator_first',
    ),
    'aggr_par' => '',
    'has_aggr_column' => 0,
    'aggr_column' => 'views_aggregator_sum',
    'aggr_par_column' => '',
  ),
  'student_email' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'has_aggr' => 0,
    'aggr' => array(
      'views_aggregator_first' => 'views_aggregator_first',
    ),
    'aggr_par' => '',
    'has_aggr_column' => 0,
    'aggr_column' => 'views_aggregator_sum',
    'aggr_par_column' => '',
  ),
  'field_student_phone' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'has_aggr' => 0,
    'aggr' => array(
      'views_aggregator_first' => 'views_aggregator_first',
    ),
    'aggr_par' => '',
    'has_aggr_column' => 0,
    'aggr_column' => 'views_aggregator_sum',
    'aggr_par_column' => '',
  ),
  'student_total_points' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'has_aggr' => 0,
    'aggr' => array(
      'views_aggregator_first' => 'views_aggregator_first',
    ),
    'aggr_par' => '',
    'has_aggr_column' => 0,
    'aggr_column' => 'views_aggregator_sum',
    'aggr_par_column' => '',
  ),
  'nothing' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'has_aggr' => 0,
    'aggr' => array(
      'views_aggregator_first' => 'views_aggregator_first',
    ),
    'aggr_par' => '',
    'has_aggr_column' => 0,
    'aggr_column' => 'views_aggregator_sum',
    'aggr_par_column' => '',
  ),
);
$handler->display->display_options['style_options']['column_aggregation']['totals_per_page'] = '1';
$handler->display->display_options['style_options']['column_aggregation']['totals_row_position'] = array(
  1 => 0,
  2 => '2',
);
$handler->display->display_options['style_options']['column_aggregation']['precision'] = '2';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['relationships'] = FALSE;
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['student_teacher_references_target_id']['id'] = 'student_teacher_references_target_id';
$handler->display->display_options['relationships']['student_teacher_references_target_id']['table'] = 'field_data_student_teacher_references';
$handler->display->display_options['relationships']['student_teacher_references_target_id']['field'] = 'student_teacher_references_target_id';
$handler->display->display_options['relationships']['student_teacher_references_target_id']['label'] = 'Teacher';
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Student: Student ID */
$handler->display->display_options['fields']['student_id']['id'] = 'student_id';
$handler->display->display_options['fields']['student_id']['table'] = 'student';
$handler->display->display_options['fields']['student_id']['field'] = 'student_id';
$handler->display->display_options['fields']['student_id']['separator'] = '';
/* Field: Student: Student first name */
$handler->display->display_options['fields']['student_first_name']['id'] = 'student_first_name';
$handler->display->display_options['fields']['student_first_name']['table'] = 'field_data_student_first_name';
$handler->display->display_options['fields']['student_first_name']['field'] = 'student_first_name';
/* Field: Student: Student last name */
$handler->display->display_options['fields']['student_last_name']['id'] = 'student_last_name';
$handler->display->display_options['fields']['student_last_name']['table'] = 'field_data_student_last_name';
$handler->display->display_options['fields']['student_last_name']['field'] = 'student_last_name';
/* Field: Student: Family tag */
$handler->display->display_options['fields']['field_student_family']['id'] = 'field_student_family';
$handler->display->display_options['fields']['field_student_family']['table'] = 'field_data_field_student_family';
$handler->display->display_options['fields']['field_student_family']['field'] = 'field_student_family';
$handler->display->display_options['fields']['field_student_family']['label'] = 'Family';
$handler->display->display_options['fields']['field_student_family']['settings'] = array(
  'bypass_access' => 0,
  'link' => 0,
);
/* Field: Teacher: Teacher first name */
$handler->display->display_options['fields']['teacher_first_name']['id'] = 'teacher_first_name';
$handler->display->display_options['fields']['teacher_first_name']['table'] = 'field_data_teacher_first_name';
$handler->display->display_options['fields']['teacher_first_name']['field'] = 'teacher_first_name';
$handler->display->display_options['fields']['teacher_first_name']['relationship'] = 'student_teacher_references_target_id';
/* Field: Teacher: Teacher last name */
$handler->display->display_options['fields']['teacher_last_name']['id'] = 'teacher_last_name';
$handler->display->display_options['fields']['teacher_last_name']['table'] = 'field_data_teacher_last_name';
$handler->display->display_options['fields']['teacher_last_name']['field'] = 'teacher_last_name';
$handler->display->display_options['fields']['teacher_last_name']['relationship'] = 'student_teacher_references_target_id';
$handler->display->display_options['fields']['teacher_last_name']['label'] = 'Teacher l. name';
/* Field: Student: Student email */
$handler->display->display_options['fields']['student_email']['id'] = 'student_email';
$handler->display->display_options['fields']['student_email']['table'] = 'field_data_student_email';
$handler->display->display_options['fields']['student_email']['field'] = 'student_email';
/* Field: Student: Student phone */
$handler->display->display_options['fields']['field_student_phone']['id'] = 'field_student_phone';
$handler->display->display_options['fields']['field_student_phone']['table'] = 'field_data_field_student_phone';
$handler->display->display_options['fields']['field_student_phone']['field'] = 'field_student_phone';
/* Field: Student: Accumulated points */
$handler->display->display_options['fields']['student_total_points']['id'] = 'student_total_points';
$handler->display->display_options['fields']['student_total_points']['table'] = 'field_data_student_total_points';
$handler->display->display_options['fields']['student_total_points']['field'] = 'student_total_points';
$handler->display->display_options['fields']['student_total_points']['label'] = 'Points';
$handler->display->display_options['fields']['student_total_points']['settings'] = array(
  'thousand_separator' => '',
  'prefix_suffix' => 1,
);
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = 'Edit';
$handler->display->display_options['fields']['nothing']['alter']['text'] = 'Edit';
$handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['nothing']['alter']['path'] = 'student/[student_id]/edit?destination=/admin/student';
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Student: Student ID */
$handler->display->display_options['arguments']['student_id']['id'] = 'student_id';
$handler->display->display_options['arguments']['student_id']['table'] = 'student';
$handler->display->display_options['arguments']['student_id']['field'] = 'student_id';
$handler->display->display_options['arguments']['student_id']['title_enable'] = TRUE;
$handler->display->display_options['arguments']['student_id']['title'] = 'Student [student_last_name]';
$handler->display->display_options['arguments']['student_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['student_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['student_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['student_id']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['path'] = 'admin/student';

/* Display: User students */
$handler = $view->new_display('page', 'User students', 'page_2');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'My students';
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['style_options']['row_class'] = 'individuallineitem';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['row_options']['hide_empty'] = TRUE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['header'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Student: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'student';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['element_wrapper_type'] = 'h2';
/* Field: Student: Student ID */
$handler->display->display_options['fields']['student_id']['id'] = 'student_id';
$handler->display->display_options['fields']['student_id']['table'] = 'student';
$handler->display->display_options['fields']['student_id']['field'] = 'student_id';
$handler->display->display_options['fields']['student_id']['element_type'] = 'span';
$handler->display->display_options['fields']['student_id']['element_label_class'] = 'titleoflineitem';
/* Field: Student: Student first name */
$handler->display->display_options['fields']['student_first_name']['id'] = 'student_first_name';
$handler->display->display_options['fields']['student_first_name']['table'] = 'field_data_student_first_name';
$handler->display->display_options['fields']['student_first_name']['field'] = 'student_first_name';
$handler->display->display_options['fields']['student_first_name']['element_type'] = 'span';
$handler->display->display_options['fields']['student_first_name']['element_label_class'] = 'titleoflineitem';
/* Field: Student: Student last name */
$handler->display->display_options['fields']['student_last_name']['id'] = 'student_last_name';
$handler->display->display_options['fields']['student_last_name']['table'] = 'field_data_student_last_name';
$handler->display->display_options['fields']['student_last_name']['field'] = 'student_last_name';
$handler->display->display_options['fields']['student_last_name']['element_type'] = 'span';
$handler->display->display_options['fields']['student_last_name']['element_label_class'] = 'titleoflineitem';
/* Field: Student: Student email */
$handler->display->display_options['fields']['student_email']['id'] = 'student_email';
$handler->display->display_options['fields']['student_email']['table'] = 'field_data_student_email';
$handler->display->display_options['fields']['student_email']['field'] = 'student_email';
$handler->display->display_options['fields']['student_email']['element_type'] = 'span';
$handler->display->display_options['fields']['student_email']['element_label_class'] = 'titleoflineitem';
/* Field: Student: Student phone */
$handler->display->display_options['fields']['field_student_phone']['id'] = 'field_student_phone';
$handler->display->display_options['fields']['field_student_phone']['table'] = 'field_data_field_student_phone';
$handler->display->display_options['fields']['field_student_phone']['field'] = 'field_student_phone';
$handler->display->display_options['fields']['field_student_phone']['element_type'] = 'span';
$handler->display->display_options['fields']['field_student_phone']['element_label_class'] = 'titleoflineitem';
/* Field: Student: Years with current teacher */
$handler->display->display_options['fields']['field_years_with_current_teacher']['id'] = 'field_years_with_current_teacher';
$handler->display->display_options['fields']['field_years_with_current_teacher']['table'] = 'field_data_field_years_with_current_teacher';
$handler->display->display_options['fields']['field_years_with_current_teacher']['field'] = 'field_years_with_current_teacher';
$handler->display->display_options['fields']['field_years_with_current_teacher']['element_type'] = 'span';
$handler->display->display_options['fields']['field_years_with_current_teacher']['element_label_class'] = 'titleoflineitem';
$handler->display->display_options['fields']['field_years_with_current_teacher']['settings'] = array(
  'thousand_separator' => '',
  'prefix_suffix' => 1,
);
/* Field: Student: Years with previous teacher */
$handler->display->display_options['fields']['field_years_with_previous_teache']['id'] = 'field_years_with_previous_teache';
$handler->display->display_options['fields']['field_years_with_previous_teache']['table'] = 'field_data_field_years_with_previous_teache';
$handler->display->display_options['fields']['field_years_with_previous_teache']['field'] = 'field_years_with_previous_teache';
$handler->display->display_options['fields']['field_years_with_previous_teache']['element_type'] = 'span';
$handler->display->display_options['fields']['field_years_with_previous_teache']['element_label_class'] = 'titleoflineitem';
$handler->display->display_options['fields']['field_years_with_previous_teache']['settings'] = array(
  'thousand_separator' => '',
  'prefix_suffix' => 1,
);
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = 'Author';
$handler->display->display_options['fields']['name']['element_type'] = 'span';
$handler->display->display_options['fields']['name']['element_label_class'] = 'titleoflineitem';
/* Field: Student: Date created */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'student';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Created';
$handler->display->display_options['fields']['created']['element_type'] = 'span';
$handler->display->display_options['fields']['created']['element_label_class'] = 'titleoflineitem';
$handler->display->display_options['fields']['created']['date_format'] = 'short';
$handler->display->display_options['fields']['created']['second_date_format'] = 'long';
/* Field: Student: Pursuit reference */
$handler->display->display_options['fields']['student_pursuit_reference']['id'] = 'student_pursuit_reference';
$handler->display->display_options['fields']['student_pursuit_reference']['table'] = 'field_data_student_pursuit_reference';
$handler->display->display_options['fields']['student_pursuit_reference']['field'] = 'student_pursuit_reference';
$handler->display->display_options['fields']['student_pursuit_reference']['label'] = 'Registration ID(s)';
$handler->display->display_options['fields']['student_pursuit_reference']['element_type'] = 'span';
$handler->display->display_options['fields']['student_pursuit_reference']['element_label_class'] = 'titleoflineitem';
$handler->display->display_options['fields']['student_pursuit_reference']['empty'] = 'No current registrations';
$handler->display->display_options['fields']['student_pursuit_reference']['hide_alter_empty'] = FALSE;
$handler->display->display_options['fields']['student_pursuit_reference']['type'] = 'entityreference_entity_id';
$handler->display->display_options['fields']['student_pursuit_reference']['settings'] = array(
  'bypass_access' => 0,
  'link' => 0,
);
$handler->display->display_options['fields']['student_pursuit_reference']['delta_offset'] = '0';
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = '';
$handler->display->display_options['fields']['nothing']['alter']['text'] = 'Edit or delete';
$handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['nothing']['alter']['path'] = 'student/[student_id]/edit?destination=/my-students';
$handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
$handler->display->display_options['defaults']['sorts'] = FALSE;
/* Sort criterion: Student: Student last name (student_last_name) */
$handler->display->display_options['sorts']['student_last_name_value']['id'] = 'student_last_name_value';
$handler->display->display_options['sorts']['student_last_name_value']['table'] = 'field_data_student_last_name';
$handler->display->display_options['sorts']['student_last_name_value']['field'] = 'student_last_name_value';
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Student: Author */
$handler->display->display_options['arguments']['uid']['id'] = 'uid';
$handler->display->display_options['arguments']['uid']['table'] = 'student';
$handler->display->display_options['arguments']['uid']['field'] = 'uid';
$handler->display->display_options['arguments']['uid']['default_action'] = 'default';
$handler->display->display_options['arguments']['uid']['default_argument_type'] = 'current_user';
$handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['path'] = 'my-students';




  $views['students'] = $view;

  return $views;
}