#include "SuccessManager.h"

/* Proto :
void SuccessManager::sName() {
	if (successEnable) {
		if () {
			bName = true;
		}
	}
}
*/

SuccessManager::SuccessManager(SuccessAPI* sAPI) {
	successAPI = sAPI;
	successEnable=true;

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

	timeLL = 0.f;
	timeLLloop = 0.f;
}

void SuccessManager::NewGame(int difficulty) {
	successEnable = difficulty;

	gridResetted = false;

	bonusPoints = 0;
	for (int i=0; i<8; i++) {
		succEveryTypeInARow[i] = 0;
	}
}

void SuccessManager::s6InARow(int nbInCombi) {
	if (successEnable && !b6InARow) {
		if (nbInCombi >= 6) {
			successAPI->successCompleted("6 in a row", 1652152);
			b6InARow=true;
		}
	}
}

void SuccessManager::sHardScore(StorageAPI* storage) {
	if (successEnable && !bHardScore) {
		if (storage->getSavedGamePointsSum() > 1000000) {
			successAPI->successCompleted("Hardscore gamer", 1653102);
			bHardScore=true;
		}
	}
}
void SuccessManager::sFastAndFinish(float time) {
	if (successEnable && !bFastAndFinish) {
		if (time<=35.f) {
			successAPI->successCompleted("Fast and finish", 1666602);
			bFastAndFinish = true;
		}
	}
}

void SuccessManager::sResetGrid() {
	if (successEnable && !bResetGrid) {
		if (!gridResetted) {
			successAPI->successCompleted("Don't reset the grid !", 1666632);
			bResetGrid = true;
		}
	}
}

void SuccessManager::sTakeYourTime(float time) {
	if (successEnable && !bTakeYourTime) {
		if (time/60 > 15) {
			successAPI->successCompleted("Take your time", 1653092);
			bTakeYourTime = true;
		}
	}
}

void SuccessManager::sExterminaScore(int points) {
	if (successEnable && !bExterminaScore) {
		if (points > 100000) {
			successAPI->successCompleted("Exterminascore", 1653192);
			bExterminaScore = true;
		}
	}
}
void SuccessManager::sLevel1For1K(int level, int points) {
	if (successEnable && !bLevel1For1K) {
		if (level==1 && points>=1000) {
			successAPI->successCompleted("1k points for level 1", 1653122);
			bLevel1For1K = true;
		}
	}
}

void SuccessManager::sLevel10(int level) {
	if (successEnable && !bLevel10) {
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
	if (successEnable && (!bRainbow || !bDoubleRainbow)) {
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
	if (successEnable && !bBonusToExcess) {
		if (type == bonus)
			bonusPoints +=nb;
		if (bonusPoints >= 100) {
			LOGI("azertyuiop	%d\n", bonusPoints);
			successAPI->successCompleted("Bonus to excess", 1653182);
			bBonusToExcess = true;
		}
	}
}

void SuccessManager::sLuckyLuke() {
	if (successEnable && !bLuckyLuke) {
		if (timeLLloop<5.f)
			timeLL += timeLLloop;
		else
			timeLL = 0.f;

		if (timeLL >= 30.f) {
			successAPI->successCompleted("Lucky Luke", 1671902);
			bLuckyLuke = true;
		}
	}
}

void SuccessManager::sDonator() {
	if (successEnable && !bDonator) {
		successAPI->successCompleted("Donator", 1671922);
		bDonator = true;
	}
}
