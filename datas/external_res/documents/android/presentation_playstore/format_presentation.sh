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
semantic[0]="Description:\n"
semantic[1]="\n\nText:\n"
semantic[2]="\n"
semantic[3]="\n"
semantic[4]="\n\n"
semantic[5]="\n"
semantic[6]="\n"
semantic[7]="\n"
semantic[8]="\n"
semantic[9]="\n"
semantic[10]="\n"
semantic[11]="\n"
semantic[12]="\n"
semantic[13]="\r"
semantic[14]="\r"


function parse_file {
    c=1
    echo -ne "${semantic[0]}"
    while read line; do
        echo -ne "$line ${semantic[$c]}"
        c=$(($c + 1))
    done < $file
    echo -ne "$line ${semantic[$c]}"
}



for file in $files; do
    echo -e "##########################Language: $file####################"
    parse_file $file
    echo -e "##########################End language: $file#############"
done
