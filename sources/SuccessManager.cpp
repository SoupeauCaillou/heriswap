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
#include "SuccessManager.h"

/* Proto :
void SuccessManager::sName() {
	if (hardMode) {
		if () {
			bName = true;
		}
	}
}
*/

SuccessManager::SuccessManager(SuccessAPI* sAPI) {
	successAPI = sAPI;
	hardMode=true;

	bRainbow = false;
	bDoubleRainbow = false;
	bLevel10 = false;
	bLevel1For2K = false;
	bExterminaScore = false;
	bTakeYourTime = false;
	bResetGrid = false;
	bFastAndFinish = false;
	bHardScore = false;
	b6InARow = false;
	bLuckyLuke = false;
	bTestEverything = false;
	bBTAC = false;
	bBTAM = false;
	b666Loser = false;
	bTheyGood = false;
	bWhatToDo = false;
	bBimBamBoum = false;
	bDoubleInOne = false;
	bBonusToExcess = false;


	timeTotalPlayed = 0.f;
	timeUserInputloop = 0.f;
	timeInSwappingPreparation = 0.f;

	numberCombinationInARow = 0;
	//ne dois pas etre reinit a chaque game :
	l666numberLose = 0;
	lTheyGood = 0;
	gameDuration = 0;
}

void SuccessManager::NewGame(Difficulty difficulty) {
	hardMode = (difficulty == DifficultyHard);

	gridResetted = false;

	numberCombinationInARow = 0;

	bonusTilesNumber = 0;
	for (int i=0; i<8; i++) {
		succEveryTypeInARow[i] = 0;
	}
}

void SuccessManager::s6InARow(int nbInCombi) {
	if (hardMode && !b6InARow) {
		if (nbInCombi >= 6) {
			successAPI->successCompleted("6 in a row", 3430);
			b6InARow=true;
		}
	}
}

void SuccessManager::sHardScore(StorageAPI* storage) {
	if (!bHardScore) {
		if (storage->getSavedGamePointsSum() > 1000000) {
			successAPI->successCompleted("Hardscore gamer", 3432);
			bHardScore=true;
		}
	}
}
void SuccessManager::sFastAndFinish(float time) {
	if (hardMode && !bFastAndFinish) {
		if (time<=53.f) {
			successAPI->successCompleted("Fast and finish", 2988);
			bFastAndFinish = true;
		}
	}
}

void SuccessManager::sResetGrid() {
	if (hardMode && !bResetGrid) {
		if (!gridResetted) {
			successAPI->successCompleted("Don't reset the grid !", 3418);
			bResetGrid = true;
		}
	}
}

void SuccessManager::sTakeYourTime() {
	if (hardMode && !bTakeYourTime) {
		if (gameDuration > 900.) {
			successAPI->successCompleted("Take your time", 3424);
			bTakeYourTime = true;
		}
	}
	gameDuration = 0.;
}

void SuccessManager::sExterminaScore(int points) {
	if (hardMode && !bExterminaScore) {
		if (points > 100000) {
			successAPI->successCompleted("Exterminascore", 3422);
			bExterminaScore = true;
		}
	}
}
void SuccessManager::sLevel1For2K(int level, int points) {
	if (hardMode && !bLevel1For2K) {
		if (level==1 && points>=2000) {
			successAPI->successCompleted("2k points for level 1", 3434);
			bLevel1For2K = true;
		}
	}
}

void SuccessManager::sLevel10(int level) {
	if (hardMode && !bLevel10) {
		if (level == 10) {
			successAPI->successCompleted("Level 10", 3426);
			bLevel10 = true;
		}
	}
}

bool rainbow(int* successType) {
	for (int i=0; i<8; i++) {
		if (successType[i]==0) return false;
	}
	return true;
}

void SuccessManager::sRainbow(int type) {
	if (hardMode && (!bRainbow || !bDoubleRainbow)) {
		if (succEveryTypeInARow[type]) {
			for (int i=0; i<8; i++) succEveryTypeInARow[i] = 0;
			bRainbow = false;
		} else {
			succEveryTypeInARow[type] = 1;
		}
		if (rainbow(succEveryTypeInARow)) {
			if (!bRainbow) {
				successAPI->successCompleted("Rainbow combination", 3440);
				bRainbow = true;
			} else {
				successAPI->successCompleted("Double rainbow ", 3444);
				bDoubleRainbow = true;
			}
		}
	}
}
void SuccessManager::sBonusToExcess(int type, int bonus, int nb) {
	if (hardMode && !bBonusToExcess) {
		if (type == bonus)
			bonusTilesNumber +=nb;
		if (bonusTilesNumber >= 100) {
			successAPI->successCompleted("Bonus bonus", 3420);
			bBonusToExcess = true;
		}
	}
}

void SuccessManager::sLuckyLuke() {
	if (hardMode && !bLuckyLuke) {
		if (timeUserInputloop<2.f)
			timeTotalPlayed += timeUserInputloop;
		else
			timeTotalPlayed = 0.f;

		if (timeTotalPlayed >= 15.f) {
			successAPI->successCompleted("Lucky Luke", 3442);
			bLuckyLuke = true;
		}
	}
}

void SuccessManager::sTestEverything(StorageAPI* str) {
	if (!bTestEverything) {
		if (str->everyModesPlayed()) {
			successAPI->successCompleted("Test everything", 3454);
			bTestEverything = true;
		}
	}
}

void SuccessManager::sBTAC(StorageAPI* storage, Difficulty difficulty, unsigned int points) {
	if (!bBTAC) {
		float avg;
	    std::vector<StorageAPI::Score> entries = storage->savedScores(1, difficulty, avg);
		int s = entries.size();
		if (s >= 5 && (int)points > entries[0].points) {
			successAPI->successCompleted("Beat them all (score race)", 3436);
			bBTAC = true;
		}
	}
}

void SuccessManager::sBTAM(StorageAPI* storage, Difficulty difficulty, float time) {
	if (!bBTAM) {
		float avg;
		std::vector<StorageAPI::Score> entries = storage->savedScores(2, difficulty, avg);
		int s = entries.size();
		if (s >= 5 && time < entries[0].time) {
			successAPI->successCompleted("Beat them all (time attack)", 3428);
			bBTAM = true;
		}
	}
}

void SuccessManager::s666Loser(int level) {
	if (hardMode && !b666Loser) {
		if (level==6)
			l666numberLose++;
		else
			l666numberLose = 0;

		if (l666numberLose==3) {
			successAPI->successCompleted("666 Loser !", 3438);
			b666Loser = true;
		}
	}
}

void SuccessManager::sTheyGood(bool Ibetter) {
	if (!bTheyGood) {
		if (!Ibetter)
			lTheyGood++;
		else
			lTheyGood=0;

		if (lTheyGood==3) {
			successAPI->successCompleted("They're too good", 3446);
			bTheyGood = true;
		}
	}
}

void SuccessManager::sWhatToDo(bool swapInPreparation, float dt) {
	if (!bWhatToDo) {
		if (swapInPreparation)
			timeInSwappingPreparation+=dt;
		else
			timeInSwappingPreparation=0.f;

		if (timeInSwappingPreparation>5.0f) {
			successAPI->successCompleted("What I gonna do ?", 3448);
			bWhatToDo = true;
		}
	}
}

// Delete::Enter gives the number of combinations it'll delete
// User::Enter cancel success (user take the control) so it gives 0
void SuccessManager::sBimBamBoum(int newCombiCount) {
	if (hardMode && !bBimBamBoum) {
		if (newCombiCount == 0) {
			numberCombinationInARow = 0;
		} else {
			numberCombinationInARow += 1;
		}

		if (numberCombinationInARow >= 3) {
			successAPI->successCompleted("Bim Bam Boum", 3450);
			bBimBamBoum = true;
		}
	}
}

void SuccessManager::sDoubleInOne(std::vector<Combinais> &s) {
	if (hardMode && !bDoubleInOne) {
		if (s.size() > 1) {
			successAPI->successCompleted("Double in one", 3452);
			bDoubleInOne = true;
		}
	}
}
