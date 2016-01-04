<?php

function boa_job_stop($form, &$form_state, $jobId) {
	if (!isValidJob($jobId))
		invalidJob($jobId);

	$form['jobid'] = array(
		'#type' => 'value',
		'#value' => $jobId,
	);

	return confirm_form($form, t('Are you sure you want to stop this job?'), 'boa/jobs', t('Any temporary results will be deleted.'), t('Stop'), t('Cancel'));
}

function _boa_stop_job($jobId) {
	$query = db_update('boa_jobs');
	$query->fields(array('hadoop_status' => 3));
	$query->condition('id', $jobId);
	$query->execute();
}

function boa_job_stop_submit($form, &$form_state) {
	$jobId = $form_state['values']['jobid'];

	if (!isValidJob($jobId))
		invalidJob($jobId);

	_boa_stop_job($jobId);

	drupal_set_message(t('Job :jobid was stopped', array(':jobid' => $jobId)), 'warning');
	drupal_goto('boa/job/' . $jobId);
	drupal_exit();
}

function _boa_stop_rpc($jobId) {
	if (!isValidJob($jobId))
		services_erorr(t('Invalid job :jobid or this job does not belong to you.', array(':jobid' => $jobId)), 403);
	else
		_boa_stop_job($jobId);

	drupal_exit();
}

?>
