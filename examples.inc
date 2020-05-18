<?php

global $examples, $example_options;

if (!function_exists('t')) {
	function t($str) { return $str; }
}

# ------------------------------------------------------------------------------------ #

$example_names['q1'] = t('Example Title');
$example_jobs['q1'] = 226;
$examples['q1'] = <<<BOA
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['q2'] = t('Example Title');
$example_jobs['q2'] = 227;
$examples['q2'] = <<<BOA
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['q3'] = t('Example Title');
$example_jobs['q3'] = 228;
$examples['q3'] = <<<BOA
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['q4'] = t('Example Title');
$example_jobs['q4'] = 229;
$examples['q4'] = <<<BOA
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['q5'] = t('Example Title');
$example_jobs['q5'] = 230;
$examples['q5'] = <<<BOA
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['q6'] = t('Example Title');
$example_jobs['q6'] = 231;
$examples['q6'] = <<<BOA
BOA;

# ------------------------------------------------------------------------------------ #


# ------------------------------------------------------------------------------------ #

$example_options = array(
	 0 => t('-- Select Example --'),
	 'q1' => $example_names['q1'],
	 'q2' => $example_names['q2'],
	 'q3' => $example_names['q3'],
	 'q4' => $example_names['q4'],
	 'q5' => $example_names['q5'],
	 'q6' => $example_names['q6'],
	 'q7' => $example_names['q7'],
);

?>
