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
#include "TilesAttackModeManager.h"

#include <iomanip>
#include <sstream>

#include <base/PlacementHelper.h>
#include <base/MathUtil.h>

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"

#include "DepthLayer.h"
#include "CombinationMark.h"
#include "GridSystem.h"

TilesAttackGameModeManager::TilesAttackGameModeManager(HeriswapGame* game, SuccessManager* successMgr, StorageAPI* sAPI) : GameModeManager(game, successMgr, sAPI) {
}

TilesAttackGameModeManager::~TilesAttackGameModeManager() {
}

void TilesAttackGameModeManager::Setup() {
	GameModeManager::Setup();
}

void TilesAttackGameModeManager::Enter() {
	time = 0;
	leavesDone = 0;
	points = 0;
	bonus = MathUtil::RandomInt(theGridSystem.Types);
	succNoGridReset=false;
	pts.clear();
	pts.push_back(Vector2(0,0));
	if (theGridSystem.sizeToDifficulty() == DifficultyEasy)
		limit = 30;
	else if (theGridSystem.sizeToDifficulty() == DifficultyMedium)
		limit = 100;
	else
		limit = 100;
	pts.push_back(Vector2(limit,1));//need limit leaves to end game

	generateLeaves(0, 8);

	scores = storageAPI->savedScores(Normal, theGridSystem.sizeToDifficulty());
	rank = GameModeManager::getMyRank(time, TilesAttack, scores);;

	TEXT_RENDERING(uiHelper.scoreProgress)->flags &= ~TextRenderingComponent::IsANumberBit;

	GameModeManager::Enter();
}

void TilesAttackGameModeManager::Exit() {
	//if we didn't give up
	if (leavesDone >= limit) {
		successMgr->sFastAndFinish(time);
		successMgr->sResetGrid();
	}
	GameModeManager::Exit();
}

void TilesAttackGameModeManager::GameUpdate(float dt) {
	time+=dt;
}
float TilesAttackGameModeManager::GameProgressPercent() {
	return MathUtil::Min(1.0f*leavesDone/limit, 1.0f);;
}

void TilesAttackGameModeManager::UiUpdate(float dt) {
	//Score
	{
	std::stringstream a;
	a.precision(0);
	if (leavesDone>limit)
		a << std::fixed << (unsigned)0;
	else
		a << std::fixed << limit - leavesDone;

	TEXT_RENDERING(uiHelper.smallLevel)->text = a.str();
	}
	//Temps
	{
	std::stringstream a;

	//~not enable currently
	//~a << rank << ". ";

	int minute = ((int)time)/60;
	int seconde= ((int)time)%60;
	int tenthsec = (time - minute * 60 - seconde) * 10;
	if (minute) a << minute << ':';
	a << std::setw(2) << std::setfill('0') << seconde << '.' << std::setw(1) << tenthsec << " s";
	TEXT_RENDERING(uiHelper.scoreProgress)->text = a.str();
	}

	updateHerisson(dt, leavesDone, 0);

#ifdef DEBUG
	if (_debug) {
		for(int i=0; i<8; i++) {
			std::stringstream text;
			text << countBranchLeavesOfType(i);
			if (i == 7) {
				text << ":" << leavesDone;
			}
			TEXT_RENDERING(debugEntities[2*i+1])->text = text.str();
			TEXT_RENDERING(debugEntities[2*i+1])->hide = false;
			TEXT_RENDERING(debugEntities[2*i+1])->color = Color(0.2, 0.2, 0.2);
		}
	}
#endif
}

int TilesAttackGameModeManager::levelToLeaveToDelete(int leavesMaxSize, int limit, int nb, int leavesDone) {
	int totalBranch = leavesMaxSize; // nb de feuilles total sur l'arbre
	int breakBranch = totalBranch-20;
	int breakComb = limit-20;
	int toDelete=0;
	//si on a pas assez de feuilles sur l'arbre pour faire 20/20, on fait lineaire
	if (breakBranch<0) {
		toDelete = (leavesDone+nb)*totalBranch/(int)limit-leavesDone*totalBranch/(int)limit;
	} else {
		//si il en reste plus de 20
		if (leavesDone<breakComb && leavesDone+nb<=breakComb) {
			//on en supprime de tel sorte à tomber à 20 arbres / 20 combi left
			toDelete = (leavesDone+nb)*breakBranch/breakComb-leavesDone*breakBranch/breakComb;
		} else if (leavesDone<breakComb && leavesDone+nb>breakComb) {
			//on supprime pour avoir 20 arbres/ 20 combi left + 1 arbre pour chaque combi en +
			toDelete = breakBranch - (breakBranch*leavesDone)/breakComb;
			toDelete+= leavesDone+nb-breakComb;
		} else {
			//on en supprime 1 de l'arbre pour 1 de la grille
			toDelete = nb;
		}
	}
	return toDelete;
}

void TilesAttackGameModeManager::ScoreCalc(int nb, unsigned int type) {
	if (type == bonus) {
		points += 10*2*nb*nb*nb/6;
		deleteLeaves(~0b0, levelToLeaveToDelete(6*8, limit, 2*nb, leavesDone));
		leavesDone+=2*nb;
	} else {
		points += 10*nb*nb*nb/6;
		deleteLeaves(~0b0, levelToLeaveToDelete(6*8, limit, nb, leavesDone));
		leavesDone+=nb;
	}
	successMgr->sRainbow(type);

	successMgr->sBonusToExcess(type, bonus, nb);

	//update rank
	rank = GameModeManager::getMyRank((float)points, Normal, scores);

}

void TilesAttackGameModeManager::TogglePauseDisplay(bool paused) {
	GameModeManager::TogglePauseDisplay(paused);
}

void TilesAttackGameModeManager::WillScore(int count, int type, std::vector<Entity>& out) {
    int nb = levelToLeaveToDelete(48, limit, (type == bonus ? count * 2 : count), leavesDone);
    for (unsigned int i=0; nb>0 && i<branchLeaves.size(); i++) {
		CombinationMark::markCellInCombination(branchLeaves[i].e);
        out.push_back(branchLeaves[i].e);
        nb--;
    }
}

int TilesAttackGameModeManager::saveInternalState(uint8_t** out) {
    uint8_t* tmp;
    int parent = GameModeManager::saveInternalState(&tmp);
    int s = sizeof(leavesDone);
    uint8_t* ptr = *out = new uint8_t[parent + s];
    ptr = (uint8_t*) mempcpy(ptr, tmp, parent);
    ptr = (uint8_t*) mempcpy(ptr, &leavesDone, sizeof(leavesDone));

	TRANSFORM(herisson)->position.X = GameModeManager::position(leavesDone);

    delete[] tmp;
    return (parent + s);
}

const uint8_t* TilesAttackGameModeManager::restoreInternalState(const uint8_t* in, int size) {
    in = GameModeManager::restoreInternalState(in, size);
    memcpy(&leavesDone, in, sizeof(leavesDone)); in += sizeof(leavesDone);

    TRANSFORM(herisson)->position.X = GameModeManager::position(leavesDone);

    return in;
}
