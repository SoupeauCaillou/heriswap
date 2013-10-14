/*
    This file is part of Heriswap.

    @author Soupe au Caillou - Jordane Pelloux-Prayer
    @author Soupe au Caillou - Gautier Pelloux-Prayer
    @author Soupe au Caillou - Pierre-Eric Pelloux-Prayer

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

#include <sstream>
#include "util/ScoreStorageProxy.h"

SuccessManager::SuccessManager(GameCenterAPI* gAPI) {
	gameCenterAPI = gAPI;

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

	//must be not reinitialized at each game
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
			gameCenterAPI->unlockAchievement(E6InARow);
			b6InARow=true;
		}
	}
}

void SuccessManager::sHardScore(StorageAPI* storageAPI) {
	if (!bHardScore) {
		ScoreStorageProxy ssp;
		if (storageAPI->sum(&ssp, "points")  > 1000000.f) {
			gameCenterAPI->unlockAchievement(EHardScore);
			bHardScore=true;
		}
	}
}
void SuccessManager::sFastAndFinish(float time) {
	if (hardMode && !bFastAndFinish) {
		if (time<=53.f) {
			gameCenterAPI->unlockAchievement(EFastAndFinish);
			bFastAndFinish = true;
		}
	}
}

void SuccessManager::sResetGrid() {
	if (hardMode && !bResetGrid) {
		if (!gridResetted) {
			gameCenterAPI->unlockAchievement(EResetGrid);
			bResetGrid = true;
		}
	}
}

void SuccessManager::sTakeYourTime() {
	if (hardMode && !bTakeYourTime) {
		if (gameDuration > 900.) {
			gameCenterAPI->unlockAchievement(ETakeYourTime);
			bTakeYourTime = true;
		}
	}
	gameDuration = 0.;
}

void SuccessManager::sExterminaScore(int points) {
	if (hardMode && !bExterminaScore) {
		if (points > 100000) {
			gameCenterAPI->unlockAchievement(EExterminaScore);
			bExterminaScore = true;
		}
	}
}
void SuccessManager::sLevel1For2K(int level, int points) {
	if (hardMode && !bLevel1For2K) {
		if (level==1 && points>=2000) {
			gameCenterAPI->unlockAchievement(ELevel1For2K);
			bLevel1For2K = true;
		}
	}
}

void SuccessManager::sLevel10(int level) {
	if (hardMode && !bLevel10) {
		if (level == 10) {
			gameCenterAPI->unlockAchievement(ELevel10);
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
				gameCenterAPI->unlockAchievement(ERainbow);
				bRainbow = true;
			} else {
				gameCenterAPI->unlockAchievement(EDoubleRainbow);
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
			gameCenterAPI->unlockAchievement(EBonusToExcess);
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
			gameCenterAPI->unlockAchievement(ELuckyLuke);
			bLuckyLuke = true;
		}
	}
}

void SuccessManager::sTestEverything(StorageAPI* storageAPI) {
	if (!bTestEverything) {
		ScoreStorageProxy ssp;
		if (storageAPI->count(&ssp, "distinct difficulty, mode") == 9) {
			gameCenterAPI->unlockAchievement(ETestEverything);
			bTestEverything = true;
		}
	}
}

void SuccessManager::sBTAC(StorageAPI* storageAPI, Difficulty difficulty, unsigned int points) {
	if (!bBTAC) {
		std::stringstream ss;
		ss << "where mode = 1 and difficulty = " << difficulty << " order by points desc limit 5";
		ScoreStorageProxy ssp;
		storageAPI->loadEntries(&ssp, "points", ss.str());

		if (ssp._queue.size() == 5 && (int)points > ssp._queue.back().points) {
			gameCenterAPI->unlockAchievement(EBTAC);
			bBTAC = true;
		}
	}
}

void SuccessManager::sBTAM(StorageAPI* storageAPI, Difficulty difficulty, float time) {
	if (!bBTAM) {
		std::stringstream ss;
		ss << "where mode = 2 and difficulty = " << difficulty << " order by time asc limit 5";
		ScoreStorageProxy ssp;
		storageAPI->loadEntries(&ssp, "time", ss.str());

		if (ssp._queue.size() == 5 && time > ssp._queue.back().time) {
			gameCenterAPI->unlockAchievement(EBTAM);
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
			gameCenterAPI->unlockAchievement(E666Loser);
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
			gameCenterAPI->unlockAchievement(ETheyGood);
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
			gameCenterAPI->unlockAchievement(EWhatToDo);
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
			gameCenterAPI->unlockAchievement(EBimBamBoum);
			bBimBamBoum = true;
		}
	}
}

void SuccessManager::sDoubleInOne(std::vector<Combinais> &s) {
	if (hardMode && !bDoubleInOne) {
		if (s.size() > 1) {
			gameCenterAPI->unlockAchievement(EDoubleInOne);
			bDoubleInOne = true;
		}
	}
}
