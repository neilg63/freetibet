<?php


// Returns true if code has already been used, otherwise false and prevents the code being used again
function get_or_make_unique($code) {

	$count =  db_query("SELECT count(*) FROM UniqueLinks WHERE Code = '" . $code . "'")->fetchField();

	if ($count == 0) {
		db_insert('UniqueLinks')
		->fields(array(
		  'Code' => $code,
		))
		->execute();

		return false;
	} else {
		return true;
	}

}


// Sanitise unique ID for safe DB query
function sanitise_uqid($uqid) {
	$uqid = trim($uqid);
	if(preg_match('/[^a-z_\-0-9]/i', $uqid)) {
		return FALSE;
	}
	else {
		return $uqid;
	}
}


// Sanitise email
function sanitise_email($email) {
	return filter_var($email, FILTER_VALIDATE_EMAIL);
}