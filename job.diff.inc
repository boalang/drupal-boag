<?php

require_once 'class.Diff.php';

function boa_job_diff($jobId, $oldJobId) {
	if (!isValidJob($jobId))
		if (!boa_public_access($jobId))
			invalidJob($jobId);

	if (!isValidJob($oldJobId))
		if (!boa_public_access($oldJobId))
			invalidJob($oldJobId);

	$jobSrc = getSrc($jobId);
	$oldJobSrc = getSrc($oldJobId);

	echo <<<HTML
<html>
<head>
<style type="text/css">
.titlebar {
	position: fixed;
	top: 0px;
	width: 50%;
	height: 1.5em;
	background-color: #fff;
	margin-top: 0px;
	border-bottom: 1px solid #000;
	line-height: 1.5em;
}

.diff {
	margin-top: 3em;
    border-collapse: collapse;
}

.diff td {
	padding: 0 1em;
	vertical-align: top;
	white-space: pre;
	white-space: pre-wrap;
	font-family: Consolas,'Courier New',Courier,monospace;
	font-size: 0.75em;
	border-left: 1px solid #fff;
}
.diff td:last-child {
	border-left: 1px solid #000;
}

.diff span {
	padding: 0 3px;
}

.diff span:first-child {
	margin-top: 0;
}

.diffDeleted {
	border: 1px solid rgb(255,192,192);
	background: rgb(255,224,224);
}

.diffInserted {
	border: 1px solid rgb(192,255,192);
	background: rgb(224,255,224);
}
</style>
</head>

<body>
<h1 class="titlebar">Diff from job <a href="?q=boa/job/$oldJobId">$oldJobId</a></h1><h1 class="titlebar" style="left: 50%;">to job <a href="?q=boa/job/$jobId">$jobId</a></h1>
HTML;

	echo Diff::toTable(Diff::compare($oldJobSrc, $jobSrc));

	echo <<<HTML
</body>
</html>
HTML;

	drupal_exit();
}

function getSrc($jobId) {
	$query = db_select('boa_jobs', 'r');
	$query->fields('r', array('source'));
	$query->condition('id', $jobId);

	return $query->execute()->fetchObject()->source;
}

?>
