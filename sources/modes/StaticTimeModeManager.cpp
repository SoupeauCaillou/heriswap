#include "StaticTimeModeManager.h"


StaticTimeGameModeManager::StaticTimeGameModeManager() {
	limit = 60.0;
	time = 0.;


	score=0;
	levelUp = false;
	isReadyToStart = false;
	level = 1;
	bonus = MathUtil::RandomInt(8)+1;
	for (int i=0;i<50;i++)
		obj[i]=3+i;

	for (int i=0; i<8;i++)
		remain[i]=obj[0];

}

StaticTimeGameModeManager::~StaticTimeGameModeManager() {
}

void StaticTimeGameModeManager::Setup() {

}

bool StaticTimeGameModeManager::Update(float dt) {
	LevelUp();
	return (limit - time <0);
}

void StaticTimeGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		score += 10*level*2*nb*nb*nb/6;
	else
		score += 10*level*nb*nb*nb/6;

	remain[type-1] -= nb;

	if (remain[type-1]<0)
		remain[type-1]=0;
}

void StaticTimeGameModeManager::LevelUp() {
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











bool StaticTimeGameModeManager::LeveledUp() {
		bool bid = levelUp;
		levelUp = false;
		return bid;
}

int StaticTimeGameModeManager::GetBonus() {
	return bonus;
}

int StaticTimeGameModeManager::GetRemain(int type) {
	return remain[type];
}

int StaticTimeGameModeManager::GetObj() {
	return obj[level-1];
}

int StaticTimeGameModeManager::GetLevel() {
	return level;
}

void StaticTimeGameModeManager::Reset() {
	time = 0;
	score = 0;

	isReadyToStart = false;
	level = 1;
	bonus = MathUtil::RandomInt(8)+1;

	for (int i=0;i<8;i++) remain[i]=obj[0];

}


