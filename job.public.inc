<?php

function _job_setpublic_rpc($jobId, $public) {
	global $user;

	$query = db_update('boa_jobs');
	$query->fields(array('public' => $public ? 1 : 0));
	$query->condition('id', $jobId);
	$query->condition('uid', $user->uid);
	$query->execute();

	return "";
}

function _job_public_rpc($jobId) {
	global $user;

	$query = db_select('boa_jobs', 'j');
	$query->fields('j', array('public'));
	$query->condition('j.uid', $user->uid);
	$query->condition('j.id', $jobId);

	$result = $query->execute();
	$data = $result->fetchObject();
	return $data->public > 0 ? 1 : 0;
}

function boa_job_public($jobId) {
	if (!isValidJob($jobId))
		invalidJob($jobId);

	$query = db_update('boa_jobs');
	$query->fields(array('public' => 1));
	$query->condition('id', $jobId);
	$query->execute();

	drupal_set_message(t('The job is now PUBLIC and visible to anyone.'), 'warning');
	drupal_goto("boa/job/$jobId");
	drupal_exit();
}

function boa_job_private($jobId) {
	if (!isValidJob($jobId))
		invalidJob($jobId);

	$query = db_update('boa_jobs');
	$query->fields(array('public' => 0));
	$query->condition('id', $jobId);
	$query->execute();

	drupal_set_message(t('The job is now PRIVATE and visible to only you.'), 'warning');
	drupal_goto("boa/job/$jobId");
	drupal_exit();
}

?>
