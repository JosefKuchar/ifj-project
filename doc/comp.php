<?php

$vars = [-1, 0, 1, -1.0, -0.5, 0, 0.5, 1.0, "", "aa", "ab", null];


print_r("X,");
foreach ($vars as $key => $var) {
	print_r(var_export($var, true));
	print_r($key === count($vars) -1 ? "\n" : ",");
}

foreach ($vars as $var1) {
	print_r(var_export($var1, true) . ",");
	foreach ($vars as $key => $var2) {
		print_r($var1 > $var2 ? "true" : "false");
		print_r($key === count($vars) -1 ? "\n" : ",");
	}
}
