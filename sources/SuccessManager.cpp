#include "SuccessManager.h"
#include "Callback.cpp"
/* Proto :
void SuccessManager::sRainbow(int level) {	
	if (successEnable) {		
		if (
			bRainbow = true;
		}
	}
}
*/

SuccessManager::SuccessManager(SuccessAPI* sAPI) {
	successAPI = sAPI;
	successEnable=true;
	b6InARow=false;
	bHardScore=false;
}

void SuccessManager::NewGame() {
	bonusPoints = 0;
	for (int i=0; i<8; i++) {
		succEveryTypeInARow[i] = 0;
	}
}

void SuccessManager::s6InARow(int nbInCombi) {
	if (successEnable) {
		if (nbInCombi >= 6) {
			successAPI->successCompleted("6 in a row", 1652152);
			b6InARow=true;
		}
	}
}
	
void SuccessManager::sHardScore(ScoreStorage* storage) {	
	if (successEnable) {
		int sav=0;
		storage->request("select points from score", &sav, callbackSc);
		if (sav > 1000000) {
			successAPI->successCompleted("Hardscore gamer", 1653102);
			bHardScore=true;
		}
	}
}
void SuccessManager::sFastAndFinish(int leavesDone, int limit, float time) {
	if (successEnable) {
		if (leavesDone >= limit && time<=35.f) {
			successAPI->successCompleted("Fast and finish", 1666602);
			bFastAndFinish = true;
		}
	}
}

void SuccessManager::sResetGrid(int leavesDone, int limit) {	
	if (successEnable) {
		if (leavesDone >= limit) {
			successAPI->successCompleted("Don't reset the grid !", 1666632);	
			bResetGrid = true;
		}
	}	
}

void SuccessManager::sTakeYourTime(float time) {	
	if (successEnable) {
		if (time*60 > 15) {
			successAPI->successCompleted("Take your time", 1652152);
			bTakeYourTime = true;
		}
	}
}

void SuccessManager::sExterminaScore(int points) {		
	if (successEnable) {
		if (points > 100000) {
			successAPI->successCompleted("Exterminascore", 1653192);
			bExterminaScore = true;
		}
	}
}
void SuccessManager::sLevel1For1K(int level, int points) {	
	if (successEnable) {
		if (level==1 && points>=1000) {
			successAPI->successCompleted("1k points for level 1", 1653122);
			bLevel1For1K = true;
		}
	}
}
	
void SuccessManager::sLevel10(int level) {	
	if (successEnable) {		
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
	if (successEnable) {		
		if (succEveryTypeInARow[type]) {
			for (int i=0; i<8; i++) succEveryTypeInARow[i] = 0;
		} else {
			succEveryTypeInARow[type] = 1;
		}
		if (rainbow(succEveryTypeInARow)) {
			successAPI->successCompleted("Rainbow combination", 1653132);
			bRainbow = true;
		}
	}
}
void SuccessManager::sBonusToExcess(int type, int bonus, int nb) {	
	if (successEnable) {		
		if (type == bonus)
			bonusPoints +=nb;
		if (bonusPoints >= 100) {
			successAPI->successCompleted("Bonus to excess", 1653182);
			bBonusToExcess = true;
		}
	}
}
