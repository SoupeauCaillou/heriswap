#!/bin/bash

while read LINE; do
	w=`echo $LINE | grep WARN`
	e=`echo $LINE | grep ERRO
	`
	if [ -n "$w" ]; then
		echo -e "\033[1m\E[33m$LINE\033[0m"; tput sgr0
	elif [ -n "$e" ]; then
		echo -e "\033[1m\E[31m$LINE\033[0m"; tput sgr0
	else
		echo -e "$LINE"
	fi
done

exit 0
