<?php
	header('Cache-Control: no-cache, must-revalidate');
	header('Content-type: application/json;charset=utf-8');

	if (isset($_GET["ref"]) && isset($_GET["out"]) && isset($_GET["in"])) {
		if (is_numeric($_GET["ref"]) && is_numeric($_GET["out"]) && is_numeric($_GET["in"])) {
			echo json_encode(array('ref' =>
				array(rand($_GET["ref"], $_GET["ref"] + 10), rand(0, 100)),
			      'out' =>
				array(rand($_GET["out"], $_GET["out"] + 10), rand(0, 100)),
			      'in' =>
				array(rand($_GET["in"], $_GET["in"] + 10), rand(0, 100))));
			exit;
		}
	}

	echo json_encode(array('ref' => array(rand(0, 20), rand(0, 100)),
			       'out' => array(rand(0, 20), rand(0, 100)),
			       'in' => array(rand(0,20), rand(0, 100))));
?>
