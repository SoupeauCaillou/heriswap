#include "StaticTimeModeManager.h"


StaticTimeGameModeManager::StaticTimeGameModeManager() {
	limit = 60.0;
	time = 0.;


	score=0;
	isReadyToStart = false;

	bonus = MathUtil::RandomInt(8);
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
		score += 10*2*nb*nb*nb/6;
	else
		score += 10*nb*nb*nb/6;
}

int StaticTimeGameModeManager::GetBonus() {
	return bonus;
}

void StaticTimeGameModeManager::Reset() {
	time = 0;
	score = 0;

	isReadyToStart = false;
	bonus = MathUtil::RandomInt(8);
}

		int StaticTimeGameModeManager::GetRemain(int type) { return 0;}
		int StaticTimeGameModeManager::GetObj() { return 0;}
		int StaticTimeGameModeManager::GetLevel() {return 0;}
		void StaticTimeGameModeManager::LevelUp() { ;}
		bool StaticTimeGameModeManager::LeveledUp() {return false;}
