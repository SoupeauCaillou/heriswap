#include "ScoreAttackModeManager.h"
#include "Game.h"

ScoreAttackGameModeManager::ScoreAttackGameModeManager(Game* game, SuccessAPI* successAP) : GameModeManager(game, successAP) {
	pts.push_back(Vector2(0,0));

}

ScoreAttackGameModeManager::~ScoreAttackGameModeManager() {
}

void ScoreAttackGameModeManager::Setup() {
	GameModeManager::Setup();
}

void ScoreAttackGameModeManager::Enter() {
	pts.clear();
	pts.push_back(Vector2(0,0));
	if (theGridSystem.GridSize>=8) {
		pts.push_back(Vector2(3000,1));
		limit = 3000;
	} else {
		pts.push_back(Vector2(1000,1));
		limit = 1000;
	}
	time = 0;
	points = 0;
	bonus = MathUtil::RandomInt(theGridSystem.Types);
	GameModeManager::Enter();
}

void ScoreAttackGameModeManager::Exit() {
	GameModeManager::Exit();
}

void ScoreAttackGameModeManager::GameUpdate(float dt) {
	time+=dt;
}
float ScoreAttackGameModeManager::GameProgressPercent() {
	return points/limit;
}

void ScoreAttackGameModeManager::UiUpdate(float dt) {
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
	if (minute)
		a << minute << ':';
	a << std::setw(2) << std::setfill('0') << seconde << '.' << std::setw(1) << tenthsec << " s";
	TEXT_RENDERING(uiHelper.scoreProgress)->text = a.str();
	}

	updateHerisson(dt, points, 0);
}

void ScoreAttackGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		points += 10*2*nb*nb*nb/6;
	else
		points += 10*nb*nb*nb/6;

	GameModeManager::scoreCalcForSuccessETIAR(nb, type);
}

void ScoreAttackGameModeManager::LevelUp() {
}

bool ScoreAttackGameModeManager::LeveledUp() {
	return false;
}

GameMode ScoreAttackGameModeManager::GetMode() {
	return ScoreAttack;
}

void ScoreAttackGameModeManager::TogglePauseDisplay(bool paused) {
 GameModeManager::TogglePauseDisplay(paused);
}
