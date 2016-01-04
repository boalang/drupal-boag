<?php

function _delete_job($jobId) {
	$query = db_update('boa_jobs');
	$query->fields(array('deleted' => 1, 'hadoop_status' => 3));
	$query->condition('id', $jobId);
	$query->execute();
}

function boa_job_delete($form, &$form_state, $jobId) {
	if (!isValidJob($jobId))
		invalidJob($jobId);

	$form['jobid'] = array(
		'#type' => 'value',
		'#value' => $jobId,
	);

	return confirm_form($form, t('Are you sure you want to delete this job?'), 'boa/job/' . $jobId, t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

function boa_delete_job_submit($form, &$form_state) {
	$jobId = $form_state['values']['jobid'];

	if (!isValidJob($jobId))
		invalidJob($jobId);

	_delete_job($jobId);

	drupal_set_message(t('Job :jobid was deleted', array(':jobid' => $jobId)), 'warning');
	drupal_goto('boa/jobs');
	drupal_exit();
}

function _job_delete_rpc($jobId) {
	if (!isValidJob($jobId))
		invalidJob($jobId);

	_delete_job($jobId);

	return "";
}

?>
