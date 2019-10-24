<?php

function boa_job($jobId) {
	global $user, $base_path;

	if (!isValidJob($jobId))
		invalidJob($jobId);

	drupal_set_title(t('Information for Job ') . $jobId);

	$query = db_select('boa_jobs', 'j');
	$query->join('boa_output', 'o', 'j.id = o.id');
	$query->fields('j', array('id', 'uid', 'source', 'deleted', 'public',
		'compiler_output', 'compiler_status', 'hadoop_status', 'parent_id'));
	$query->addExpression('(SELECT name FROM boa_input WHERE id = j.input)', 'name');
	$query->addExpression('COALESCE(o.length, 0)', 'length');
	$query->addExpression('unix_timestamp(created)', 'created');
	$query->addExpression('unix_timestamp(submitted)', 'submitted');
	$query->addExpression('unix_timestamp(compile_end)', 'compile_end');
	$query->addExpression('unix_timestamp(hadoop_end)', 'hadoop_end');
	$query->addExpression('unix_timestamp(hadoop_end) - unix_timestamp(submitted)', 'finished');
	if (!is_boa_admin())
		$query->condition('deleted', false);
	$query->condition('j.id', $jobId);
	$result = $query->execute();

	$record = $result->fetchObject();

	# -----------------------------------------------

	$content = '';

	if ($record->parent_id > 0)
		$content .= '&larr; based on Job <a href="?q=boa/job/' . $record->parent_id . '">' . $record->parent_id . '</a> (<a href="?q=boa/job/' . $jobId . '/diff/' . $record->parent_id . '">diff</a>)<br/>';

	$query2 = db_select('boa_jobs', 'j');
	$query2->fields('j', array('id', 'uid', 'deleted'));
	$query2->condition('parent_id', $jobId);
	$result2 = $query2->execute();

	while ($record2 = $result2->fetchObject())
		if (is_boa_admin() || ($record2->uid == $user->uid && !$record2->deleted))
			$content .= '&rarr; modified in Job <a href="?q=boa/job/' . $record2->id . '">' . $record2->id . '</a> (<a href="?q=boa/job/' . $record2->id . '/diff/' . $jobId . '">diff</a>)<br/>';

	if ($record->parent_id > 0)
		$content .= '<br/>';

	fakeRunning($record);

	if ($record->compiler_status == 0 || $record->compiler_status == 1 ||
			($record->compiler_status == 2 && ($record->hadoop_status == 0 || $record->hadoop_status == 1)))
		$content .= '<b>Job is currently running</b> <div class="ajax-progress ajax-progress-throbber"><div class="throbber">&nbsp;</div></div><br/>(note: this page automatically reloads)<br/><br/>';

	if ($record->deleted)
		$content .= '<h2>NOTE: This job is deleted.</h2><br/>';
	if ($record->public)
		$content .= '<b>Job is PUBLIC</b>: <a href="?q=boa/job/' . $record->id . '/private">Make Private</a> <a href="?q=boa/job/public/' . $record->id . '">View Public Page</a><br/>';
	else
		$content .= '<b>Job is PRIVATE</b>: <a href="?q=boa/job/' . $record->id . '/public">Make Public</a><br/>';
	if ($user->uid != $record->uid)
		$content .= '<b>Created By:</b> <a href="?q=user/' . $record->uid . '">' . user_load($record->uid)->name . '</a><br/>';
	$content .= '<b>Created At:</b> ' . boa_date($record->created) . '<br/>';
	$content .= '<br/>';
	$content .= '<b>Input Dataset:</b> ' . $record->name . '<br/>';
	$content .= '<br/>';
	if ($record->compiler_status == 0 || $record->compiler_status == 1 ||
			($record->compiler_status == 2 && ($record->hadoop_status == 0 || $record->hadoop_status == 1)))
		$content .= '<b>Last Submitted At:</b> ' . boa_date($record->submitted) . ' (' . boa_duration(time() - $record->submitted) .')<br/>';
	else
		$content .= '<b>Last Submitted At:</b> ' . boa_date($record->submitted) . '<br/>';
	if ($record->hadoop_status == 2) {
		$content .= '<b>Last Finished At:</b> ' . boa_date($record->hadoop_end) . " (" . boa_duration($record->finished) . ")<br/>";
		$content .= '<b>Job Output Size:</b> ' . formatBytes($record->length) . "<br/>";
	}

	# -----------------------------------------------

	$content .= '<h2>Source Code <a href="#">^</a></h2>';
	$content .= '<div class="tabs"><ul class="tabs primary">';
	$content .= '<li><a class="active" href="">Source Code</a></li>';
	$content .= '<li><a href="?q=boa/job/' . $record->id . '/download-src">' . t('Download Source') . '</a></li>';
	$content .= '<li><a href="?q=boa/job/' . $record->id . '/edit">' . t('Edit Source') . '</a></li>';
	$content .= '</ul></div>';
	$content .= "<div id=\"sourcecode\" style=\"position: relative; width: 100%\">" . htmlspecialchars($record->source) . '</div>';
	drupal_add_js($base_path . 'sites/all/libraries/ace/src/ace.js');
	drupal_add_js($base_path . 'sites/all/libraries/ace/src/theme-crimson_editor.js');
	drupal_add_js($base_path . 'sites/all/libraries/ace/src/mode-boa.js');
	$content .= <<<JAVASCRIPT
<script type="text/javascript">
<!--//--><![CDATA[//><!--
var BoaMode = require("ace/mode/boa").Mode;
var editor = ace.edit("sourcecode");
editor.getSession().setMode(new BoaMode());
editor.setTheme("ace/theme/crimson_editor");
editor.renderer.setShowGutter(true);
editor.setShowPrintMargin(false);
editor.setHighlightActiveLine(false);
editor.setShowInvisibles(false);
editor.setReadOnly(true);
editor.setOptions({
	maxLines: 1000
});
//--><!]]>
</script>
JAVASCRIPT;
		$content .= <<<CSS
<style type="text/css" media="all">
.errorMarker {
	position: absolute;
	background: rgba(255,0,0,0.3);
	z-index: 20
}
.warningMarker {
	position: absolute;
	background: rgba(255,174,66,0.2);
	z-index: 20
}
#sourcecode {
	position: relative;
	width: 96%;
}
</style>
CSS;

	# -----------------------------------------------

	$form = drupal_get_form('update_form', $jobId);
	$content .= drupal_render($form);

	if ($record->compiler_status == 0 || $record->compiler_status == 1) {
		$wait = 3000;
		if ($record->compiler_status == 1 && $record->compile_end > time())
			$wait = ($record->compile_end - time()) * 1000 + 1000;
		$content .= <<<JAVASCRIPT
<script type="text/javascript">
<!--//--><![CDATA[//><!--
var jobTimer = setInterval(function (){
	location.reload();
//	var v = Drupal.settings.ajax['edit-run'];
//	jQuery(v.element).trigger(v.event);
}, $wait);
//--><!]]>
</script>
JAVASCRIPT;
	} else if ($record->compiler_status == 2 && ($record->hadoop_status == 0 || $record->hadoop_status == 1)) {
		$wait = strpos($record->compiler_output, 'task complexity: simple') > 0 ? 20000 : 40000;
		if ($record->hadoop_status == 1 && $record->hadoop_end > time())
			$wait = ($record->hadoop_end - time()) * 1000 + 1000;
		$content .= <<<JAVASCRIPT
<script type="text/javascript">
<!--//--><![CDATA[//><!--
var jobTimer = setInterval(function (){
	location.reload();
//	var v = Drupal.settings.ajax['edit-run'];
//	jQuery(v.element).trigger(v.event);
}, $wait);
//--><!]]>
</script>
JAVASCRIPT;
	}

	return $content;
}

function update_form($form, &$form_state, $jobId) {
	$form['run'] = array(
		'#type' => 'submit',
		'#value' => "Refresh Job $jobId",
		'#ajax' => array(
			'callback' => 'update_form_callback',
			'wrapper' => 'update_box',
			'progress' => array(
				'type' => 'progress'
			),
			'effect' => 'fade',
		),
		'#prefix' => '<div style="display: none;">',
		'#suffix' => '</div>',
	);

	$form['content'] = array(
		'#markup' => generate_content($jobId),
	);

	$form['#prefix'] = '<div id="update_box">';
	$form['#suffix'] = '</div>';

	return $form;
}

function update_form_callback($form, $form_state) {
	$form['content']['#markup'] = generate_content($form);
	return $form;
}

function generate_content($jobId) {
	global $user;

	if (is_array($jobId))
		$jobId = substr(strrchr($jobId['#action'], '/'), 1);

	if (!isValidJob($jobId))
		return t('Problem loading job status.');

	$query = db_select('boa_jobs', 'r');
	$query->fields('r', array('id', 'uid', 'source', 'submitted',
		'compiler_status', 'compiler_output',
		'hadoop_status', 'hadoop_output'));
	$query->addExpression('unix_timestamp(compile_start)', 'compile_start');
	$query->addExpression('unix_timestamp(compile_end)', 'compile_end');
	$query->addExpression('unix_timestamp(hadoop_start)', 'hadoop_start');
	$query->addExpression('unix_timestamp(hadoop_end)', 'hadoop_end');
	$query->addExpression('unix_timestamp(hadoop_end) - unix_timestamp(hadoop_start)', 'hadoop_span');
	$query->addExpression('unix_timestamp(compile_end) - unix_timestamp(compile_start)', 'compile_span');
	$query->condition('id', $jobId);
	$result = $query->execute();

	$record = $result->fetchObject();

	# -----------------------------------------------

	$faked = $record->hadoop_status;
	fakeRunning($record);
	$faked = $faked != $record->hadoop_status;

	$content = '<h2>Compilation <a href="#">^</a></h2>';
	$content .= '<b>Status:</b> ' . showStatus($record->compiler_status);
	if ($record->compiler_status == 0 || $record->compiler_status == 1)
		$content .= '<div class="ajax-progress ajax-progress-throbber"><div class="throbber">&nbsp;</div></div>';
	$content .= '<br/>';
	if ($record->compiler_status != 0)
		$content .= '<b>Started:</b> ' . boa_date($record->compile_start) . '<br/>';
	if ($record->compiler_status == 2 || (is_boa_admin() && $record->compiler_status < 0))
		$content .= '<b>Finished:</b> ' . boa_date($record->compile_end) . " (" . boa_duration($record->compile_span) . ")<br/>";
	if ($record->compiler_status < 0) {
		$matches = array();
		if (preg_match_all('/Lexical error at line (\d+), columns? (\d+(?:-\d+)?).\s*(.*?)\s*\tat/s', $record->compiler_output, $lexicalmatches, PREG_SET_ORDER))
			$matches = array_merge($matches, $lexicalmatches);
		if (preg_match_all('/Encountered .*? at line (\d+), columns? (\d+(?:-\d+)?).\s*(.*?)\s*\tat/s', $record->compiler_output, $parsematches, PREG_SET_ORDER))
			$matches = array_merge($matches, $parsematches);
		if (preg_match_all('/Error at lines (\d+)-\d+, columns (\d+-\d+):\s*(.*?)\s*\tat/s', $record->compiler_output, $typematches, PREG_SET_ORDER))
			$matches = array_merge($matches, $typematches);
		$content .= markErrors($matches);
	}
	if ($record->compiler_status == 2 || $record->compiler_status < 0) {
		$matches = array();
		if (preg_match_all('/WARNING at line (\d+), columns? (\d+(?:-\d+)?):\s*(.*)\s*/', $record->compiler_output, $warningmatches, PREG_SET_ORDER))
			$matches = array_merge($matches, $warningmatches);
		$content .= markErrors($matches, "warning");
	}
	if (is_boa_admin() && $record->compiler_status != 0 && $record->compiler_status != 1)
		$content .= '<b>Raw Log Output</b><pre class="output">' . htmlspecialchars(trim($record->compiler_output)) . '</pre>';

	# -----------------------------------------------

	if ($record->compiler_status == 2) {
		$content .= '<h2>Execution <a href="#">^</a></h2>';
		$content .= '<b>Status:</b> ' . showStatus($record->hadoop_status);
		if ($record->hadoop_status == 0 || $record->hadoop_status == 1) {
			$content .= '<div class="ajax-progress ajax-progress-throbber"><div class="throbber">&nbsp;</div></div>';
			if (!$faked)
				$content .= '[<a href="?q=boa/job/' . $jobId . '/status">' . t('status') . '</a>]';
		}
		$content .= '<br/>';
		if ($record->hadoop_status == 1)
			$content .= '<b>Started:</b> ' . boa_date($record->hadoop_start) . ' (' . boa_duration(time() - $record->hadoop_start) .')<br/>';
		else if ($record->hadoop_status != 0)
			$content .= '<b>Started:</b> ' . boa_date($record->hadoop_start) . '<br/>';
		if ($record->hadoop_status == 2 || $record->hadoop_status < 0)
			$content .= '<b>Finished:</b> ' . boa_date($record->hadoop_end) . ' (' . boa_duration($record->hadoop_span) . ')<br/>';
		if ($record->hadoop_status < 0 && strlen($record->hadoop_output)) {
			$matches = array();
			preg_match_all('/Caused by: (.*?)\s*\tat/s', $record->hadoop_output, $matches, PREG_SET_ORDER);
			if (count($matches)) {
				$content .= '<pre class="output">';
				$errs = array();
				for ($i = 0; $i < count($matches); $i++)
					$errs[] = trim($matches[$i][1]);
				$content .= implode("\n\n", array_unique($errs)) . '</pre>';
			}
		}
		if (strlen($record->hadoop_output) && is_boa_admin())
			$content .= '<b>Raw Log Output</b><pre class="output">' . htmlspecialchars(preg_replace("/^.*Max block location exceeded for split.*\n/m", '', trim($record->hadoop_output))) . '</pre>';

	}

	return $content;
}

//	errType: "error"
//	errType: "warning"
//	errType: "info"
function markErrors($matches, $errType = "error") {
	if (count($matches) == 0)
		return "";

	$errors = "";
	$content = "";

	for ($i = 0; $i < count($matches); $i++) {
		$row = $matches[$i][1] - 1;
		$col = $matches[$i][2];

		$err = str_replace("\n", '\n', addslashes($matches[$i][3]));
		$err = str_replace("\\t", '    ', $err);
		$err = "Col: $col - " . str_replace(" ", ' ', $err);

		if ($err[strlen($err) - 1] == '^')
			$err = substr($err, 0, strpos($err, '\n'));

		if ($i > 0)
			$errors .= ',';

		$errors .= <<<JAVASCRIPT
{
	row: $row,
	column: $col,
	text: "$err",
	type: "$errType"
}
JAVASCRIPT;

		$output = $matches[$i][0];
		if (substr($output, -2) == "at")
			$output = trim(substr($output, 0, -2));
		if (substr($output, -1) == "\n")
			$output = trim(substr($output, 0, -1));
		$content .= "<pre class=\"output\">" . htmlspecialchars($output) . "</pre>";
	}

	$content .= <<<JAVASCRIPT
<script type="text/javascript">
<!--//--><![CDATA[//><!--
editor.getSession().setAnnotations([$errors]);
var Range = ace.require('ace/range').Range;
//editor.getSession().addMarker(new Range(5, 0, 5, 5), "errorMarker", "line", false);
//--><!]]>
</script>
JAVASCRIPT;

	return $content;
}

// FIXME need to modify the previous method so we dont duplicate here
function markErrors2($matches) {
	$errors = array();

	for ($i = 0; $i < count($matches); $i++) {
		$output = $matches[$i][0];
		$output = trim(substr($output, 0, -2));
		if (substr($output, -1) == "\n")
			$output = trim(substr($output, -1));
		$errors[] = $output;
	}

	return $errors;
}

function _job_compilererrors_rpc($jobId) {
	$query = db_select('boa_jobs', 'j');
	$query->fields('j', array('compiler_output'));
	$query->condition('deleted', false);
	$query->condition('j.id', $jobId);
	$result = $query->execute();

	$record = $result->fetchObject();

	$matches = array();
	if (preg_match_all('/Lexical error at line (\d+), column (\d+).\s*(.*?)\s*\tat/s', $record->compiler_output, $lexicalmatches, PREG_SET_ORDER))
		$matches = array_merge($matches, $lexicalmatches);
	if (preg_match_all('/Encountered .*? at line (\d+), column (\d+).\s*(.*?)\s*\tat/s', $record->compiler_output, $parsematches, PREG_SET_ORDER))
		$matches = array_merge($matches, $parsematches);
	if (preg_match_all('/Error at lines (\d+)-\d+, columns (\d+)-\d+:\s*(.*?)\s*\tat/s', $record->compiler_output, $typematches, PREG_SET_ORDER))
		$matches = array_merge($matches, $typematches);

	return markErrors2($matches);
}

?>
