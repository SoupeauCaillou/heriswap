#include "TilesAttackModeManager.h"
#include "Game.h"

TilesAttackGameModeManager::TilesAttackGameModeManager(Game* game) : GameModeManager(game) {
	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(100,1));
}

TilesAttackGameModeManager::~TilesAttackGameModeManager() {
}

void TilesAttackGameModeManager::Setup() {
	GameModeManager::Setup();
}

void TilesAttackGameModeManager::Enter() {
	limit = 100;
	time = 0;
	leavesDone = 0;
	points = 0;
	bonus = MathUtil::RandomInt(8);
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
	leavesDone+=nb;
	if (leavesDone<80) deleteLeaves(-1, nb/4);
	else deleteLeaves(-1, nb);
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

