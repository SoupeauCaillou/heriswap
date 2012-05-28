#!/bin/sh

for i in `ls $1`;
do
	convert -resize $2% $i $i
done
