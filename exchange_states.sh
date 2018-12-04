#!/bin/bash
STATES=`cat states`
FILE=doc/fsm.easy.gv
COUNTER=0

for state in $STATES
do
    sed -i "s/$state /$COUNTER /g" $FILE
    ((COUNTER++))
done


