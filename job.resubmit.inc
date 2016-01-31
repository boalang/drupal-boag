<?php

function _job_resubmit($jobId) {
	$query = db_update('boa_jobs');
	$tz = date_default_timezone_get();
	date_default_timezone_set('America/Chicago');
	$query->fields(array(
		'submitted' => date('Y-m-d G:i:s'),
		'compiler_status' => 0,
		'hadoop_status' => 4,
		'compiler_output' => null,
		'hadoop_output' => null
	));
	date_default_timezone_set($tz);
	$query->condition('id', $jobId);
	$query->execute();

	$query = db_update('boa_output');
	$query->fields(array(
		'length' => 0,
		'web_result' => ''
	));
	$query->condition('id', $jobId);
	$query->execute();

	@system("/home/hadoop/hadoop-current/bin/hadoop fs -rmr /boa/$jobId/");
}

function boa_job_resubmit($jobId) {
	global $user;

	if (!isValidJob($jobId))
		invalidJob($jobId);

	$running = job_run_count($user->uid);
	_job_resubmit($jobId);

	drupal_set_message('Your job was re-submitted.  Please note it can take several minutes to get results, depending on server load.');
	if ($running > 0)
		drupal_set_message(t('You have other job(s) running.  This job will queue until those jobs finish or are stopped.'));
	if (job_run_count() > 3)
		drupal_set_message(t('Server load is currently high.  It will take some time for this job to finish.'));
	drupal_goto("boa/job/$jobId");
	drupal_exit();
}

function _job_resubmit_rpc($jobId) {
	if (!isValidJob($jobId))
		invalidJob($jobId);

	_job_resubmit($jobId);

	return "";
}

?>
