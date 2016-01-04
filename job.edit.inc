<?php

function boa_job_edit($jobId) {
	drupal_goto('boa/run', array('query' => array('edit' => $jobId)));
	drupal_exit();
}

?>
