#include "ScoreAttackModeManager.h"
#include "Game.h"

ScoreAttackGameModeManager::ScoreAttackGameModeManager(Game* game) : GameModeManager(game) {
	limit = 3000;
	time = 0.;
	points=0;
	bonus = MathUtil::RandomInt(8);
	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(300,0.125));
	pts.push_back(Vector2(750,0.25));
	pts.push_back(Vector2(1500,0.5));
	pts.push_back(Vector2(3000,1));
}

ScoreAttackGameModeManager::~ScoreAttackGameModeManager() {
}

void ScoreAttackGameModeManager::Setup() {
	GameModeManager::Setup();
}

void ScoreAttackGameModeManager::Enter() {
	limit = 3000;
	time = 0;
	points = 0;
	bonus = MathUtil::RandomInt(8);
	GameModeManager::Enter();
}

void ScoreAttackGameModeManager::Exit() {
	GameModeManager::Exit();
}

float ScoreAttackGameModeManager::GameUpdate(float dt) {
	//on met à jour le temps si on est dans userinput
	//if (game.state(UserInput)) time += dt;

	//a changer
	time+=dt;
	return points/limit;
}

void ScoreAttackGameModeManager::UiUpdate(float dt) {
	//Score
	{
	std::stringstream a;
	a.precision(0);
	a << std::fixed << limit - points;
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
	
	GameModeManager::UiUpdate(dt);
}

void ScoreAttackGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		points += 10*2*nb*nb*nb/6;
	else
		points += 10*nb*nb*nb/6;
}

void ScoreAttackGameModeManager::LevelUp() {
}

bool ScoreAttackGameModeManager::LeveledUp() {
	return false;
}

GameMode ScoreAttackGameModeManager::GetMode() {
	return ScoreAttack;
}
