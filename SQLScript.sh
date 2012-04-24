#~ struct GridComponent {
	#~ GridComponent() {
		#~ i = -1 ;
		#~ j = -1;
		#~ type = -1;
		#~ checkedV = false;
		#~ checkedH = false;
	#~ }
	#~ PERSISTENT_PROP int i;
	#~ PERSISTENT_PROP int j;
	#~ PERSISTENT_PROP int type;
	#~ RUNTIME_PROP bool checkedV;
	#~ RUNTIME_PROP bool checkedH;
#~ };
#~ 
#~ 
#~ 19:44:31) pepp: 1) createTableGrid
#~ (19:44:52) pepp: qui renvoie une requête SQL pour créer une table qui s'appelerait GRID
#~ (19:45:23) pepp: avec 7 colonne : e(primaire)/i/j/type/checkedV/checkedH/dirty

#~ (19:45:58) pepp: 2) une fonction pour insère un enregistrement dans la table
#~ (19:46:22) pepp: insertIntoTableGrid(GridComponent* gc)
#~ (19:46:41) pepp: qui renvoie : "INSERT into GRID blabla" avec toutes les colonnes mises à jour
#~ (19:46:50) pepp: 3) un update qui fait pareil
#~ (19:47:04) pepp: (ou plutôt une fonction : insert ou update suivant si ça existe

#~ (19:47:45) pepp: et la fonction dans l'autre sens
#~ (19:48:07) pepp: qui recupère une entrée sql: select from grid where e == entity
#~ (19:48:22) pepp: et qui mets à jour un GridComponent

#!/bin/bash

if [ -z $@ ]; then
	echo "No args. Usage: ${0} component_definition_file"
else
	FICHIER=`cat $@`
	ECRITURE="/tmp/test"
	NOM=`echo "$FICHIER" | grep "Component {" | cut -d" " -f2` #nom du systeme avec Component
	DIMINUTIF=`echo ${NOM:0:1}c |tr 'A-Z' 'a-z'`

	old_IFS=$IFS
	IFS=$'\n'

	for i in $FICHIER; do
		Z=`echo $i | grep "PERSISTENT"`
		if [ ! -z "$Z" ]; then
			SAUVEGARDE=$SAUVEGARDE`echo $Z | cut -d " " -f 2,3| tr -d ';'`$IFS
		fi
	done
	echo $SAUVEGARDE
fi

#~ 
	#~ #fonction serializeSize
	#~ FINAL="int "$NOM"_serializationSize($NOM* $DIMINUTIF) {"
	#~ FINAL=$FINAL$IFS"int size = 0;"
	#~ for i in $SAUVEGARDE; do
		#~ if [ ! -z `echo "$i" | grep "std::string"` ]; then #si c'est un string
			#~ FINAL=$FINAL$'\n'"size += $DIMINUTIF->${i#* }.length() + 1;"
		#~ else #sinon
			#~ FINAL=$FINAL$'\n'"size += sizeof($DIMINUTIF->${i#* });"
		#~ fi
	#~ done
	#~ FINAL=$FINAL$IFS"return size;"$'\n'"}"$'\n'
#~ 
	#~ #fonction serialize
	#~ FINAL=$FINAL$IFS"int "$NOM"_serialization($NOM* $DIMINUTIF, uint8_t* out) {"
	#~ for i in $SAUVEGARDE; do
		#~ if [ ! -z `echo "$i" | grep "std::string"` ]; then #si c'est un string
			#~ FINAL=$FINAL$'\n'"out = (uint8_t*) mempcpy(out, &$DIMINUTIF->${i#* }.c_str(), $DIMINUTIF->${i#* }.c_str().length() + 1);"
		#~ else #sinon
			#~ FINAL=$FINAL$'\n'"out = (uint8_t*) mempcpy(out, $DIMINUTIF->${i#* }, sizeof($DIMINUTIF->${i#* }));"
		#~ fi
	#~ done
	#~ FINAL=$FINAL$'\n'"return "$NOM"_serializationSize();$IFS}$IFS"
#~ 
	#~ #fonction deserialize
	#~ FINAL=$FINAL$IFS"void "$NOM"_deserialize($NOM* $DIMINUTIF, uint8_t* in, int size) {"
	#~ for i in $SAUVEGARDE; do
		#~ if [ ! -z `echo "$i" | grep "std::string"` ]; then #si c'est un string
			#~ FINAL=$FINAL$'\n'"$DIMINUTIF->${i#* } = std::string(in); in += $DIMINUTIF->${i#* }.length() + 1;"
		#~ else #sinon
			#~ FINAL=$FINAL$'\n'"memcpy(&$DIMINUTIF->${i#* }, in, sizeof($DIMINUTIF->${i#* })); in += sizeof($DIMINUTIF->${i#* });"
		#~ fi
	#~ done
	#~ FINAL=$FINAL$IFS"}"
#~ 
	#~ #on ecrit maintenant dans le fichier
	#~ echo "$FINAL" > $ECRITURE
#~ 
	#~ IFS=$old_IFS
#~ fi
#~ 
#~ 
#~ 




















