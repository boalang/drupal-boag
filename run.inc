<?php

module_load_include('inc', 'boa', 'examples');

function _get_datasets() {
	$query = db_select('boa_input', 'r');
	$query->fields('r', array('id'));
	$query->addExpression('IF(enabled = 1, name, CONCAT(name, \' [admin only]\'))', 'name');
	$query->orderBy('id', 'DESC');
	if (!is_boa_admin()) {
		$query->condition('id', 1, '>');
		$query->condition('enabled', 1);
	}
	$query->isNotNull('path');
	$query->isNotNull('compiler_path');
	return $query->execute();
}

function boa_run_form($form, &$form_state) {
	global $examples, $example_options;
	global $base_path;

	drupal_add_js($base_path . 'sites/all/libraries/ace/src/ace.js');
	drupal_add_js($base_path . 'sites/all/libraries/ace/src/ext-language_tools.js');
	drupal_add_js($base_path . 'sites/all/libraries/ace/src/theme-crimson_editor.js');
	drupal_add_js($base_path . 'sites/all/libraries/ace/src/mode-boa.js');

	$form['examples'] = array(
		'#type' => 'select',
		'#title' => t('Run an Example'),
		'#ajax' => array(
			'callback' => 'boa_run_form_callback',
			'wrapper' => 'sourcecode',
			'progress' => array(
				'type' => 'throbber'
			),
			'effect' => 'fade',
		),
		'#options' => $example_options,
		'#default_value' => 0,
	);

	$form['source'] = array(
		'#type' => 'textarea',
		'#prefix' => '<div id="sourcecode">',
		'#cols' => 50,
		'#title' => t('Boa Source Code'),
	);

	$form['parent'] = array(
		'#type' => 'hidden',
	);

	if (isset($_GET['example']))
		$form['examples']['#default_value'] = $_GET['example'];
	if (isset($_GET['edit'])) {
		if (!boa_public_access($_GET['edit']) && !isValidJob($_GET['edit'])) {
			invalidJob($_GET['edit']);
		} else {
			$query = db_select('boa_jobs', 'j');
			$query->fields('j', array('source', 'input'));
			$query->condition('id', $_GET['edit']);
			if (!isValidJob($_GET['edit']))
				$query->condition('public', 1);
			$result = $query->execute();
			$record = $result->fetchObject();
			$form['source']['#default_value'] = $record->source;
			$form['input']['#default_value'] = $record->input;
			$form['parent']['#default_value'] = $_GET['edit'];
		}
	}
	if (isset($_GET['source']))
		$form['source']['#default_value'] = $_GET['source'];

	$source = !empty($form_state['values']['examples']) ? $examples[$form_state['values']['examples']] : (!empty($form['source']['#default_value']) ? $form['source']['#default_value'] : "");
	$form['source']['#suffix'] = '<div style="position: relative; width: 100%;" id="source"></div></div>';

	$form['#attributes']['onsubmit'] = "jQuery('textarea[name=\"source\"]').val(jQuery('source').getSession().getValue());";

	$form['ace'] = array(
		'#markup' => <<<JAVASCRIPT
<script type="text/javascript">
<!--//--><![CDATA[//><!--
var BoaMode = require("ace/mode/boa").Mode;
ace.require("ace/ext/language_tools");
function setupAce() {
	var editor = ace.edit("source");
	editor.getSession().setMode(new BoaMode());
	editor.setTheme("ace/theme/crimson_editor");
	editor.renderer.setShowGutter(true);
	editor.setShowPrintMargin(false);
	editor.setShowInvisibles(false);
	var textarea = jQuery('textarea[name="source"]');
	editor.getSession().setValue(textarea.val());
	textarea.parent().hide();
	textarea.change(function(){
		editor.getSession().setValue(textarea.val());
	});
	editor.getSession().on('change', function(){
		textarea.val(editor.getSession().getValue());
	});
	editor.setOptions({
		enableBasicAutocompletion: true,
		enableSnippets: true,
		enableLiveAutocompletion: true,
		minLines: 10,
		maxLines: 1000
	});
}
setupAce();
jQuery(document).ajaxComplete(function(){
	setupAce();
});
//--><!]]>
</script>
<style type="text/css" media="all">
.ace_sb {
	overflow-y: auto !important;
}
#source {
	position: relative;
	width: 100%;
}
</style>
JAVASCRIPT
	);

	$form['input'] = array(
		'#type' => 'select',
		'#title' => t('Input Dataset (use the SMALL dataset when testing queries!)'),
		'#default_value' => isset($form['input']['#default_value']) ? $form['input']['#default_value'] : 0
	);

	$result = _get_datasets();
	foreach ($result as $record)
		$form['input']['#options'][$record->id] = $record->name;

	$form['run'] = array(
		'#type' => 'submit',
		'#value' => t('Run Program'),
	);

	$form['note'] = array(
		'#markup' => t('<span style="font-size: 0.8em;">NOTE: All data submitted to this site is subject to our <a href="/policies/privacy.php" target="_blank">privacy policy</a>.</span>'),
	);

	return $form;
}

function boa_run_form_callback($form, $form_state) {
	global $examples;
	$form['source']['#value'] = '';
	if (!empty($form_state['values']['examples']))
		$form['source']['#value'] = htmlspecialchars_decode($examples[$form_state['values']['examples']]);
	return $form['source'];
}

function boa_run_form_submit($form, &$form_state) {
	global $user;

	if (!is_valid_input($form_state['values']['input']))
		invalid_input();

	$source = trim($form_state['values']['source']);
	if (empty($source)) {
		drupal_set_message(t('Boa program is empty!'), 'error');
		drupal_goto('boa/run', array('query' => array('source' => $form_state['values']['source'])));
		drupal_exit();
	}

	$running = job_run_count($user->uid);

	$query = db_insert('boa_jobs');
	$tz = date_default_timezone_get();
	date_default_timezone_set('America/Chicago');
	$query->fields(array(
		'submitted' => date('Y-m-d G:i:s'),
		'uid' => $user->uid,
		'parent_id' => isset($form_state['values']['parent']) ? (int)$form_state['values']['parent'] : NULL,
		'source' => $source,
		'input' => $form_state['values']['input'],
	));
	date_default_timezone_set($tz);
	$id = $query->execute();

	$query = db_insert('boa_output');
	$query->fields(array(
		'id' => $id,
		'length' => 0
	));
	$query->execute();

	drupal_set_message(t('Your job was submitted.  Please note it can take several minutes to get results, depending on server load.'));
	if ($running > 0)
		drupal_set_message(t('You have other job(s) running.  This job will queue until those jobs finish or are stopped.'));
	if (job_run_count() > 3)
		drupal_set_message(t('Server load is currently high.  It will take some time for this job to finish.'));
	drupal_goto("boa/job/$id");
	drupal_exit();
}

function _run_rpc($source, $input) {
	global $user;

	if (!is_valid_input($input))
		invalid_input();

	$query = db_insert('boa_jobs');
	$tz = date_default_timezone_get();
	date_default_timezone_set('America/Chicago');
	$query->fields(array(
		'submitted' => date('Y-m-d G:i:s'),
		'uid' => $user->uid,
		'source' => trim($source),
		'input' => $input,
	));
	date_default_timezone_set($tz);
	$id = $query->execute();

	$query = db_insert('boa_output');
	$query->fields(array(
		'id' => $id,
		'length' => 0
	));
	$query->execute();

	$query = db_select('boa_jobs', 'j');
	$query->join('boa_input', 'i', 'j.input = i.id');
	$query->fields('j', array('submitted', 'compiler_status', 'hadoop_status'));
	$query->fields('i', array('name'));
	$query->addField('i', 'id', 'inputid');
	$query->condition('j.id', $id);
	$result = $query->execute();

	$rows = array();
	while ($data = $result->fetchObject()) {
		fakeRunning($data);

		$rows[] = array(
			'id' => $id,
			'submitted' => $data->submitted . ' ' . strftime('%Z'),
			'input' => array('id' => $data->inputid, 'name' => $data->name),
			'compiler_status' => showStatus($data->compiler_status),
			'hadoop_status' => showStatus($data->hadoop_status),
		);
	}

	return $rows[0];
}

function _datasets_rpc() {
	$result = _get_datasets();
	$rows = array();
	foreach ($result as $record)
		$rows[] = array('id' => $record->id, 'name' => $record->name);

	return $rows;
}
