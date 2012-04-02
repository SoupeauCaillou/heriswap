#/bin/bash

for i in {A..Z}; 
do 
	convert -background transparent -fill white -font Cantarell-Bold -pointsize 50 label:$i $i.png
done

for i in {a..z}; 
do 
	convert -background transparent -fill white -font Cantarell-Bold -pointsize 50 label:$i $i.png
done

for i in {0..9};
do 
        convert -background transparent -fill white -font Cantarell-Bold -pointsize 50 label:$i $i.png
done


