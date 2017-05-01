<?php

function boa_public_download($jobId) {
	global $user;

	set_time_limit(0);

	drupal_set_title(t('Output for Job ') . $jobId);

	$query = db_select('boa_jobs', 'r');
	$query->join('boa_output', 'o', 'r.id = o.id');
	$query->fields('r', array('hadoop_status'));
	$query->addExpression('COALESCE(o.length, 0)', 'length');
	$query->condition('r.id', $jobId);

	$record = $query->execute()->fetchObject();

	if ($record->hadoop_status == 2) {
		if ($record->length > 0) {
			$tmp = tempnam("/tmp", "boa-output");
			@unlink($tmp);
			drupal_register_shutdown_function('download_tmp_cleanup', $tmp);

			set_time_limit(0);
			system("/home/hadoop/hadoop-current/bin/hadoop fs -get /boa/$jobId/output.txt $tmp");
			$file = fopen($tmp, 'r');

			drupal_add_http_header('Content-Type', 'text; utf-8');
			drupal_add_http_header('Content-Length', $record->length);
			drupal_add_http_header('Content-Disposition', "attachment; filename=\"boa-job$jobId-output.txt\"");

			while (!feof($file)) {
				print fread($file, 65536);
				while (@ob_end_flush());
				@flush();
			}

			fclose($file);
			unlink($tmp);

			drupal_exit();
		}

		drupal_set_message(t('The job finished successfully, but has no output.'), 'warning');
		drupal_goto("boa/job/public/$jobId/view");
		drupal_exit();
	}

	drupal_set_message(t('The job has not successfully finished running.'), 'warning');
	drupal_goto("boa/job/public/$jobId/view");
	drupal_exit();
}

function download_tmp_cleanup($tmp) {
	@unlink($tmp);
}

?>
