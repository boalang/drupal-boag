<?php

function boa_job_list() {
	global $user;

	$header = array(
		array(
			'data' => t('User'),
			'field' => 'u.name',
			'descending' => false,
		),
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
	);

	$query = db_select('boa_jobs', 'j');

	$query->join('boa_output', 'o', 'j.id = o.id');
	$query->join('boa_input', 'i', 'j.input = i.id');
	$query->join('users', 'u', 'j.uid = u.uid');

	$query->fields('j', array('id', 'submitted'));
	$query->fields('i', array('name'));
	$query->addField('u', 'name', 'username');
	$query->addExpression('substring_index(source, \'\n\', 20)', 'source');
	$query->condition('deleted', false);
	$query->condition('public', true);
	$query->condition('hadoop_status', 2);
	$query->where('COALESCE(o.length, 0) > 0');

	$user_data = field_get_items('user', user_load($user->uid), 'field_table_rows');
	$maxRows = empty($user_data) ? 10 : $user_data[0]['value'];
	$query = $query->extend('TableSort')->orderByHeader($header)->extend('PagerDefault')->limit($maxRows);

	$result = $query->execute();

	$rows = array();
	while ($data = $result->fetchObject()) {
		fakeRunning($data);

		$rows[] = array(
			$data->username,
			'<a title="' . htmlspecialchars(trim($data->source)) . '" href="?q=boa/job/public/' . $data->id . '">' . $data->id . '</a>',
			$data->submitted,
			$data->name,
		);
	}

	$options = array(
		'header' => $header,
		'rows' => $rows,
		'attributes' => array('width' => '100%'),
		'sticky' => true,
		'caption' => NULL,
		'colgroups' => array(),
		'empty' => t('There are currently no matching public jobs.')
	);

	return theme_table($options).theme('pager');
}

?>
