#include "ScoreAttackModeManager.h"
#include "Game.h"

ScoreAttackGameModeManager::ScoreAttackGameModeManager(Game* game,SuccessAPI* successAP) : GameModeManager(game), successAPI(successAP) {
	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(100,1));
}

ScoreAttackGameModeManager::~ScoreAttackGameModeManager() {
}

void ScoreAttackGameModeManager::Setup() {
	GameModeManager::Setup();
}

void ScoreAttackGameModeManager::Enter() {
	limit = 3000;
	for (int i=0; i<8; i++) successType[i] = 0;
	time = 0;
	points = 0;
	bonus = MathUtil::RandomInt(8);
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

	// test succes
	if (successType[type]) {
		for (int i=0; i<8; i++) successType[i] = 0;
	} else {
		 successType[type] = 1;
	}
	if (successDone(successType)) {
		successAPI->successCompleted("Rainbow combination ", 1653132);
	}
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
