#!/bin/sh

for i in `ls *.png`;
do
	convert -resize 50% $i $i
done
