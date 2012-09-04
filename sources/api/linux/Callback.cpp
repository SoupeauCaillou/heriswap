/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Callback.h"
#include "api/StorageAPI.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "HeriswapGame.h"

int callback(void *save, int argc, char **argv, char **azColName){
	std::string *sav = static_cast<std::string*>(save);
	if (argv[0] != 0)
		*sav = argv[0];
	return 0;
}

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

int callbackResultSize(void *save, int argc, char **argv, char **azColName){
	int *sav = static_cast<int*>(save);
	(*sav)++;
	return 0;
}

