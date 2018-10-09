<?php

module_load_include('inc', 'boa', 'job.download');

function boa_public_download($jobId) {
	_download_job($jobId, "boa/job/public/$jobId/view");
}

?>
