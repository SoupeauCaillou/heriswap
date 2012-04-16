#include "NormalModeManager.h"
#include "Game.h"
#define CAMERASPEED 0.f
#include <base/Vector2.h>
#include <base/MathUtil.h>
#include <vector>

NormalGameModeManager::NormalGameModeManager() {
	limit = 45.0;
	time = 0.;

	points=0;
	levelUp = false;
	level = 1;
	bonus = MathUtil::RandomInt(8);
	for (int i=0;i<50;i++)
		obj[i]=3+i;

	for (int i=0; i<8;i++)
		remain[i]=obj[0];

	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(15,0.125));
	pts.push_back(Vector2(25,0.25));
	pts.push_back(Vector2(35,0.5));
	pts.push_back(Vector2(45,1));
}

NormalGameModeManager::~NormalGameModeManager() {
}

void NormalGameModeManager::Setup() {
	SetupCore();
	HideUI(true);
}



bool NormalGameModeManager::Update(float dt) {
	//on met à jour le temps si on est dans userinput
	//if (game.state(UserInput)) time += dt;

	//a changer
	time+=dt;
	LevelUp();
	return (limit - time <0);
}

void NormalGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		points += 10*level*2*nb*nb*nb/6;
	else
		points += 10*level*nb*nb*nb/6;

	remain[type] -= nb;
	time -= nb/4;
	if (time < 0)
		time = 0;

	if (remain[type]<0)
		remain[type]=0;
}

void NormalGameModeManager::LevelUp() {
	int match = 1, i=0;
	while (match && i<8) {
		if (remain[i] != 0)
			match=0;
		i++;
	}
	//si on a tous les objectifs
	if (match) {
		level++;
		levelUp = true;
		time -= 20;
		if (time < 0)
			time = 0;
		std::cout << "Level up to level " << level << std::endl;
		bonus = MathUtil::RandomInt(8);
		for (int i=0;i<8;i++)
			remain[i] = obj[level-1];
	}
}

bool NormalGameModeManager::LeveledUp() {
	bool bid = levelUp;
	levelUp = false;
	return bid;
}

void NormalGameModeManager::Reset() {
	time = 0;
	points = 0;
	level = 1;
	bonus = MathUtil::RandomInt(8);

	for (int i=0;i<8;i++) remain[i]=obj[0];

}


void NormalGameModeManager::HideUI(bool toHide) {
	HideUICore(toHide);
}

void NormalGameModeManager::UpdateUI(float dt) {
	//Score
	{
	std::stringstream a;
	a.precision(0);
	a << std::fixed << points;
	TEXT_RENDERING(uiHelper.scoreProgress)->text = a.str();
	}
	
	//Level
	{
	std::stringstream a;
	a << level;
	TEXT_RENDERING(uiHelper.smallLevel)->text = a.str();
	}

	//Hérisson
	UpdateCore(dt);
	TRANSFORM(herisson)->position.X = -5.5+11*GameModeManager::position(time, pts);
}

GameMode NormalGameModeManager::GetMode() {
	return Normal;
}
