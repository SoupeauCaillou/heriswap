#include "TilesAttackModeManager.h"

#include <iomanip>
#include <sstream>

#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"

#include "DepthLayer.h"
#include "base/PlacementHelper.h"
#include "Game.h"
#include "CombinationMark.h"

TilesAttackGameModeManager::TilesAttackGameModeManager(Game* game, SuccessAPI* successAP) : GameModeManager(game, successAP) {
	successAPI = successAP;
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

	pts.clear();
	pts.push_back(Vector2(0,0));
	if (theGridSystem.GridSize>8)
		limit = 250;
	else if (theGridSystem.GridSize == 8)
		limit = 100;
	else
		limit = 30;
	pts.push_back(Vector2(limit,1));//need limit leaves to end game
	
	GameModeManager::Enter();
}

void TilesAttackGameModeManager::Exit() {
	//test succès si fini en -90 sec
	if (theGridSystem.GridSize == 8 && leavesDone >= limit && time<=35.f)
		successAPI->successCompleted("Fast and finish", 1666602);
	//test succès no grid reset
	if (theGridSystem.GridSize == 8 && leavesDone >= limit && succNoGridReset)
		successAPI->successCompleted("Don't reset the grid !", 1666632);	
		
	GameModeManager::Exit();
}

void TilesAttackGameModeManager::GameUpdate(float dt) {
	time+=dt;
}
float TilesAttackGameModeManager::GameProgressPercent() {
	return leavesDone/limit;
}

void TilesAttackGameModeManager::UiUpdate(float dt) {
	//Score
	{
	std::stringstream a;
	a.precision(0);
	a << std::fixed << MathUtil::Max(0.0f, limit - leavesDone);
	TEXT_RENDERING(uiHelper.smallLevel)->text = a.str();
	}
	//Temps
	{
	std::stringstream a;
	int minute = ((int)time)/60;
	int seconde= ((int)time)%60;
	int tenthsec = (time - minute * 60 - seconde) * 10;
	if (minute) a << minute << ':';
	a << std::setw(2) << std::setfill('0') << seconde << '.' << std::setw(1) << tenthsec << " s";
	TEXT_RENDERING(uiHelper.scoreProgress)->text = a.str();
	}

	updateHerisson(dt, leavesDone, 0);
}

static int levelToLeaveToDelete(int gridType, int leavesMaxSize, int limit, int nb, int leavesDone) {
	int totalBranch = gridType*6;
	int breakBranch = leavesMaxSize-20;
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

void TilesAttackGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		points += 10*2*nb*nb*nb/6;
	else
		points += 10*nb*nb*nb/6;

	deleteLeaves(-1, levelToLeaveToDelete(theGridSystem.Types, 48, limit, nb, leavesDone));

	leavesDone+=nb;
	GameModeManager::scoreCalcForSuccessETIAR(nb, type);
}

void TilesAttackGameModeManager::LevelUp() {
}

bool TilesAttackGameModeManager::LeveledUp() {
	return false;
}

GameMode TilesAttackGameModeManager::GetMode() {
	return TilesAttack;
}

void TilesAttackGameModeManager::TogglePauseDisplay(bool paused) {
	GameModeManager::TogglePauseDisplay(paused);
}

void TilesAttackGameModeManager::WillScore(int count, int type, std::vector<Entity>& out) {
    int nb = levelToLeaveToDelete(theGridSystem.Types, 48, limit, 3, leavesDone);
    for (int i=0; nb>0 && i<branchLeaves.size(); i++) {
		CombinationMark::markCellInCombination(branchLeaves[i].e);
        out.push_back(branchLeaves[i].e);
        nb--;
    }
}


