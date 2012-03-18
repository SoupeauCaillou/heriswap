#include "ScoreAttackModeManager.h"


ScoreAttackGameModeManager::ScoreAttackGameModeManager() {
	limit = 3000.;
	time = 0.;


	score=0;
	levelUp = false;
	isReadyToStart = false;
	level = 1;
	bonus = MathUtil::RandomInt(8);
	for (int i=0;i<50;i++)
		obj[i]=3+i;

	for (int i=0; i<8;i++)
		remain[i]=obj[0];

}

ScoreAttackGameModeManager::~ScoreAttackGameModeManager() {
}

void ScoreAttackGameModeManager::Setup() {

}

bool ScoreAttackGameModeManager::Update(float dt) {
	LevelUp();
	return (limit - score <0);
}

void ScoreAttackGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		score += 10*level*2*nb*nb*nb/6;
	else
		score += 10*level*nb*nb*nb/6;

	remain[type] -= nb;

	if (remain[type]<0)
		remain[type]=0;
}

void ScoreAttackGameModeManager::LevelUp() {
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
		std::cout << "Level up to level " << level << std::endl;
		bonus = MathUtil::RandomInt(8)+1;
		for (int i=0;i<8;i++)
			remain[i] = obj[level-1];
	}
}











bool ScoreAttackGameModeManager::LeveledUp() {
		bool bid = levelUp;
		levelUp = false;
		return bid;
}

int ScoreAttackGameModeManager::GetBonus() {
	return bonus;
}


int ScoreAttackGameModeManager::GetRemain(int type) {
	return remain[type];
}

int ScoreAttackGameModeManager::GetObj() {
	return obj[level-1];
}

int ScoreAttackGameModeManager::GetLevel() {
	return level;
}

void ScoreAttackGameModeManager::Reset() {
	time = 0;
	score = 0;

	isReadyToStart = false;
	level = 1;
	bonus = MathUtil::RandomInt(8);

	for (int i=0;i<8;i++) remain[i]=obj[0];

}


