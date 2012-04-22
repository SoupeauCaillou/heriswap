#!/bin/bash

if [ -z $@ ]; then
	echo "No args. Usage: ${0} component_definition_file"
else
	FICHIER=`cat $@`
	ECRITURE="/tmp/test"
	NOM=`echo "$FICHIER" | grep "Component {" | cut -d" " -f2` #nom du systeme avec Component
	DIMINUTIF=`echo ${NOM:0:1}c |tr 'A-Z' 'a-z'`

	##pour virer le morceau component :
	#tot=`echo "$NOM"|wc -c`
	#total=`expr $tot - 10`
	#NOM=${NOM:0:$total}

	old_IFS=$IFS
	IFS=$'\n'

	for i in $FICHIER; do
		Z=`echo $i | grep "PERSISTENT"`
		if [ ! -z "$Z" ]; then
			SAUVEGARDE=$SAUVEGARDE`echo $Z | cut -d " " -f 2,3| tr -d ';'`$IFS
		fi
	done


	#fonction serializeSize
	FINAL="int "$NOM"_serializationSize($NOM* $DIMINUTIF) {"
	FINAL=$FINAL$IFS"int size = 0;"
	for i in $SAUVEGARDE; do
		if [ ! -z `echo "$i" | grep "std::string"` ]; then #si c'est un string
			FINAL=$FINAL$'\n'"size += $DIMINUTIF->${i#* }.length() + 1;"
		else #sinon
			FINAL=$FINAL$'\n'"size += sizeof($DIMINUTIF->${i#* });"
		fi
	done
	FINAL=$FINAL$IFS"return size;"$'\n'"}"$'\n'

	#fonction serialize
	FINAL=$FINAL$IFS"int "$NOM"_serialization($NOM* $DIMINUTIF, uint8_t* out) {"
	for i in $SAUVEGARDE; do
		if [ ! -z `echo "$i" | grep "std::string"` ]; then #si c'est un string
			FINAL=$FINAL$'\n'"out = (uint8_t*) mempcpy(out, &$DIMINUTIF->${i#* }.c_str(), $DIMINUTIF->${i#* }.c_str().length() + 1);"
		else #sinon
			FINAL=$FINAL$'\n'"out = (uint8_t*) mempcpy(out, $DIMINUTIF->${i#* }, sizeof($DIMINUTIF->${i#* }));"
		fi
	done
	FINAL=$FINAL$'\n'"return "$NOM"_serializationSize();$IFS}$IFS"

	#fonction deserialize
	FINAL=$FINAL$IFS"void "$NOM"_deserialize($NOM* $DIMINUTIF, uint8_t* in, int size) {"
	for i in $SAUVEGARDE; do
		if [ ! -z `echo "$i" | grep "std::string"` ]; then #si c'est un string
			FINAL=$FINAL$'\n'"$DIMINUTIF->${i#* } = std::string(in); in += $DIMINUTIF->${i#* }.length() + 1;"
		else #sinon
			FINAL=$FINAL$'\n'"memcpy(&$DIMINUTIF->${i#* }, in, sizeof($DIMINUTIF->${i#* })); in += sizeof($DIMINUTIF->${i#* });"
		fi
	done
	FINAL=$FINAL$IFS"}"




	#on ecrit maintenant dans le fichier
	echo "$FINAL" > $ECRITURE

	IFS=$old_IFS
fi























