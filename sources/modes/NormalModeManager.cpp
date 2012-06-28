/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "NormalModeManager.h"

#include <sstream>

#include <base/Vector2.h>
#include <base/MathUtil.h>
#include <base/PlacementHelper.h>
#include <base/MathUtil.h>

#include "systems/ScrollingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/MusicSystem.h"

#include "CombinationMark.h"
#include "Game.h"
#include "GridSystem.h"

#define SKY_SPEED 2.3
#define DECOR2_SPEED 1.6
#define DECOR1_SPEED 1

NormalGameModeManager::NormalGameModeManager(Game* game, SuccessManager* SuccessMgr) : GameModeManager(game,SuccessMgr) {
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
	bonus = MathUtil::RandomInt(theGridSystem.Types);
	for (int i=0;i<theGridSystem.Types;i++) remain[i]=3;
	nextHerissonSpeed = 1;
	levelMoveDuration = 0;

	generateLeaves(0, theGridSystem.Types);

	GameModeManager::Enter();
}

void NormalGameModeManager::Exit() {
	successMgr->sTakeYourTime();
	successMgr->s666Loser(level);

    MUSIC(stressTrack)->volume = 0;
    ADSR(stressTrack)->active = false;
    ADSR(stressTrack)->value = 0;

	GameModeManager::Exit();
}

void NormalGameModeManager::TogglePauseDisplay(bool paused) {
	GameModeManager::TogglePauseDisplay(paused);
}

void NormalGameModeManager::GameUpdate(float dt) {
#ifdef DEBUG
	// no time update when debug shown
	if (!_debug)
#endif
	time += dt;
	successMgr->gameDuration += dt;
}

float NormalGameModeManager::GameProgressPercent() {
	return MathUtil::Min(1.0f, (float)time/limit);
}


void NormalGameModeManager::UiUpdate(float dt) {
	#define CLOCHETTE_TIME 35.0f
	ADSR(stressTrack)->active = (time > CLOCHETTE_TIME);
    if (ADSR(stressTrack)->active) {
        ADSR(stressTrack)->attackValue = ADSR(stressTrack)->sustainValue = MathUtil::Min((time - CLOCHETTE_TIME) / (limit - CLOCHETTE_TIME), 1.0f);
    }

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
			SCROLLING(decor1er)->speed = 0;
		}
	} else {
		updateHerisson(dt, time, 0);
	}

#ifdef DEBUG
	if (_debug) {
		for(int i=0; i<8; i++) {
			std::stringstream text;
			text << (int)remain[i] << "," << (int)(level+2) << "," <<  countBranchLeavesOfType(i);
			TEXT_RENDERING(debugEntities[2*i+1])->text = text.str();
			TEXT_RENDERING(debugEntities[2*i+1])->hide = false;
			TEXT_RENDERING(debugEntities[2*i+1])->color = Color(0.2, 0.2, 0.2);
		}
	}
#endif
}

#include <cmath>
int NormalGameModeManager::levelToLeaveToDelete(int type, int nb, int initialLeaveCount, int removedLeave, int leftOnBranch) {
	int leftForType = MathUtil::Max(0, initialLeaveCount - (removedLeave + nb));
	if (leftForType <= 3) {
		assert (leftOnBranch >= leftForType);
		return (leftOnBranch - leftForType);
	} else {
		// il y a 3 feuilles à supprimer pour (initialLeaveCount - 3) feuilles dans la grille
		int shouldBeRemoved = floor(3 * (removedLeave + nb) / (float)(initialLeaveCount - 3));
		assert (shouldBeRemoved >= 0 && shouldBeRemoved <= 3);
		int alreadyRmvd = 6 - leftOnBranch;
		return shouldBeRemoved - alreadyRmvd;
	}
}

static float timeGain(int nb, float time) {
	return MathUtil::Min(time, 2.f*nb/theGridSystem.GridSize);
}

void NormalGameModeManager::WillScore(int count, int type, std::vector<Entity>& out) {
    int nb = levelToLeaveToDelete(type, count, level+2, level+2 - remain[type], countBranchLeavesOfType(type));
    for (unsigned int i=0; nb>0 && i<branchLeaves.size(); i++) {
        if (type== branchLeaves[i].type) {
            CombinationMark::markCellInCombination(branchLeaves[i].e);
            out.push_back(branchLeaves[i].e);
            nb--;
        }
    }

    // move background during delete/spawn sequence (+ fall ?)
    float deleteDuration = 0.3;
    float spawnDuration = 0.2;
    // herisson distance
    float currentPos = TRANSFORM(herisson)->position.X;
    float newPos = GameModeManager::position(time - timeGain(count, time));
    // update herisson and decor at the same time.
    levelMoveDuration = deleteDuration + spawnDuration;
    if (theGridSystem.GridSize == 5) levelMoveDuration *= 2;
    nextHerissonSpeed = (newPos - currentPos) / levelMoveDuration;

    SCROLLING(decor1er)->speed = MathUtil::Max(0.0f, -nextHerissonSpeed);
    // SCROLLING(decor2nd)->speed.X = nextHerissonSpeed * DECOR2_SPEED;
    // SCROLLING(sky)->speed.X = nextHerissonSpeed * SKY_SPEED;

}

void NormalGameModeManager::ScoreCalc(int nb, unsigned int type) {
	if (type == bonus)
		points += 10*level*2*nb*nb*nb/6;
	else
		points += 10*level*nb*nb*nb/6;

	deleteLeaves(type, levelToLeaveToDelete(type, nb, level+2, level+2 - remain[type], countBranchLeavesOfType(type)));
	remain[type] -= nb;
	time -= timeGain(nb, time);

	if (remain[type]<0)
		remain[type]=0;

	successMgr->sRainbow(type);

	successMgr->sBonusToExcess(type, bonus, nb);

	successMgr->sExterminaScore(points);

}

bool NormalGameModeManager::LevelUp() {
	int match = 1, i=0;
	while (match && i<theGridSystem.Types) {
		if (remain[i] != 0)	match=0;
		i++;
	}

	//si on a tous les objectifs
	if (match) {
		successMgr->sLevel1For2K(level, points);

		level++;

		successMgr->sLevel10(level);

		time -= MathUtil::Min(20*8.f/theGridSystem.GridSize,time);

		LOGI("Level up to level %d", level);

		for (int i=0;i<theGridSystem.Types;i++)
			remain[i] = 2+level;

		//reput hedgehog on first animation position
		c->ind = 0;
		bonus = MathUtil::RandomInt(theGridSystem.Types);
		LoadHerissonTexture(bonus+1);
		RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[1]);
		SCROLLING(decor1er)->speed = 0;
	}
	return match;
}

GameMode NormalGameModeManager::GetMode() {
	return Normal;
}

Entity NormalGameModeManager::getSmallLevelEntity() {
    return uiHelper.smallLevel;
}

int NormalGameModeManager::saveInternalState(uint8_t** out) {
    uint8_t* tmp;
    int parent = GameModeManager::saveInternalState(&tmp);
    int s = sizeof(level) + sizeof(remain);
    uint8_t* ptr = *out = new uint8_t[parent + s];
    ptr = (uint8_t*) mempcpy(ptr, tmp, parent);
    ptr = (uint8_t*) mempcpy(ptr, &level, sizeof(level));
    ptr = (uint8_t*) mempcpy(ptr, &remain[0], sizeof(remain));

    delete[] tmp;
    return (parent + s);
}

const uint8_t* NormalGameModeManager::restoreInternalState(const uint8_t* in, int size) {
    in = GameModeManager::restoreInternalState(in, size);
    memcpy(&level, in, sizeof(level)); in += sizeof(level);
    memcpy(&remain[0], in, sizeof(remain)); in += sizeof(remain);
    return in;
}
