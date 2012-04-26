#!/bin/bash

if [ -z $@ ]; then
	echo "No Args."
	echo "Usage: bash ${0} component_definition_file"
	exit
elif [ ! -e $@ ]; then
	echo "$@ not found."
	echo "Usage: bash ${0} component_definition_file"
	exit
fi

FICHIER=`cat $@`
ECRITURE="/tmp/ResSQL.cpp"
DEBUG_FILE="/tmp/DebugSQL.cpp"
DEBUG_ENABLE="YES"

NOML=`echo "$FICHIER" | grep "Component {" | cut -d" " -f2` #nom du systeme avec Component (GridComponent)
NOMC=`echo $NOML | sed s/"Component"/""/g` #nom du systeme sans Component (Grid)
DIMINUTIF=`echo ${NOMC:0:1}c |tr 'A-Z' 'a-z'` #diminutif du systeme (gc)

IFS=$'\n' #au lieu que le comparateur pour les boucles soit l'espace, c'est le retour à la ligne (il traite ligne par ligne plutot que mot par mot)

for i in $FICHIER; do
	Z=`echo $i | grep "_PROP"`
	if [ ! -z "$Z" ]; then
		SAUVEGARDE=$SAUVEGARDE`echo $Z | cut -d " " -f 2,3| tr -d ';'`$IFS
	fi
done


# function void createTableGrid(ScoreStorage* ss)
	FINAL="void createTable$NOMC (ScoreStorage* ss) {
	ss->exec(\"create table $NOMC(e number(5) foreign key, "

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
	#on rajoute dirty
	FINAL=$FINAL"dirty number(10))\", 0, 0);"$IFS"}"$IFS
#end function createTableGrid

#function void insertIntoTableGrid(ScoreStorage* ss, Entity e, GridComponent* gc)
	FINAL=$FINAL$IFS"void insertIntoTable$NOMC(ScoreStorage* ss, Entity e, $NOML* $DIMINUTIF, bool dirty) {
	std::stringstream s;
	s << \"insert into $NOMC VALUES('\" << e << \"','\" << "
	for i in $SAUVEGARDE; do
		TYPE=`echo $i | cut -d" " -f 1`
		VAR=`echo $i | cut -d" " -f 2`
		FINAL=$FINAL"$NOML->$VAR << \"', '\" << "
	done
	#on rajoute dirty
	FINAL=$FINAL"dirty << \"');\""
	FINAL=$FINAL$IFS"	ss->exec(s.str(), 0, 0);"$IFS"}"$IFS
#end function insertIntoTableGrid

#function void updateIntoTableGrid(ScoreStorage* ss, Entity e, GridComponent * gc)
	FINAL=$FINAL$IFS"void updateIntoTable$NOMC(ScoreStorage* ss, Entity e, $NOML* $DIMINUTIF, bool dirty) {
	std::stringstream s;
	s << \"update $NOMC set "
	for i in $SAUVEGARDE; do
		TYPE=`echo $i | cut -d" " -f 1`
		VAR=`echo $i | cut -d" " -f 2`
		FINAL=$FINAL"$VAR='\" << $NOML->$VAR<<\"', "
	done
	#on rajoute dirty
	FINAL=$FINAL"dirty='\"<<dirty << \"' where e=\"<<e<<\")\";"
	FINAL=$FINAL$IFS"	ss->exec(s.str(), 0, 0);"$IFS"}"$IFS
#end function updateIntoTableGrid

#function static int callbackGrid(GridComponent *gc, int argc, char **argv, char **azColName)
	FINAL=$FINAL$IFS"static int callback$NOMC($NOML *$DIMINUTIF, int argc, char **argv, char **azColName) {
	for (int i=0; i<argc; i++) {
		std::istringstream iss(argv[i]);"$IFS"		"
		for i in $SAUVEGARDE; do
		TYPE=`echo $i | cut -d" " -f 1`
		VAR=`echo $i | cut -d" " -f 2`
		FINAL=$FINAL"if (!strcmp,azColName[i],\"$VAR\")) {"
			if [ "$TYPE" = "std::string" ]; then
				FINAL=$FINAL$IFS"			gc->$VAR = argv[i];"
			else
				FINAL=$FINAL$IFS"			iss >> gc->$VAR;"
			fi
		FINAL=$FINAL$IFS"		} else "
		done
		#on rajoute dirty
	FINAL=$FINAL" if (!strcmp,azColName[i],\"dirty\")){
		//not supported : dirty=argv[i];$IFS		}"
	FINAL=$FINAL$IFS"	}$IFS	return 0;"$IFS"}"$IFS
#end function callbackGrid

#function void retrieveGridComponent(ScoreStorage* ss, Entity e, GridComponent * gc)
	FINAL=$FINAL$IFS"void retrieve$NOML(ScoreStorage* ss, Entity e, $NOML* $DIMINUTIF) {
	std::stringstream s;
	s << \"select * from $NOMC where e=\"<< e << \")\";"
	FINAL=$FINAL$IFS"	ss->exec(s.str(), $DIMINUTIF, callback$NOMC);"
	FINAL=$FINAL$IFS"}"
#end function retrieveComponent

#on ecrit maintenant dans le fichier
echo "$FINAL" > $ECRITURE




# en entrée : 

#struct GridComponent {
#	GridComponent() {
#		i = -1 ;
#		j = -1;
#		type = -1;
#		checkedV = false;
#		checkedH = false;
#	}
#	PERSISTENT_PROP int i;
#	PERSISTENT_PROP int j;
#	PERSISTENT_PROP int type;
#	RUNTIME_PROP bool checkedV;
#	RUNTIME_PROP bool checkedH;
#};


# en sortie on aura :

#	void createTableGrid (ScoreStorage* ss) {
#		ss->exec("create table Grid(e number(5) foreign key, i number(10), j number(10), type number(10), 
#			checkedV number(10), checkedH number(10), dirty number(10)), 0, 0);"
#	}
#
#	void insertIntoTableGrid(ScoreStorage* ss, Entity e, GridComponent* gc, bool dirty) {
#		std::stringstream s;
#		s << "insert into Grid VALUES('" << e << "','" << GridComponent->i << "', '" << GridComponent->j << "', '" << GridComponent->type << "', '" << GridComponent->checkedV << "', '" << GridComponent->checkedH << "', '" << dirty << "');"
#		ss->exec(s.str(), 0 ,0);
#	}
#  
#	void updateIntoTableGrid(ScoreStorage* ss, Entity e, GridComponent* gc, bool dirty) {
#		std::stringstream s;
#		s << "update Grid set i='" << GridComponent->i<<"', j='" << GridComponent->j<<"', type='" << GridComponent->type<<"', checkedV='" << GridComponent->checkedV<<"', checkedH='" << GridComponent->checkedH<<"', dirty='"<<dirty << "' where e="<<e<<")";
#		ss->exec(s.str(),0 ,0);
#	}
# 
#	static int callbackGrid(GridComponent *gc, int argc, char **argv, char **azColName){
#		for(i=0; i<argc; i++){
#			std::istringstream iss(argv[i]);
#			if (!strcmp(azColName[i],"i")) {
#				iss >> gc->i;
#			} else if (!strcmp(azColName[i],"j")) {
#				iss >> gc->j;
#			} else if (!strcmp(azColName[i],"type")) {
#				iss >> gc->type;
#			} else if (!strcmp(azColName[i],"checkedV")) {
#				iss >> gc->checkedV;
#			} else if (!strcmp(azColName[i],"checkedH")) {
#				iss >> gc->checkedH;
#			} else if (!strcmp(azColName[i],"dirty")) {
#				iss >> gc->dirty;
#			}
#		}
#		return 0;
#	}
#
#	void retrieveGridComponent(ScoreStorage* ss, Entity e, GridComponent* gc) {
#		std::stringstream s;
#		s << "select * from Grid where e="<< e << ")";
#		ss->exec(s.str(), gc, callbackGrid); 
#	}
 
#debug

DEBUG='void createTableGrid (ScoreStorage* ss) {
	ss->exec("create table Grid(e number(5) foreign key, i number(10), j number(10), type number(10), 
		checkedV number(10), checkedH number(10), dirty number(10)), 0, 0);"
}

void insertIntoTableGrid(ScoreStorage* ss, Entity e, GridComponent* gc, bool dirty) {
	std::stringstream s;
	s << "insert into Grid VALUES('" << e << "','" << GridComponent->i << "', '" << GridComponent->j << "', '" << GridComponent->type << "', '" << GridComponent->checkedV << "', '" << GridComponent->checkedH << "', '" << dirty << "');"
	ss->exec(s.str(), 0 ,0);
}

void updateIntoTableGrid(ScoreStorage* ss, Entity e, GridComponent* gc, bool dirty) {
	std::stringstream s;
	s << "update Grid set i='" << GridComponent->i<<"', j='" << GridComponent->j<<"', type='" << GridComponent->type<<"', checkedV='" << GridComponent->checkedV<<"', checkedH='" << GridComponent->checkedH<<"', dirty='"<<dirty << "' where e="<<e<<")";
	ss->exec(s.str(),0 ,0);
}
static int callbackGrid(GridComponent *gc, int argc, char **argv, char **azColName){
	for(i=0; i<argc; i++){
		std::istringstream iss(argv[i]);
		if (!strcmp(azColName[i],"i")) {
			iss >> gc->i;
		} else if (!strcmp(azColName[i],"j")) {
			iss >> gc->j;
		} else if (!strcmp(azColName[i],"type")) {
			iss >> gc->type;
		} else if (!strcmp(azColName[i],"checkedV")) {
			iss >> gc->checkedV;
		} else if (!strcmp(azColName[i],"checkedH")) {
			iss >> gc->checkedH;
		} else if (!strcmp(azColName[i],"dirty")) {
			iss >> gc->dirty;
		}
	}
	return 0;
}

void retrieveGridComponent(ScoreStorage* ss, Entity e, GridComponent* gc) {
	std::stringstream s;
	s << "select * from Grid where e="<< e << ")";
	ss->exec(s.str(), gc, callbackGrid); 
}'
	
if [ "$DEBUG_ENABLE" = "YES" ]; then 
	echo "$FINAL"
	echo "$IFS---------------------------" >> $DEBUG_FILE
	echo "Output should be : " >> $DEBUG_FILE
	echo "---------------------------$IFS" >> $DEBUG_FILE
	echo "$DEBUG" >> $DEBUG_FILE
	echo "$IFS *********************************$IFS Debug actived, wrote in $DEBUG_FILE"
fi
