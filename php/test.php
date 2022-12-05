<?php
declare(strict_types=1);
$i = 10;
for ($i = 0; $i < 10; $i = $i + 1) {
    for ($j = 0; $j < 10; $j = $j + 1) {
        write($j, "\n");
        if ($j === 5) {
            break;
        } else{}
    }
    if ($i === 6) {
        write("Break\n");
        break;
    } else{}
}
?>
