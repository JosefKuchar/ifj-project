<?php

// There can be comments

/*
Or multiline comments
*/

declare(strict_types=1);

function foo(string $test, ?int $haha): string {
    if (1 + 1 === 2) {
        return "Hello world";
    } else {
        $haha = 3 * (6 + 5) + 35e2;
        while($haha < 100) {
            $haha = $haha + 1;
            $test = foo($test, $haha);
            1 + 5;
        }
    }
}

// {}*-,;><==

// ($_ >= $asdf)
// else

// test()
