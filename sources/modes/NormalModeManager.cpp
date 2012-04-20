#include "NormalModeManager.h"
#include "Game.h"
#define CAMERASPEED 0.f
#include <base/Vector2.h>
#include <base/MathUtil.h>
#include <vector>
#include "CombinationMark.h"
#include "systems/ScrollingSystem.h"

#define SKY_SPEED 0.3
#define DECOR2_SPEED 0.6
#define DECOR1_SPEED 1

NormalGameModeManager::NormalGameModeManager() {
	limit = 45.0;
	time = 0.;

	points=0;
	levelUp = false;
	level = 1;
	bonus = MathUtil::RandomInt(8);

	for (int i=0; i<8;i++)
		remain[i]=3;


	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(15,0.125));
	pts.push_back(Vector2(25,0.25));
	pts.push_back(Vector2(35,0.5));
	pts.push_back(Vector2(45,1));
}

NormalGameModeManager::~NormalGameModeManager() {
}

void NormalGameModeManager::Setup() {
	SetupCore(bonus);
	HideUI(true);
}

void NormalGameModeManager::Reset() {
	time = 0;
	points = 0;
	level = 1;
	bonus = MathUtil::RandomInt(8);
	branchLeaves.clear();
	for (int i=0;i<8;i++) remain[i]=3;
	ResetCore(bonus);
	HideUI(true);
	nextHerissonSpeed = 1;
	levelMoveDuration = 0;
}

float NormalGameModeManager::Update(float dt) {
	if (RENDERING(herisson)->hide) {
		RENDERING(herisson)->hide = false;
		generateLeaves(6);
	}
	time+=dt;
	LevelUp();
	return (limit - time)/limit;
}

static int levelToLeaveToDelete(int nb, int level) {
    return 6*nb/(2+level);
}

static float timeGain(int nb, float time) {
	return MathUtil::Min(time, nb / 4.0f);
}

void NormalGameModeManager::deleteLeaves(int type, int nb) {
	for (int i=0; nb>0 && i<branchLeaves.size(); i++) {
		if (type == branchLeaves[i].type) {
			theEntityManager.DeleteEntity(branchLeaves[i].e);
			branchLeaves.erase(branchLeaves.begin()+i);
			nb--;
			i--;
		}
	}
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
    float newPos = GameModeManager::position(time - timeGain(count, time), pts);
    // update herisson and decor at the same time.
    levelMoveDuration = deleteDuration + spawnDuration;
    nextHerissonSpeed = (newPos - currentPos) / levelMoveDuration;
    
    SCROLLING(decord1er)->speed.X = nextHerissonSpeed;
    SCROLLING(decor2nd)->speed.X = nextHerissonSpeed * DECOR2_SPEED;
    SCROLLING(sky)->speed.X = nextHerissonSpeed * SKY_SPEED;
    
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
		GameModeManager::LoadHerissonTexture(bonus+1);
		for (int i=0;i<8;i++)
			remain[i] = 2+level;

		// cacher le n'herisson
		RENDERING(herisson)->hide = true;
		// et le positionner
		TRANSFORM(herisson)->position.X = GameModeManager::position(time, pts);;
	}
}

bool NormalGameModeManager::LeveledUp() {
	bool bid = levelUp;
	levelUp = false;
	return bid;
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
	
	if (levelMoveDuration > 0) {
		UpdateCore(dt, time, nextHerissonSpeed);
		levelMoveDuration -= dt;
		
		if (levelMoveDuration <= 0) {
			// stop scrolling
			float s = 0;
			SCROLLING(decord1er)->speed.X = s;
    		SCROLLING(decor2nd)->speed.X = s * DECOR2_SPEED;
    		SCROLLING(sky)->speed.X = s * SKY_SPEED;
		}
		
	} else {
		UpdateCore(dt, time, 0);
	}
}

GameMode NormalGameModeManager::GetMode() {
	return Normal;
}
