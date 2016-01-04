<?php

function boa_search() {
	return drupal_get_form('search_form');
}

function search_form($form, &$form_state) {
	$form['search'] = array(
		'#type' => 'textfield',
		'#title' => t('Enter a Single Keyword or an Exact Phrase'),
	);

	$form['output'] = array(
		'#type' => 'checkbox',
		'#title' => t('Job has output data'),
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Search'),
	);

	return $form;
}

function search_form_submit($form, &$form_state) {
	if ($form['output']['#value'])
		drupal_goto('boa/search/!output!' . $form['search']['#value']);
	else
		drupal_goto('boa/search/' . $form['search']['#value']);
}

function boa_search_results($keyword) {
	global $user;

	$output = strpos($keyword, '!output!') === 0;
	if ($output)
		$keyword = substr($keyword, 8);

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
			'descending' => true,
		),
		array(
			'data' => t('Input Dataset'),
			'field' => 'name',
		),
		array(
			'data' => t('Compile Status'),
			'field' => 'compiler_status',
		),
		array(
			'data' => t('Run Status'),
			'field' => 'hadoop_status',
		),
		array(
			'data' => t('Job Commands'),
		),
	);

	$query = db_select('boa_jobs', 'j');
	$query->join('boa_output', 'o', 'j.id = o.id');
	$query->join('boa_input', 'i', 'j.input = i.id');
	$query->join('users', 'u', 'j.uid = u.uid');
	$query->fields('j', array('id', 'submitted', 'deleted', 'compiler_status', 'hadoop_status'));
	$query->fields('i', array('name'));
	$query->addField('u', 'name', 'username');
	$query->addExpression('COALESCE(o.length, 0) > 0', 'has_result');
	$query->addExpression('substring_index(source, \'\n\', 20)', 'source');
	$query->addExpression('unix_timestamp(compile_end)', 'compile_end');
	$query->addExpression('unix_timestamp(hadoop_end)', 'hadoop_end');
	if (!is_boa_admin()) {
		$query->condition('j.uid', $user->uid);
		$query->condition('deleted', false);
	} else {
		array_unshift($header, array('data' => t('User'), 'field' => 'u.name'));
	}
	$query->condition('source', '%' . $keyword . '%', 'LIKE');
	//$query->where("MATCH(source) AGAINST('" . mysql_escape_string($keyword) . "')");
	if ($output)
		$query->condition('o.length', 0, '>');
	$query = $query->extend('TableSort')->orderByHeader($header)->extend('PagerDefault')->limit(30);

	$result = $query->execute();

	$rows = array();
	while ($data = $result->fetchObject()) {
		$actions = array();

		fakeRunning($data);

		if ($data->compiler_status == 0 || $data->compiler_status == 1 || $data->hadoop_status == 1 ||
		    ($data->compiler_status == 1 && $data->hadoop_status == 0))
			$actions['stop'] = t('stop');
		else
			$actions['resubmit'] = t('resubmit');

		if (!$data->deleted)
			$actions['delete'] = t('delete');

		if ($data->has_result && $data->hadoop_status == 2)
			$actions['download'] = t('download output');

		$actionList = array();
		foreach ($actions as $key => $val)
			$actionList[] = "[<a href=\"?q=boa/job/{$data->id}/$key\">$val</a>]";

		$rowdata = array(
			'<a title="' . htmlspecialchars(trim($data->source)) . '" href="?q=boa/job/' . $data->id . '">' . $data->id . '</a>',
			$data->submitted,
			$data->name,
			showStatus($data->compiler_status),
			showStatus($data->hadoop_status),
			implode(' ', $actionList),
		);
		if (is_boa_admin())
			array_unshift($rowdata, $data->username);
		$rows[] = $rowdata;
	}

	$options = array(
		'header' => $header,
		'rows' => $rows,
		'attributes' => array('width' => '100%'),
		'sticky' => true,
		'caption' => t('Search results for: <b>:keyword</b>', array(':keyword' => $keyword)),
		'colgroups' => array(),
		'empty' => t('No jobs match this search criteria.')
	);

	return theme_table($options).theme('pager');
}

?>
