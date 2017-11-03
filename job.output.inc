<?php

function boa_job_output($jobId) {
	global $user;

	if (!isValidJob($jobId))
		invalidJob($jobId);

	drupal_set_title(t('Output for Job ') . $jobId);

	$query = db_select('boa_jobs', 'r');
	$query->join('boa_output', 'o', 'r.id = o.id');
	$query->fields('r', array('hadoop_status'));
	$query->fields('o', array('web_result'));
	$query->addExpression('COALESCE(o.length, 0)', 'length');
	$query->condition('r.id', $jobId);

	$record = $query->execute()->fetchObject();

	if ($record->hadoop_status == 2) {
		if ($record->length > 0) {
			$trimmed = $record->length > strlen($record->web_result);
			$content = "";
			if ($trimmed)
				$content .= "\nNote: Output is <b>" . formatBytes($record->length) . "</b>, only showing first 64k\n";
			$content = _print_table($record->web_result);
			if ($trimmed)
				$content .= "... remaining output not shown, please <a href=\"?q=boa/job/$jobId/download\">download output</a>.\n";

			return $content;
		}

		drupal_set_message(t('The job finished successfully, but has no output.'), 'warning');
		drupal_goto("boa/job/$jobId");
		drupal_exit();
	}

	drupal_set_message(t('The job has not successfully finished running.'), 'warning');
	drupal_goto("boa/job/$jobId");
	drupal_exit();
}

function _print_table($table) {
	$rows = explode("\n", substr($table, 0, strrpos($table, "\n")));
	$content = '';
	$last = '';
	$prevTable = false;
	foreach ($rows as $row) {
		$eqPos = strpos($row, '=');
		$bracketPos = strpos($row, '[');
		if ($bracketPos != -1 && $bracketPos < $eqPos)
			$cur = substr($row, 0, strpos($row, '['));
		else
			$cur = substr($row, 0, strpos($row, '='));

		if (strpos($row, '[] = ') !== false) {
			if ($last != $cur) {
				if ($prevTable)
					$content .= "</table>\n";
				if ($content != '')
					$content .= "</pre>\n";
				$content .= "<pre>";
			}
			$content .= '<b>' . str_replace('[] = ', '</b> = ', htmlspecialchars($row)) . "\n";
			$prevTable = false;
			$last = $cur;
			continue;
		}

		if ($last != $cur) {
			if ($prevTable)
				$content .= "</table>\n";
			if ($content != '')
				$content .= "</pre>\n";
			$content .= "<pre><table>\n";
		}
		$content .= _print_row($row);
		$prevTable = true;
		$last = $cur;
	}
	if ($prevTable)
		$content .= "</table></pre>\n";
	return $content;
}

function _print_row($row) {
	$splitpos = strpos($row, '] = ');
	$cols = explode('[', substr($row, 0, $splitpos + 1));
	$keycount = count($cols) - 1;
	$cols[] = substr($row, $splitpos + 4);

	$content = '<tr>';
	for ($i = 0; $i < count($cols); $i++) {
		$col = $cols[$i];
		if ($i > 0 && $i <= $keycount)
			$col = trim(substr($col, 0, -1));
		if ($i == count($cols) - 1)
			$content .= '<td style="width: 100%;">';
		else
			$content .= '<td style="min-width: 100px;">';
		if ($i == 0)
			$content .= '<b>';
		else if ($i <= $keycount)
			$content .= '<i>';
		$content .= htmlspecialchars($col);
		if ($i == 0)
			$content .= '</b>';
		else if ($i < $keycount)
			$content .= '</i>';
		$content .= '</td>';
	}
	$content .= '</tr>';

	return $content;
}

?>
