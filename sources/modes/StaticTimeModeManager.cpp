#include "StaticTimeModeManager.h"
#include "Game.h"

StaticTimeGameModeManager::StaticTimeGameModeManager(Game* game, SuccessAPI* successAP) : GameModeManager(game, successAP) {
	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(15,0.125));
	pts.push_back(Vector2(25,0.25));
	pts.push_back(Vector2(35,0.5));
	pts.push_back(Vector2(45,1));
}

StaticTimeGameModeManager::~StaticTimeGameModeManager() {
}

void StaticTimeGameModeManager::Setup() {
	GameModeManager::Setup();
}

void StaticTimeGameModeManager::Enter() {
	limit = 45;
	time = 0;
	points = 0;
	bonus = MathUtil::RandomInt(theGridSystem.Types);
	GameModeManager::Enter();
}

void StaticTimeGameModeManager::Exit() {
	GameModeManager::Exit();
}

void StaticTimeGameModeManager::GameUpdate(float dt) {
	time+=dt;
}

float StaticTimeGameModeManager::GameProgressPercent() {
	return time/limit;
}

void StaticTimeGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		points += 10*2*nb*nb*nb/6;
	else
		points += 10*nb*nb*nb/6;

	GameModeManager::scoreCalcForSuccessETIAR(nb, type);

}

void StaticTimeGameModeManager::LevelUp() {
}

bool StaticTimeGameModeManager::LeveledUp() {
	return false;
}

void StaticTimeGameModeManager::UiUpdate(float dt) {
	//Score
	{
	std::stringstream a;
	a.precision(0);
	a << std::fixed << points;
	TEXT_RENDERING(uiHelper.scoreProgress)->text = a.str();
	}
	updateHerisson(dt, time, 0);
}

GameMode StaticTimeGameModeManager::GetMode() {
	return StaticTime;
}

void StaticTimeGameModeManager::TogglePauseDisplay(bool paused) {
 GameModeManager::TogglePauseDisplay(paused);
}
