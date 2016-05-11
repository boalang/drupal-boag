<?php

function _job_rpc($id) {
	global $user;

	$query = jobs_query($user->uid);
	$query->condition('j.id', $id);

	$result = $query->execute();

	while ($data = $result->fetchObject()) {
		fakeRunning($data);

		return array(
			'id' => $data->id,
			'submitted' => $data->submitted . ' ' . strftime('%Z'),
			'input' => array('id' => $data->inputid, 'name' => $data->name),
			'compiler_status' => showStatus($data->compiler_status),
			'hadoop_status' => showStatus($data->hadoop_status),
		);
	}
}

function _jobs_count_rpc($public) {
	global $user;

	$query = db_select('boa_jobs', 'j');
	$query->addExpression('COUNT(id)', 'count');
	$query->condition('j.uid', $user->uid);
	$query->condition('deleted', false);
	if ($public)
		$query->condition('public', true);

	$result = $query->execute();
	$data = $result->fetchObject();
	return $data->count;
}

function jobs_query($uid) {
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
	if ($uid > 0)
		$query->condition('j.uid', $uid);
	if (!is_boa_admin() || !isset($_GET['deleted']) || !$_GET['deleted'])
		$query->condition('deleted', false);

	return $query;
}

function _jobs_range_rpc($public, $offset, $limit) {
	global $user;

	$query = jobs_query($user->uid);
	if ($public)
		$query->condition('public', true);
	$query->orderBy('j.id', 'DESC');
	$query->range($offset, $limit);

	$result = $query->execute();

	$rows = array();
	while ($data = $result->fetchObject()) {
		fakeRunning($data);

		$rows[] = array(
			'id' => $data->id,
			'submitted' => $data->submitted . ' ' . strftime('%Z'),
			'input' => array('id' => $data->inputid, 'name' => $data->name),
			'compiler_status' => showStatus($data->compiler_status),
			'hadoop_status' => showStatus($data->hadoop_status),
		);
	}

	return $rows;
}

function _jobs_rpc($public) {
	global $user;

	$user_data = field_get_items('user', user_load($user->uid), 'field_table_rows');
	$maxRows = empty($user_data) ? 25 : $user_data[0]['value'];
	return _jobs_range_rpc($public, 0, $maxRows);
}

function boa_jobs() {
	global $user;

	$uid = $user->uid;
	if (isset($_GET['userId']) && is_boa_admin())
		$uid = (int)$_GET['userId'];

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
		array(
			'data' => t('Job Commands'),
		),
	);
	if ($uid == 0)
		array_unshift($header, array('data' => t('User'), 'field' => 'u.name'));

	$deleted = is_boa_admin() && isset($_GET['deleted']) && $_GET['deleted'];

	$query = jobs_query($uid);
	$user_data = field_get_items('user', user_load($user->uid), 'field_table_rows');
	$maxRows = empty($user_data) ? 10 : $user_data[0]['value'];
	$query = $query->extend('TableSort')->orderByHeader($header)->extend('PagerDefault')->limit($maxRows);

	$result = $query->execute();

	$rows = array();
	while ($data = $result->fetchObject()) {
		$actionList = array();

		fakeRunning($data);

		if ($data->compiler_status == 0 || $data->compiler_status == 1 || $data->hadoop_status == 1 ||
		    ($data->compiler_status == 1 && $data->hadoop_status == 0))
			$actionList[] = "[<a href=\"?q=boa/job/{$data->id}/stop\">" . t('stop') . '</a>]';
		//else if ($data->enabled)
		//	$actionList[] = "[<a href=\"?q=boa/job/{$data->id}/resubmit\">" . t('resubmit') . '</a>]';

		if ($data->hadoop_status == 1)
			$actionList[] = "[<a href=\"?q=boa/job/{$data->id}/status\">" . t('status') . '</a>]';

		//if (!$data->deleted)
		//	$actionList[] = "[<a href=\"?q=boa/job/{$data->id}/delete\">" . t('delete') . '</a>]';

		if ($data->has_result && $data->hadoop_status == 2)
			$actionList[] = "[<a href=\"?q=boa/job/{$data->id}/download\">" . t('download output') . '</a> (' . formatBytes($data->length) . ')]';

		$rowdata = array(
			'<a title="' . htmlspecialchars(trim($data->source)) . '" href="?q=boa/job/' . $data->id . '">' . $data->id . '</a>',
			$data->submitted,
			$data->name,
			showStatus($data->compiler_status),
			showStatus($data->hadoop_status),
			implode(' ', $actionList),
		);
		if ($uid == 0)
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

	if (is_boa_admin())
		$options['caption'] = '<a href="index.php?q=boa/running">View currently running jobs</a>' . drupal_render(drupal_get_form('view_user_list', $uid, $deleted));

	return theme_table($options).theme('pager');
}

function view_user_list($form, &$form_state, $uid, $deleted) {
	$form['container'] = array(
		'#prefix' => '<div class="container-inline">',
		'#suffix' => '</div>',
	);

	$form['container']['userId'] = array(
		'#type' => 'select',
		'#default_value' => $uid,
		'#options' => array( 0 => t('All Users')),
	);

	$query = db_select('users', 'r');
	$query->join('boa_jobs', 'j', 'j.uid = r.uid');
	$query->fields('r', array('uid', 'name'));
	$query->orderBy('name');
	$query->groupBy('r.uid');
	$result = $query->execute();
	foreach ($result as $record)
		$form['container']['userId']['#options'][$record->uid] = $record->name;

	$form['container']['deleted'] = array(
		'#type' => 'checkbox',
		'#title' => t('Also show deleted jobs'),
		'#default_value' => $deleted,
	);

	$form['container']['page'] = array(
		'#type' => 'hidden',
		'#default_value' => '0',
	);

	$form['container']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('View Jobs'),
	);

	$form['#method'] = 'get';
	foreach (drupal_get_query_parameters($_GET, array('page', 'op', 'deleted', 'userId')) as $key => $val)
		$form[$key] = array(
			'#type' => 'hidden',
			'#value' => $val,
		);

	return $form;
}

?>
