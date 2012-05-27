#!/bin/bash

while read LINE; do
	w=`echo $LINE | grep WARN`
	e=`echo $LINE | grep ERRO`
	i=`echo $LINE | grep INFO`

	#local colors have priority
	if [ -n "`echo $LINE | grep SuccessAPI::successCompleted`" ]; then
		echo -e "\033[1m\E[91m$LINE\033[0m"; tput sgr0
	#end local
	elif [ -n "$w" ]; then
		echo -e "\033[1m\E[33m$LINE\033[0m"; tput sgr0
	elif [ -n "$e" ]; then
		echo -e "\033[1m\E[31m$LINE\033[0m"; tput sgr0
	elif [ -n "$i" ]; then
		echo -e "\033[0m\E[36m$LINE\033[0m"; tput sgr0
	else
		echo -e "$LINE"
	fi
done

exit 0
