<?php
declare(strict_types=1);


function f(?string $a) : void {
  write($a);
}

f("test");
