#!/bin/bash

# IFJ22 - Priklady pro studenty pro dokresleni zadani (spusteni konfigurovano pro server Merlin)

COMPILER="../main "  # zde muze byt vas prekladac IFJ22 (napr. ./ifj22)
HELPER="ifj22.php"
INTERPRETER="../ic22int "  # zde muze byt interpret IFJcode22 (./ic22int)
RETURNCODE=0

for i in *.php; do
    
    if [[ $i == $HELPER ]]; then
        continue
    fi
    
    IN=`echo $i | sed 's/\(.*\)\.php/\1.in/g'`
    OUT=`echo $i | sed 's/\(.*\)\.php/\1.out/g'`
    PRG="out.ifjc22"
    
    $COMPILER < "$i" > $PRG   # nekontroluji se chyby prekladu (pro chybove testy nutno rozsirit)
    echo "$PRG"
    
    echo -n "DIFF: $INTERPRETER $PRG and $OUT: "
    cat $IN | $INTERPRETER $PRG 2> >(sed $'s,.*,\e[31m&\e[m,'>&2) | diff - $OUT > /dev/null 2> /dev/null
    DIFFOK=$?
    if [ "x$DIFFOK" == "x0" ]; then
        echo "OK"
    else
        echo "DIFFERENCE to $OUT"
        cat $IN | $INTERPRETER $PRG 2> >(sed $'s,.*,\e[31m&\e[m,'>&2) | diff - $OUT
        RETURNCODE=1
    fi
    echo
done;

# rm *.tmp

exit $RETURNCODE
