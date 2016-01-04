<?php

function boa_public_view($jobId) {
	global $user, $base_path;

	drupal_set_title(t('Public Information for Job ') . $jobId);

	$query = db_select('boa_jobs', 'j');
	$query->join('boa_output', 'o', 'j.id = o.id');
	$query->fields('o', array('web_result'));
	$query->addExpression('COALESCE(o.length, 0)', 'length');
	$query->fields('j', array('id', 'uid', 'source', 'deleted',
		'compiler_status', 'hadoop_status'));
	$query->addExpression('(SELECT name FROM boa_input WHERE id = j.input)', 'name');
	$query->addExpression('unix_timestamp(created)', 'created');
	$query->addExpression('unix_timestamp(submitted)', 'submitted');
	$query->addExpression('unix_timestamp(compile_start)', 'compile_start');
	$query->addExpression('unix_timestamp(compile_end)', 'compile_end');
	$query->addExpression('unix_timestamp(hadoop_start)', 'hadoop_start');
	$query->addExpression('unix_timestamp(hadoop_end)', 'hadoop_end');
	$query->addExpression('unix_timestamp(hadoop_end) - unix_timestamp(submitted)', 'finished');
	$query->addExpression('unix_timestamp(hadoop_end) - unix_timestamp(hadoop_start)', 'hadoop_span');
	$query->addExpression('unix_timestamp(compile_end) - unix_timestamp(compile_start)', 'compile_span');
	$query->condition('j.id', $jobId);
	$result = $query->execute();

	$record = $result->fetchObject();

	# -----------------------------------------------

	$content = '';

	if ($record->deleted)
		$content .= '<h2>NOTE: This job is deleted.</h2><br/>';
	$content .= '<b>Created By:</b> ' . user_load($record->uid)->name . '<br/>';
	$content .= '<b>Created At:</b> ' . boa_date($record->created) . '<br/>';
	$content .= '<br/>';
	$content .= '<b>Input Dataset:</b> ' . $record->name . '<br/>';
	$content .= '<br/>';
	$content .= '<b>Last Submitted At:</b> ' . boa_date($record->submitted) . '<br/>';
	if ($record->hadoop_status == 2)
		$content .= '<b>Last Finished At:</b> ' . boa_date($record->hadoop_end) . ' (' . boa_duration($record->finished) . ')<br/>';

	# -----------------------------------------------

	$content .= '<h2>Source Code</h2>';
	$content .= '<div class="tabs"><ul class="tabs primary">';
	$content .= '<li><a class="active" href="">Source Code</a></li>';
	//$content .= '<li><a href="?q=boa/job/' . $record->id . '/download-src">' . t('Download Source') . '</a></li>';
	$content .= '<li><a href="?q=boa/job/' . $record->id . '/edit">' . t('Edit Source') . '</a></li>';
	$content .= '</ul></div>';
	$content .= "<div id=\"sourcecode\" style=\"position: relative; width: 100%;\">" . htmlspecialchars($record->source) . '</div>';
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
	maxLines: 500
});
//--><!]]>
</script>
JAVASCRIPT;
		$content .= <<<CSS
<style type="text/css" media="all">
#sourcecode {
	position: relative;
	width: 96%;
}
</style>
CSS;

	# -----------------------------------------------

	if ($record->compiler_status == 2 && $record->hadoop_status == 2) {
		$content .= '<h2>Output</h2>';
		$content .= '<b>Job Output Size:</b> ' . formatBytes($record->length) . '<br/>';
		if ($record->length > 0) {
			$trimmed = $record->length > strlen($record->web_result);

			$content .= '<br/>';
			if ($trimmed)
				$content .= 'Note: Output is <b>' . formatBytes($record->length) . '</b>, only showing first 64k';
			$content .= '<div class="tabs"><ul class="tabs primary">';
			$content .= '<li><a class="active" href="">Output</a></li>';
			$content .= '<li><a href="?q=boa/job/public/' . $record->id . '/download">Download Output</a></li>';
			$content .= '</ul></div>';

			$content .= '<pre class="output" style="margin-top: 0;">';
			$content .= htmlspecialchars($record->web_result);
			$content .= '</pre>';
			if ($trimmed)
				$content .= '... remaining output not shown, please <a href="?q=boa/job/public/' . $record->id . '/download">download output</a>.';
		}
	}

	# -----------------------------------------------

	$content .= '<h2>Compilation</h2>';
	$content .= '<b>Status:</b> ' . showStatus($record->compiler_status);
	$content .= '<br/>';
	if ($record->compiler_status != 0)
		$content .= '<b>Started:</b> ' . boa_date($record->compile_start) . '<br/>';
	if ($record->compiler_status == 2)
		$content .= '<b>Finished:</b> ' . boa_date($record->compile_end) . " (" . boa_duration($record->compile_span) . ')<br/>';

	# -----------------------------------------------

	if ($record->compiler_status == 2) {
		$content .= '<h2>Execution</h2>';
		$content .= '<b>Status:</b> ' . showStatus($record->hadoop_status);
		$content .= '<br/>';
		if ($record->hadoop_status != 0)
			$content .= '<b>Started:</b> ' . boa_date($record->hadoop_start) . '<br/>';
		if ($record->hadoop_status == 2)
			$content .= '<b>Finished:</b> ' . boa_date($record->hadoop_end) . ' (' . boa_duration($record->hadoop_span) . ')<br/>';
	}

	return $content;
}

?>
