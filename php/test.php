<?php
declare(strict_types=1);

$a = 10;

function g(): ?int {
    return 110;
}

$a = g();
write($a);
