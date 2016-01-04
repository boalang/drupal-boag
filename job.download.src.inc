<?php

function _job_source_rpc($jobId) {
	$query = db_select('boa_jobs', 'j');
	$query->fields('j', array('source'));
	$query->condition('deleted', false);
	$query->condition('j.id', $jobId);
	$result = $query->execute();
	$record = $result->fetchObject();
	return $record->source;
}

function boa_job_download_src($jobId) {
	global $user;

	if (!isValidJob($jobId))
		invalidJob($jobId);

	set_time_limit(0);

	drupal_set_title(t('Download Source ') . $jobId);

	$query = db_select('boa_jobs', 'r');
	$query->fields('r', array('source'));
	$query->addExpression('CHAR_LENGTH(source)', 'result_length');
	$query->condition('id', $jobId);
	$result = $query->execute();

	$record = $result->fetchObject();

	drupal_add_http_header('Content-Type', 'text; utf-8');
	drupal_add_http_header('Content-Length', $record->result_length);
	drupal_add_http_header('Content-Disposition', "attachment; filename=\"boa-job$jobId-source.boa\"");

	print $record->source;

	drupal_exit();
}

?>
