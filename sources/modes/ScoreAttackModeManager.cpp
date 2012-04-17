#include "ScoreAttackModeManager.h"
#include "Game.h"

ScoreAttackGameModeManager::ScoreAttackGameModeManager() {
	limit = 3000;
	time = 0.;
	points=0;
	bonus = MathUtil::RandomInt(8);
	distance = 0.f;
	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(300,0.125));
	pts.push_back(Vector2(750,0.25));
	pts.push_back(Vector2(1500,0.5));
	pts.push_back(Vector2(3000,1));
}

ScoreAttackGameModeManager::~ScoreAttackGameModeManager() {
}

void ScoreAttackGameModeManager::Setup() {
	SetupCore();
	HideUI(true);
}



bool ScoreAttackGameModeManager::Update(float dt) {
	//on met à jour le temps si on est dans userinput
	//if (game.state(UserInput)) time += dt;

	//a changer
	time+=dt;
	return (limit - points <0);
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

void ScoreAttackGameModeManager::Reset() {
	time = 0;
	points = 0;
	branchLeaves.clear();
	distance = 0.f;
	bonus = MathUtil::RandomInt(8);
}


void ScoreAttackGameModeManager::HideUI(bool toHide) {
	HideUICore(toHide);
	TEXT_RENDERING(uiHelper.smallLevel)->hide = true;
	TEXT_RENDERING(uiHelper.scoreProgress)->isANumber = false;
}

void ScoreAttackGameModeManager::UpdateUI(float dt) {
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
	//Hérisson
			UpdateCore(dt);

	distance = MathUtil::Lerp(-PlacementHelper::ScreenWidth * 0.5 - TRANSFORM(herisson)->size.X * 0.5,
	PlacementHelper::ScreenWidth * 0.5 + TRANSFORM(herisson)->size.X * 0.5, GameModeManager::position(points, pts)) - TRANSFORM(herisson)->position.X;
	if (distance > 0) {
		float vitesse = 0.1f;
		UpdateCore(dt);
		TRANSFORM(herisson)->position.X += vitesse*dt;
		distance -= vitesse*dt;
	} else distance = 0;
}

GameMode ScoreAttackGameModeManager::GetMode() {
	return ScoreAttack;
}
