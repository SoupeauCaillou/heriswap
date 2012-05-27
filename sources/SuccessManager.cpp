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
	bLevel1For1K = false;
	bExterminaScore = false;
	bTakeYourTime = false;
	bResetGrid = false;
	bFastAndFinish = false;
	bHardScore = false;
	b6InARow = false;
	bLuckyLuke = false;
	bDonator = false;
	bTestEverything = false;
	bBTAC = false;
	bBTAM = false;
	b666Loser = false;
	bTheyGood = false;
	bWhatToDo = false;
	bBimBamBoum = false;
	bDoubleInOne = false;

	timeTotalPlayed = 0.f;
	timeUserInputloop = 0.f;
	timeInSwappingPreparation = 0.f;

	numberCombinationInARow = 0;
	//ne dois pas etre reinit a chaque game :
	l666numberLose = 0;
	lTheyGood = 0;
}

void SuccessManager::NewGame(int difficulty) {
	hardMode = difficulty;

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
			successAPI->successCompleted("6 in a row", 1652152);
			b6InARow=true;
		}
	}
}

void SuccessManager::sHardScore(StorageAPI* storage) {
	if (!bHardScore) {
		if (storage->getSavedGamePointsSum() > 1000000) {
			successAPI->successCompleted("Hardscore gamer", 1653102);
			bHardScore=true;
		}
	}
}
void SuccessManager::sFastAndFinish(float time) {
	if (hardMode && !bFastAndFinish) {
		if (time<=35.f) {
			successAPI->successCompleted("Fast and finish", 1666602);
			bFastAndFinish = true;
		}
	}
}

void SuccessManager::sResetGrid() {
	if (hardMode && !bResetGrid) {
		if (!gridResetted) {
			successAPI->successCompleted("Don't reset the grid !", 1666632);
			bResetGrid = true;
		}
	}
}

void SuccessManager::sTakeYourTime(float time) {
	if (hardMode && !bTakeYourTime) {
		if (time/60 > 15) {
			successAPI->successCompleted("Take your time", 1653092);
			bTakeYourTime = true;
		}
	}
}

void SuccessManager::sExterminaScore(int points) {
	if (hardMode && !bExterminaScore) {
		if (points > 100000) {
			successAPI->successCompleted("Exterminascore", 1653192);
			bExterminaScore = true;
		}
	}
}
void SuccessManager::sLevel1For1K(int level, int points) {
	if (hardMode && !bLevel1For1K) {
		if (level==1 && points>=1000) {
			successAPI->successCompleted("1k points for level 1", 1653122);
			bLevel1For1K = true;
		}
	}
}

void SuccessManager::sLevel10(int level) {
	if (hardMode && !bLevel10) {
		if (level == 10) {
			successAPI->successCompleted("Level 10", 1653112);
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
				successAPI->successCompleted("Rainbow combination", 1653132);
				bRainbow = true;
			} else {
				successAPI->successCompleted("Double rainbow ", 1671892);
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
			successAPI->successCompleted("Bonus to excess", 1653182);
			bBonusToExcess = true;
		}
	}
}

void SuccessManager::sLuckyLuke() {
	if (hardMode && !bLuckyLuke) {
		if (timeUserInputloop<5.f)
			timeTotalPlayed += timeUserInputloop;
		else
			timeTotalPlayed = 0.f;

		if (timeTotalPlayed >= 30.f) {
			successAPI->successCompleted("Lucky Luke", 1671902);
			bLuckyLuke = true;
		}
	}
}

void SuccessManager::sDonator() {
	if (!bDonator) {
		successAPI->successCompleted("Donator", 1671922);
		bDonator = true;
	}
}

void SuccessManager::sTestEverything(StorageAPI* str) {
	if (!bTestEverything) {
		if (str->everyModesPlayed()) {
			successAPI->successCompleted("Test everything", 1684852);
			bTestEverything = true;
		}
	}
}

void SuccessManager::sBTAC(StorageAPI* storage, int difficulty, unsigned int points) {
	if (!bBTAC) {
	    std::vector<StorageAPI::Score> entries = storage->savedScores(1, difficulty);
		int s = entries.size();
		if (s >= 5 && points > entries[0].points) {
			successAPI->successCompleted("Beat them all (classic)", 1684862);
			bBTAC = true;
		}
	}
}

void SuccessManager::sBTAM(StorageAPI* storage, int difficulty, float time) {
	if (!bBTAM) {
		std::vector<StorageAPI::Score> entries = storage->savedScores(2, difficulty);
		int s = entries.size();
		if (s >= 5 && time < entries[0].time) {
			successAPI->successCompleted("Beat them all (MODE2)", 1684872);
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
			successAPI->successCompleted("666 Loser !", 1684892);
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
			successAPI->successCompleted("They're too good", 1684902);
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
			successAPI->successCompleted("What I gonna do ?", 1684912);
			bWhatToDo = true;
		}
	}
}

// Delete::Enter gives newCombiCount
// User::Enter cancel it and gives 0
void SuccessManager::sBimBamBoum(int newCombiCount) {
	if (hardMode && !bBimBamBoum) {
		if (newCombiCount == 0) {
			numberCombinationInARow = 0;
		} else {
			numberCombinationInARow += newCombiCount;
		}

		if (numberCombinationInARow >= 3) {
			successAPI->successCompleted("Bim Bam Boum", 1685032);
			bBimBamBoum = true;
		}
	}
}

void SuccessManager::sDoubleInOne(std::vector<Combinais> &s) {
	if (hardMode && !bDoubleInOne) {
		if (s.size() > 1) {
			successAPI->successCompleted("Double in one", 1685322);
			bDoubleInOne = true;
		}
	}
}
