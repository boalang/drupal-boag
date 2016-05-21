<?php

function boa_job_status($jobId) {
	global $user;

	if (!isValidJob($jobId))
		invalidJob($jobId);

	drupal_set_title(t('Hadoop Status for Job ') . $jobId);

	$query = db_select('boa_jobs', 'r');
	$query->join('boa_output', 'o', 'r.id = o.id');
	$query->fields('r', array('hadoop_status'));
	$query->fields('r', array('hadoop_output'));
	$query->condition('r.id', $jobId);

	$record = $query->execute()->fetchObject();

	if ($record->hadoop_status == 1) {
		if (!preg_match('/Job ID: (job_\d+_\d+)/s', $record->hadoop_output, $matches)) {
			drupal_set_message(t('Unable to view execution status.'), 'error');
			drupal_goto("boa/job/$jobId");
			drupal_exit();
		}

		set_time_limit(0);
		exec("/home/hadoop/hadoop-current/bin/hadoop job -status ${matches[1]}", $out);

		return '<pre>' . implode("\n", $out) . '</pre>';
	}

	drupal_set_message(t('The job is not currently running.'), 'warning');
	drupal_goto("boa/job/$jobId");
	drupal_exit();
}

?>
