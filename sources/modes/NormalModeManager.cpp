#include "NormalModeManager.h"


NormalGameModeManager::NormalGameModeManager() {
	limit = 5.0;
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

NormalGameModeManager::~NormalGameModeManager() {
}

void NormalGameModeManager::Setup() {

}

bool NormalGameModeManager::Update(float dt) {
	LevelUp();
	return (limit - time <0);
}

void NormalGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		score += 10*level*2*nb*nb*nb/6;
	else
		score += 10*level*nb*nb*nb/6;

	remain[type-1] -= nb;
	time -= 2;
	if (time < 0)
		time = 0;

	if (remain[type-1]<0)
		remain[type-1]=0;
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
		bonus = MathUtil::RandomInt(8)+1;
		for (int i=0;i<8;i++)
			remain[i] = obj[level-1];
	}
}











bool NormalGameModeManager::LeveledUp() {
		bool bid = levelUp;
		levelUp = false;
		return bid;
}

int NormalGameModeManager::GetBonus() {
	return bonus;
}

int NormalGameModeManager::GetRemain(int type) {
	return remain[type];
}

int NormalGameModeManager::GetObj() {
	return obj[level-1];
}

int NormalGameModeManager::GetLevel() {
	return level;
}

void NormalGameModeManager::Reset() {
	time = 0;
	score = 0;

	isReadyToStart = false;
	level = 1;
	bonus = MathUtil::RandomInt(8)+1;

	for (int i=0;i<8;i++) remain[i]=obj[0];

}


