#include "StaticTimeModeManager.h"
#include "Game.h"

StaticTimeGameModeManager::StaticTimeGameModeManager() {
	limit = 45.0;
	time = 0.;
	points=0;
	bonus = MathUtil::RandomInt(8);

	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(15,0.125));
	pts.push_back(Vector2(25,0.25));
	pts.push_back(Vector2(35,0.5));
	pts.push_back(Vector2(45,1));
}

StaticTimeGameModeManager::~StaticTimeGameModeManager() {
}

void StaticTimeGameModeManager::Setup() {
	SetupCore(bonus);
	HideUI(true);
}

void StaticTimeGameModeManager::Reset() {
	time = 0;
	branchLeaves.clear();
	points = 0;
	bonus = MathUtil::RandomInt(8);
	ResetCore(bonus);
	HideUI(true);
}



float StaticTimeGameModeManager::Update(float dt) {
	time+=dt;
	return (limit - time)/limit;
}

void StaticTimeGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		points += 10*2*nb*nb*nb/6;
	else
		points += 10*nb*nb*nb/6;
}

void StaticTimeGameModeManager::LevelUp() {
}

bool StaticTimeGameModeManager::LeveledUp() {
	return false;
}

void StaticTimeGameModeManager::HideUI(bool toHide) {
	HideUICore(toHide);
	TEXT_RENDERING(uiHelper.smallLevel)->hide = true;
}

void StaticTimeGameModeManager::UpdateUI(float dt) {
	//Score
	{
	std::stringstream a;
	a.precision(0);
	a << std::fixed << points;
	TEXT_RENDERING(uiHelper.scoreProgress)->text = a.str();
	}
	UpdateCore(dt, time);
}

GameMode StaticTimeGameModeManager::GetMode() {
	return StaticTime;
}
