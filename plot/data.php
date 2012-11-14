<?php
/*
	Awnser with data on json format {"ref":[[12,37], ...],"out":[[5,12], ...],"in":[[2,52], ...],...}.
	The point given after curve name will be added to that curve (eg. [12,37] and optinally more
	points will be added to the curve ref in the plot).

	New curves can be added or skipped as pleased.

	The JS will (except when it has no data) send the curve name and a value as an get
	argument. The value will be the last plotted x value on that curve.
	Eg. data.php?ref=12&out=5&in=2
*/


	header('Cache-Control: no-cache, must-revalidate');
	header('Content-type: application/json;charset=utf-8');

	if (isset($_GET["ref"]) && isset($_GET["out"]) && isset($_GET["in"])) {
		if (is_numeric($_GET["ref"]) && is_numeric($_GET["out"]) && is_numeric($_GET["in"])) {
			echo json_encode(array('ref' =>
				array(array(rand($_GET["ref"], $_GET["ref"] + 10), rand(0, 100))),
			      'out' =>
				array(array(rand($_GET["out"], $_GET["out"] + 10), rand(0, 100))),
			      'in' =>
				array(array(rand($_GET["in"], $_GET["in"] + 10), rand(0, 100)))));
			exit;
		}
	}

	echo json_encode(array('ref' => array(array(rand(0, 20), rand(0, 100))),
			       'out' => array(array(rand(0, 20), rand(0, 100))),
			       'in' => array(array(rand(0,20), rand(0, 100)))));
?>
