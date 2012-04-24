#!/bin/bash

if [ -z $@ ]; then
	echo "No args. Usage: ${0} component_definition_file"
	exit
fi

FICHIER=`cat $@`
ECRITURE="/tmp/test"
NOML=`echo "$FICHIER" | grep "Component {" | cut -d" " -f2` #nom du systeme avec Component
NOMC=`echo $NOML | sed s/"Component"/""/g` #nom du systeme sans Component
DIMINUTIF=`echo ${NOMC:0:1}c |tr 'A-Z' 'a-z'` #premiere lettre du systeme

IFS=$'\n' #au lieu que le comparateur pour les boucles soit l'espace, c'est le retour à la ligne (il traite ligne par ligne plutot que mot par mot)

for i in $FICHIER; do
	Z=`echo $i | grep "_PROP"`
	if [ ! -z "$Z" ]; then
		SAUVEGARDE=$SAUVEGARDE`echo $Z | cut -d " " -f 2,3| tr -d ';'`$IFS
	fi
done


# fonction std::string createTableGrid()
	FINAL="std::string createTable$NOMC () {
	return \"create table $NOMC(e number(5) foreign key, "

	#pour chaque variable enregistré, on regarde son type et on rajoute dans la liste
	for i in $SAUVEGARDE; do
		#si c'est un int, bool, float ou double, on le met dans la table dans une case "number"
		TYPE=`echo $i | cut -d" " -f 1`
		VAR=`echo $i | cut -d" " -f 2`
		case "$TYPE" in
			"int"|"float"|"double"|"bool"|"Bool")
				FINAL=$FINAL"$VAR number(10), "
				;;
			*)
				FINAL=$FINAL"$VAR char2(10), "
				;;
		esac
	done
	#on rajoute le truc dirty
	FINAL=$FINAL"dirty number(10));\""$IFS"}"$IFS$IFS
#end fonction createTableGrid

#fonction std::string insertIntoTableGrid(Entity e, GridComponent* gc)
	FINAL=$FINAL"std::string insertIntoTable$NOMC(Entity e, $NOML* $DIMINUTIF, bool dirty) {
		std::stringstream s;
		s << \"insert into $NOMC VALUES('\" << e << \"','\" << "
	for i in $SAUVEGARDE; do
		TYPE=`echo $i | cut -d" " -f 1`
		VAR=`echo $i | cut -d" " -f 2`
		FINAL=$FINAL"$NOML->$VAR << \"', '\" << "
	done
	#on rajoute dirty
	FINAL=$FINAL"dirty << \"');\""
	FINAL=$FINAL$IFS"	return s.str();"$IFS"}"$IFS$IFS
#end fonction insertIntoTableGrid

#fonction std::string updateIntoTableGrid(Entity e, GridComponent * gc)
	FINAL=$FINAL"std::string updateIntoTable$NOMC(Entity e, $NOML* $DIMINUTIF, bool dirty) {
	std::stringstream s;
	s << \"update $NOMC set "
	for i in $SAUVEGARDE; do
		TYPE=`echo $i | cut -d" " -f 1`
		VAR=`echo $i | cut -d" " -f 2`
		FINAL=$FINAL"$VAR='\" << $NOML->$VAR<<\"', "
	done
	#on rajoute dirty
	FINAL=$FINAL"dirty='\"<<dirty << \"' where e=\"<<e<<\")\";"
	FINAL=$FINAL$IFS"	return s.str();"$IFS"}"$IFS$IFS
#end fonction updateIntoTableGrid

#fonction void retrieveGridComponent(Entity e, GridComponent * gc)
	FINAL=$FINAL"void retrieve$NOMCComponent(Entity e, $NOML* $DIMINUTIF) {
	std::stringstream s;
	s << \"select * from $NOMC where e=\"<< e << \")\";"
	#exec(s.str(), feedback)
	FINAL=$FINAL$IFS"}"
#end fonction retrieveComponent

#on ecrit maintenant dans le fichier
echo "$FINAL"
#echo "$FINAL" > $ECRITURE




















