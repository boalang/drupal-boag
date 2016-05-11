<?php

function boa_running() {
	global $user;

	$header = array(
		array(
			'data' => t('User'),
			'field' => 'u.name'
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
		array(
			'data' => t('Job Commands'),
		),
	);

	$query = db_select('boa_jobs', 'j');
	$query->join('users', 'u', 'j.uid = u.uid');
	$query->join('boa_input', 'i', 'j.input = i.id');
	$query->fields('j', array('id', 'submitted'));
	$query->fields('i', array('name'));
	$query->addField('u', 'name', 'username');
	$query->addExpression('substring_index(j.source, \'\n\', 20)', 'source');
	$query->condition('j.hadoop_status', '1');
	$query = $query->extend('TableSort')->orderByHeader($header)->extend('PagerDefault');

	$result = $query->execute();

	$rows = array();
	while ($data = $result->fetchObject())
		$rows[] = array(
			$data->username,
			'<a title="' . htmlspecialchars(trim($data->source)) . '" href="?q=boa/job/' . $data->id . '">' . $data->id . '</a>',
			$data->submitted,
			$data->name,
			"[<a href=\"?q=boa/job/{$data->id}/stop\">" . t('stop') . '</a>] [<a href="?q=boa/job/' . $data->id . '/status">' . t('status') . '</a>]',
		);

	$options = array(
		'header' => $header,
		'rows' => $rows,
		'attributes' => array('width' => '100%'),
		'sticky' => true,
		'caption' => NULL,
		'colgroups' => array(),
		'empty' => t('No jobs are currently running.')
	);

	return theme_table($options).theme('pager');
}

?>
