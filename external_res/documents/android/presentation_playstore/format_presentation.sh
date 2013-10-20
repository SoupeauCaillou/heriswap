#!/bin/bash


cd $(dirname $0)

if [ $# = 0 ]; then
    echo -e "No arg provided. Formatting every languages\n"
    sleep 1
    files=*.txt
else
    files=$@
fi

declare -A semantic
#add there your specific presentation format
semantic[1]="\n"


function parse_file {
    c=1
    while read line; do
        echo -e "$line ${semantic[$c]}"
        c=$(($c + 1))
    done < $file
}



for file in $files; do
    echo -e "##########################Language: $file####################"
    parse_file $file
    echo -e "##########################End language: $file#############"
done
