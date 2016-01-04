<?php

function boa_job_list() {
	global $user;

	$header = array(
		array(
			'data' => t('Id'),
			'field' => 'id',
			'sort' => 'desc',
			'descending' => true,
		),
		array(
			'data' => t('Last Submitted'),
			'field' => 'submitted',
			'sort' => 'desc',
			'descending' => true,
		),
		array(
			'data' => t('Input Dataset'),
			'field' => 'name',
			'sort' => 'desc',
			'descending' => true,
		),
		array(
			'data' => t('Compile Status'),
			'field' => 'compiler_status',
			'sort' => 'desc',
			'descending' => true,
		),
		array(
			'data' => t('Run Status'),
			'field' => 'hadoop_status',
			'sort' => 'desc',
			'descending' => true,
		),
	);
	array_unshift($header, array('data' => t('User'), 'field' => 'u.name'));

	$deleted = is_boa_admin() && isset($_GET['deleted']) && $_GET['deleted'];

	$query = db_select('boa_jobs', 'j');
	$query->join('boa_output', 'o', 'j.id = o.id');
	$query->join('boa_input', 'i', 'j.input = i.id');
	$query->join('users', 'u', 'j.uid = u.uid');
	$query->fields('j', array('id', 'submitted', 'deleted', 'compiler_status', 'hadoop_status'));
	$query->fields('i', array('name', 'enabled'));
	$query->addField('u', 'name', 'username');
	$query->addField('i', 'id', 'inputid');
	$query->addExpression('COALESCE(o.length, 0) > 0', 'has_result');
	$query->addExpression('COALESCE(o.length, 0)', 'length');
	$query->addExpression('substring_index(source, \'\n\', 20)', 'source');
	$query->addExpression('unix_timestamp(compile_end)', 'compile_end');
	$query->addExpression('unix_timestamp(hadoop_end)', 'hadoop_end');
	$query->condition('deleted', false);
	$user_data = field_get_items('user', user_load($user->uid), 'field_table_rows');
	$query->condition('j.public', true);
	$user_data = field_get_items('user', user_load($user->uid), 'field_table_rows');
	$maxRows = empty($user_data) ? 10 : $user_data[0]['value'];
	$query = $query->extend('TableSort')->orderByHeader($header)->extend('PagerDefault')->limit($maxRows);

	$result = $query->execute();

	$rows = array();
	while ($data = $result->fetchObject()) {
		$actionList = array();

		fakeRunning($data);

		$rowdata = array(
			'<a title="' . htmlspecialchars(trim($data->source)) . '" href="?q=boa/job/' . $data->id . '">' . $data->id . '</a>',
			$data->submitted,
			$data->name,
			showStatus($data->compiler_status),
			showStatus($data->hadoop_status),
		);
		array_unshift($rowdata, $data->username);
		$rows[] = $rowdata;
	}

	$options = array(
		'header' => $header,
		'rows' => $rows,
		'attributes' => array('width' => '100%'),
		'sticky' => true,
		'caption' => NULL,
		'colgroups' => array(),
		'empty' => t('No jobs have been submitted yet.')
	);

	return theme_table($options).theme('pager');
}

?>
