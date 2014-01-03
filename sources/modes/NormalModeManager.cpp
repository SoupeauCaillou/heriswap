/*
    This file is part of Heriswap.

    @author Soupe au Caillou - Jordane Pelloux-Prayer
    @author Soupe au Caillou - Gautier Pelloux-Prayer
    @author Soupe au Caillou - Pierre-Eric Pelloux-Prayer

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

#include "CombinationMark.h"
#include "systems/HeriswapGridSystem.h"

#include "base/PlacementHelper.h"
#include "base/EntityManager.h"

#include "systems/ADSRSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/MusicSystem.h"
#include "systems/ScrollingSystem.h"
#include "systems/TextSystem.h"
#include "systems/TransformationSystem.h"

#include "util/Random.h"

#include <glm/glm.hpp>

#include <sstream>

#define SKY_SPEED 2.3
#define DECOR2_SPEED 1.6
#define DECOR1_SPEED 1

NormalGameModeManager::NormalGameModeManager(HeriswapGame* game, SuccessManager* SuccessMgr, StorageAPI* sAPI) : GameModeManager(game,SuccessMgr, sAPI) {
    pts.push_back(glm::vec2(0.f,0.f));
    pts.push_back(glm::vec2(15.f,0.125f));
    pts.push_back(glm::vec2(25.f,0.25f));
    pts.push_back(glm::vec2(35.f,0.5f));
    pts.push_back(glm::vec2(45.f,1.f));

    helpAvailable = true;
    levelMoveDuration = 0;
}

NormalGameModeManager::~NormalGameModeManager() {
}

void NormalGameModeManager::Setup() {
    GameModeManager::Setup();
}

void NormalGameModeManager::Enter() {
    PROFILE("NormalGameModeManager", "Enter", BeginEvent);
    limit = 45;
    time = 0;
    points = 0;
    level = 1;
    bonus = Random::Int(0, theHeriswapGridSystem.Types-1);
    for (int i=0;i<theHeriswapGridSystem.Types;i++) remain[i]=3;
    nextHerissonSpeed = 1;
    levelMoveDuration = 0;
    helpAvailable = true;
    BUTTON(herisson)->enabled = true;

    generateLeaves(0, theHeriswapGridSystem.Types);

    TEXT(uiHelper.scoreProgress)->flags |= TextComponent::IsANumberBit;
    PROFILE("NormalGameModeManager", "Enter", EndEvent);
    GameModeManager::Enter();
}

void NormalGameModeManager::Exit() {
    successMgr->sTakeYourTime();
    successMgr->s666Loser(level);

    BUTTON(herisson)->enabled = false;

    MUSIC(stressTrack)->volume = 0;
    ADSR(stressTrack)->active = false;
    ADSR(stressTrack)->value = 0;

    GameModeManager::Exit();
}

void NormalGameModeManager::TogglePauseDisplay(bool paused) {
    GameModeManager::TogglePauseDisplay(paused);
}

void NormalGameModeManager::GameUpdate(float dt, Scene::Enum state) {
    #ifdef DEBUG
    // no time update when debug shown
    if (!_debug)
    #endif
    //update UI (pause button, etc)
    if (HeriswapGame::pausableState(state))
        uiHelper.update(dt);

    if (state != Scene::UserInput)
        return;


    time += dt;
    successMgr->gameDuration += dt;

    if (helpAvailable && BUTTON(herisson)->clicked) {
        leavesInHelpCombination = theHeriswapGridSystem.ShowOneCombination();
        helpAvailable = false;
    }
}

float NormalGameModeManager::GameProgressPercent() {
    return glm::min(1.0f, (float)time/limit);
}


void NormalGameModeManager::UiUpdate(float dt) {
    #define CLOCHETTE_TIME 35.0f
    ADSR(stressTrack)->active = (time > CLOCHETTE_TIME);
    if (ADSR(stressTrack)->active) {
        ADSR(stressTrack)->attackValue = ADSR(stressTrack)->sustainValue = glm::min((time - CLOCHETTE_TIME) / (limit - CLOCHETTE_TIME), 1.0f);
    }

    //Score
    {
    std::stringstream a;
    a.precision(0);
    a << std::fixed << points;
    TEXT(uiHelper.scoreProgress)->text = a.str();
    }

    //Level
    {
    std::stringstream a;
    a << level;
    TEXT(uiHelper.smallLevel)->text = a.str();
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

#if SAC_DEBUG
    if (_debug) {
        for(int i=0; i<8; i++) {
            std::stringstream text;
            text << (int)remain[i] << "," << (int)(level+2) << "," <<  countBranchLeavesOfType(i);
            TEXT(debugEntities[2*i+1])->text = text.str();
            TEXT(debugEntities[2*i+1])->show = true;
            TEXT(debugEntities[2*i+1])->color = Color(0.2, 0.2, 0.2);
        }
    }
#endif
}

#include <cmath>
int NormalGameModeManager::levelToLeaveToDelete(int, int nb, int initialLeaveCount, int removedLeave, int leftOnBranch) {
    int leftForType = glm::max(0, initialLeaveCount - (removedLeave + nb));
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

static float timeGain(int nb, int level, float time) {
    return glm::min(time, 2.f*nb/theHeriswapGridSystem.GridSize);
}

void NormalGameModeManager::WillScore(int count, int type, std::vector<BranchLeaf>& out) {
    int nb = levelToLeaveToDelete(type, count, level+2, level+2 - remain[type], countBranchLeavesOfType(type));
    for (unsigned int i=0; nb>0 && i<branchLeaves.size(); i++) {
        if (type == (int)branchLeaves[i].type) {
            CombinationMark::markCellInCombination(branchLeaves[i].e);
            out.push_back(branchLeaves[i]);
            nb--;
        }
    }

    // move background during delete/spawn sequence (+ fall ?)
    float deleteDuration = 0.3;
    float spawnDuration = 0.2;
    // herisson distance
    float currentPos = TRANSFORM(herisson)->position.x;
    float newPos = GameModeManager::position(time - timeGain(count, level, time));
    // update herisson and decor at the same time.
    levelMoveDuration = deleteDuration + spawnDuration;
    if (theHeriswapGridSystem.sizeToDifficulty() != DifficultyHard)
        levelMoveDuration *= 2;

    nextHerissonSpeed = (newPos - currentPos) / levelMoveDuration;

    SCROLLING(decor1er)->speed = glm::max(0.0f, -nextHerissonSpeed);
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
    time -= timeGain(nb, level, time);

    if (remain[type]<0)
        remain[type]=0;

    successMgr->sRainbow(type);

    successMgr->sBonusToExcess(type, bonus, nb);

    successMgr->sExterminaScore(points);
}

void NormalGameModeManager::startLevel(int lvl) {
    level = lvl;

    successMgr->sLevel10(lvl);

    LOGI("New level: '" << lvl << "'");

    for (int i=0;i<theHeriswapGridSystem.Types;i++)
        remain[i] = 2+level;

    if (level < 10)  {
        helpAvailable = true;
    } else {
        helpAvailable = false;
    }

    // put hedgehog back on first animation position
    // c->ind = 0;
    bonus = Random::Int(0, theHeriswapGridSystem.Types-1);
    LoadHerissonTexture(bonus+1);
    // RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[1]);
    SCROLLING(decor1er)->speed = 0;
}

void NormalGameModeManager::changeLevel(int lvl) {
    startLevel(lvl);
    generateLeaves(0, theHeriswapGridSystem.Types);
    uiHelper.game->setupGameProp();
}

bool NormalGameModeManager::LevelUp() {
    int match = 1, i=0;
    while (match && i<theHeriswapGridSystem.Types) {
        if (remain[i] != 0) match=0;
        i++;
    }

    //si on a tous les objectifs
    if (match) {
        successMgr->sLevel1For2K(level, points);

        time -= glm::min(20 * 8.f / theHeriswapGridSystem.GridSize, time);

        PROFILE("NormalGameModeManager", "changeLevel", InstantEvent);

        startLevel(level+1);
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

    TRANSFORM(herisson)->position.x = GameModeManager::position(time);

    delete[] tmp;
    return (parent + s);
}

const uint8_t* NormalGameModeManager::restoreInternalState(const uint8_t* in, int size) {
    in = GameModeManager::restoreInternalState(in, size);
    memcpy(&level, in, sizeof(level)); in += sizeof(level);
    memcpy(&remain[0], in, sizeof(remain)); in += sizeof(remain);

    TRANSFORM(herisson)->position.x = GameModeManager::position(time);
    MUSIC(stressTrack)->volume = 0;
    ADSR(stressTrack)->active = false;
    ADSR(stressTrack)->value = 0;

    return in;
}
