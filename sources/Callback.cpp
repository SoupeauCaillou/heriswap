#include "Callback.h"
#include "api/StorageAPI.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "Game.h"

int callbackSc(void *save, int argc, char **argv, char **azColName){
	int *sav = static_cast<int*>(save);
	for (int i=0; i<argc; i++) {
		if (!strcmp(azColName[i],"points")) {
			*sav += atoi(argv[i]);
			return 0;
		}
	}
	return 0;
}

int callbackScore(void *save, int argc, char **argv, char **azColName){
	int i;
	// name | mode | difficulty | points | time
	std::vector<StorageAPI::Score> *sav = static_cast<std::vector<StorageAPI::Score>* >(save);
	StorageAPI::Score score1;
	for(i=0; i<argc; i++){
		std::istringstream iss(argv[i]);
		if (!strcmp(azColName[i],"name")) {
			score1.name = argv[i];
		} else if (!strcmp(azColName[i],"points")) {
			iss >> score1.points;
		} else if (!strcmp(azColName[i],"time")) {
			iss >> score1.time;
		} else if (!strcmp(azColName[i],"level")) {
			iss >> score1.level;
		}
	}
	sav->push_back(score1);
	return 0;
}

int callbackNames(void *save, int argc, char **argv, char **azColName){
	std::vector<std::string> *sav = static_cast<std::vector<std::string>*>(save);
	for (int i=0; i<argc; i++) {
		sav->push_back(argv[i]);
	}
	return 0;
}

int callback(void *save, int argc, char **argv, char **azColName){
	std::string *sav = static_cast<std::string*>(save);
	*sav = argv[0];
	return 0;
}
