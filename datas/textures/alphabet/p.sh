#!/bin/bash

mkdir tmp

for i in `ls *png`
do
    a=`echo $i | cut -d_ -f1`
    b=`echo $i | cut -d_ -f2`
    dec=`printf "%x" "$a"`
    #echo "'$i' -> '${dec}_${b}'"
    cp ${i} tmp/${dec}_${b}
done
