#!/bin/bash

for i in funt_config_0.*;
do
    cd $i
    lines=(`cat eval_result.txt | wc -l`)
    if [ $lines -ne 56730 ]
    then
        echo $i
        echo $lines
        echo
    fi
    cd ..
done
