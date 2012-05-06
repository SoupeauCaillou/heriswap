#include "TilesAttackModeManager.h"
#include "Game.h"

TilesAttackGameModeManager::TilesAttackGameModeManager(Game* game, SuccessAPI* successAP) : GameModeManager(game, successAP) {
	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(100,1));
	limit = 100;
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
	pts.push_back(Vector2(limit,1));

	GameModeManager::Enter();
}

void TilesAttackGameModeManager::Exit() {
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
	a << std::fixed << MathUtil::Max(0.0f, limit - points);
	TEXT_RENDERING(uiHelper.smallLevel)->text = a.str();
	}
	//Temps
	{
	std::stringstream a;
	int timeA = time;
	int minute = timeA/60;
	int seconde= timeA%60;
	int tenthsec = (time - minute * 60 - seconde) * 10;
	if (minute) a << minute << ':';
	a << std::setw(2) << std::setfill('0') << seconde << '.' << std::setw(1) << tenthsec << " s";
	TEXT_RENDERING(uiHelper.scoreProgress)->text = a.str();
	}

	updateHerisson(dt, leavesDone, 0);
}

void TilesAttackGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		points += 10*2*nb*nb*nb/6;
	else
		points += 10*nb*nb*nb/6;

	int totalBranch = theGridSystem.Types*6;
	int breakBranch = totalBranch-20;
	int breakComb = (int)limit-20;

	//si on a pas assez de feuilles sur l'arbre pour faire 20/20, on fait lineaire
	if (breakBranch<0) {
		deleteLeaves(-1, (leavesDone+nb)*totalBranch/(int)limit-leavesDone*totalBranch/(int)limit);
	} else {
		//si il en reste plus de 20
		if (leavesDone<breakComb && leavesDone+nb<=breakComb) {
			//on en supprime de tel sorte à tomber à 20 arbres / 20 combi left
			float p = 1.f*breakBranch/breakComb;
			int number = p*(leavesDone+nb)-p*leavesDone;
			GameModeManager::deleteLeaves(-1, number);
		//si on vient de passer de + de 20 à - de 20
		} else if (leavesDone<breakComb && leavesDone+nb>breakComb) {
			//on supprime pour avoir 20 arbres/ 20 combi left + 1 arbre pour chaque combi en +
			int number = breakBranch - (breakBranch*leavesDone)/breakComb;
			number+= leavesDone+nb-breakComb;
			deleteLeaves(-1, number);
		} else {
			//on en supprime 1 de l'arbre pour 1 de la grille
			deleteLeaves(-1, nb);
		}
	}
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

