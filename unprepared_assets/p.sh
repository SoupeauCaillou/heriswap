#!/bin/bash

for i in `ls *png`
do
    a=`echo $i | cut -d_ -f1`
    b=`echo $i | cut -d_ -f2`
    dec=`printf "%x" "$a"`
    #echo "'$i' -> '${dec}_${b}'"
    mv ${i} ${dec}_${b}
done
