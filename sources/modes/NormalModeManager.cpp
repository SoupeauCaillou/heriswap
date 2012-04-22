#include "NormalModeManager.h"
#include "Game.h"
#define CAMERASPEED 0.f
#include <base/Vector2.h>
#include <base/MathUtil.h>
#include <vector>
#include "CombinationMark.h"
#include "systems/ScrollingSystem.h"

#define SKY_SPEED 2.3
#define DECOR2_SPEED 1.6
#define DECOR1_SPEED 1

NormalGameModeManager::NormalGameModeManager(Game* game) : GameModeManager(game) {
	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(15,0.125));
	pts.push_back(Vector2(25,0.25));
	pts.push_back(Vector2(35,0.5));
	pts.push_back(Vector2(45,1));
}

NormalGameModeManager::~NormalGameModeManager() {
}

void NormalGameModeManager::Setup() {
	GameModeManager::Setup();
}

void NormalGameModeManager::Enter() {
	limit = 45;
	time = 0;
	points = 0;
	level = 1;
	bonus = MathUtil::RandomInt(8);
	for (int i=0;i<8;i++) {
		remain[i]=3;
	}
	nextHerissonSpeed = 1;
	levelMoveDuration = 0;
	levelUp = levelUpPending = false;
	
	GameModeManager::Enter();
}

void NormalGameModeManager::Exit() {
	GameModeManager::Exit();
}

void NormalGameModeManager::TogglePauseDisplay(bool paused) {
	GameModeManager::TogglePauseDisplay(paused);
}

float NormalGameModeManager::GameUpdate(float dt) {
	if (levelUpPending) {
		RENDERING(herisson)->hide = false;
        LoadHerissonTexture(bonus+1);
		generateLeaves(6);
		levelUpPending = false;
	}
	time += dt;
	LevelUp();
	return time/limit;
}


void NormalGameModeManager::UiUpdate(float dt) {
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

	if (levelMoveDuration > 0) {
		updateHerisson(dt, time, nextHerissonSpeed);
		levelMoveDuration -= dt;
		if (levelMoveDuration <= 0) {
			// stop scrolling
			SCROLLING(decor1er)->speed.X = 0;
		}
	} else {
		updateHerisson(dt, time, 0);
	}
}

static int levelToLeaveToDelete(int nb, int level) {
    return 6*nb/(2+level);
}

static float timeGain(int nb, float time) {
	return MathUtil::Min(time, nb / 4.0f);
}

void NormalGameModeManager::WillScore(int count, int type, std::vector<Entity>& out) {
    int nb = levelToLeaveToDelete(count, level);
    for (int i=0; nb>0 && i<branchLeaves.size(); i++) {
        if ((type+1) == branchLeaves[i].type) {
            CombinationMark::markCellInCombination(branchLeaves[i].e);
            out.push_back(branchLeaves[i].e);
            nb--;
        }
    }

    // move background during delete/spawn sequence (+ fall ?)
    float deleteDuration = 0.3*FAST;
    float spawnDuration = 0.2*FAST;
    // herisson distance
    float currentPos = TRANSFORM(herisson)->position.X;
    float newPos = GameModeManager::position(time - timeGain(count, time));
    // update herisson and decor at the same time.
    levelMoveDuration = deleteDuration + spawnDuration;
    nextHerissonSpeed = (newPos - currentPos) / levelMoveDuration;

    SCROLLING(decor1er)->speed.X = nextHerissonSpeed;
    // SCROLLING(decor2nd)->speed.X = nextHerissonSpeed * DECOR2_SPEED;
    // SCROLLING(sky)->speed.X = nextHerissonSpeed * SKY_SPEED;

}

void NormalGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		points += 10*level*2*nb*nb*nb/6;
	else
		points += 10*level*nb*nb*nb/6;

	remain[type] -= nb;
	deleteLeaves(type+1, levelToLeaveToDelete(nb, level));
	time -= timeGain(nb, time);

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
		LoadHerissonTexture(bonus+1);
		for (int i=0;i<8;i++)
			remain[i] = 2+level;

		// cacher le n'herisson
		RENDERING(herisson)->hide = true;
		// et le positionner
		TRANSFORM(herisson)->position.X = GameModeManager::position(time);
		levelUpPending = true;
	}
}

bool NormalGameModeManager::LeveledUp() {
	bool bid = levelUp;
	levelUp = false;
	return bid;
}

GameMode NormalGameModeManager::GetMode() {
	return Normal;
}

Entity NormalGameModeManager::getSmallLevelEntity() {
    return uiHelper.smallLevel;
}
