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


#include "GameModeManager.h"

#include "DepthLayer.h"
#include "systems/TwitchSystem.h"
#include "systems/HeriswapGridSystem.h"

#include "base/PlacementHelper.h"
#include "base/EntityManager.h"

#include "systems/AnimationSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ScrollingSystem.h"
#include "systems/System.h"
#include "systems/TextSystem.h"
#include "systems/TransformationSystem.h"

#include "util/Random.h"

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>

static float initialHerissonPosition(Entity herisson) {
    return -PlacementHelper::ScreenSize.x * 0.5f + TRANSFORM(herisson)->size.x * 0.25f;
}

static float finalHerissonPosition(Entity herisson) {
    return PlacementHelper::ScreenSize.x * 0.5f + TRANSFORM(herisson)->size.x * 0.5f;
}

GameModeManager::GameModeManager(HeriswapGame* game, SuccessManager* sMgr, StorageAPI* sAPI) {
    uiHelper.game = game;
    successMgr = sMgr;
    storageAPI = sAPI;
}

float GameModeManager::position(float t) {
    float p = 0;

    if (t <= pts[0].x) {
        p = pts[0].y;
    } else {
        unsigned int i;
        for (i = 0; i<pts.size()-1; i++) {
            if (t>pts[i].x && t<pts[i+1].x) {
                p = glm::lerp(pts[i].y, pts[i+1].y, (t-pts[i].x)/(pts[i+1].x-pts[i].x));
                break;
            }
        }
        if (i == pts.size()-1) {
            p = pts[pts.size()-1].y;
        }
    }
    return glm::lerp(initialHerissonPosition(herisson),
                     finalHerissonPosition(herisson),
                     p);
}

void GameModeManager::LoadHerissonTexture(int type) {
    std::stringstream a;
    LOGW_IF(type > 8, "type > 8");
    LOGW_IF(type < 1, "type < 1");
    type = (type > 8 ? 8 : (type < 1 ? 1 : type));
    a << "herisson_" << type;
    ANIMATION(herisson)->name = a.str();
}

void GameModeManager::Setup() {
    herisson = theEntityManager.CreateEntity("herisson",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("gamemode/herisson"));

    branch = theEntityManager.CreateEntity("branch",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("gamemode/branch"));

    decor2nd = theEntityManager.CreateEntity("decor2nd",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("gamemode/decor2nd"));
    // HACK
    TRANSFORM(decor2nd)->size.x = PlacementHelper::ScreenSize.x;

    SCROLLING(decor2nd)->images.push_back("decor2nd_0");
    SCROLLING(decor2nd)->images.push_back("decor2nd_3");
    SCROLLING(decor2nd)->images.push_back("decor2nd_2");
    SCROLLING(decor2nd)->images.push_back("decor2nd_1");

    decor1er = theEntityManager.CreateEntity("decor1er",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("gamemode/decor1er"));
    TRANSFORM(decor1er)->size.x = PlacementHelper::ScreenSize.x;

    SCROLLING(decor1er)->images.push_back("decor1er_0");
    SCROLLING(decor1er)->images.push_back("decor1er_1");
    SCROLLING(decor1er)->images.push_back("decor1er_2");
    SCROLLING(decor1er)->images.push_back("decor1er_3");

    fillVec();

    uiHelper.build();

    #if SAC_DEBUG
    _debug = false;
    for(int i=0; i<8; i++) {
        debugEntities[2*i] = theEntityManager.CreateEntity("debugEntities");
        ADD_COMPONENT(debugEntities[2*i], Rendering);
        ADD_COMPONENT(debugEntities[2*i], Transformation);
        RENDERING(debugEntities[2*i])->texture = theRenderingSystem.loadTextureFile(HeriswapGame::cellTypeToTextureNameAndRotation(i, 0));
        TRANSFORM(debugEntities[2*i])->z = DL_DebugLayer;
        TRANSFORM(debugEntities[2*i])->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(80));

        debugEntities[2*i + 1] = theEntityManager.CreateEntity("debugEntities");
        ADD_COMPONENT(debugEntities[2*i + 1], Text);
        TEXT(debugEntities[2*i + 1])->positioning = TextComponent::CENTER;
        ADD_COMPONENT(debugEntities[2*i + 1], Transformation);
        TRANSFORM(debugEntities[2*i + 1])->position = TRANSFORM(debugEntities[2*i])->position;
        TRANSFORM(debugEntities[2*i+1])->z = DL_DebugLayer + 0.01f;
        TEXT(debugEntities[2*i + 1])->fontName = "typo";
        TEXT(debugEntities[2*i + 1])->charHeight = PlacementHelper::GimpHeightToScreen(25);
    }
    #endif
}

void GameModeManager::showGameDecor(bool onlyBg) {
    SCROLLING(decor2nd)->show = true;
    SCROLLING(decor1er)->show = true;

    RENDERING(herisson)->show = !onlyBg;
    RENDERING(branch)->show = !onlyBg;
    if (onlyBg)
        uiHelper.hide();
    else
        uiHelper.show();

    // delete leaves
    for (unsigned int az=0;az<branchLeaves.size();az++) {
        RENDERING(branchLeaves[az].e)->show = !onlyBg;
    }
}

void GameModeManager::Enter() {
    PROFILE("GameModeManager", "Enter", BeginEvent);

    RENDERING(herisson)->show = true;
    RENDERING(herisson)->color.a = 1;
    RENDERING(branch)->show = true;
    SCROLLING(decor2nd)->show = true;
    SCROLLING(decor1er)->show = true;
    LoadHerissonTexture(bonus+1);

    uiHelper.show();
    theHeriswapGridSystem.ShowAll(true);
    TRANSFORM(herisson)->position.x = initialHerissonPosition(herisson);
    PROFILE("GameModeManager", "Enter", EndEvent);
}

void GameModeManager::Exit() {
    RENDERING(herisson)->show = false;
    RENDERING(branch)->show = false;
    SCROLLING(decor2nd)->show = false;
    SCROLLING(decor1er)->show = false;

    uiHelper.game->stopInGameMusics();
    uiHelper.hide();

    // delete leaves
    for (unsigned int az=0;az<branchLeaves.size();az++) {
        theEntityManager.DeleteEntity(branchLeaves[az].e);
    }
    branchLeaves.clear();
    theHeriswapGridSystem.DeleteAll();
    PROFILE("GameModeManager", "Exit", InstantEvent);
}

void GameModeManager::TogglePauseDisplay(bool paused) {
    theHeriswapGridSystem.ShowAll(!paused);
    //show levelbig score

    BUTTON(uiHelper.pauseButton)->enabled = !paused;
    RENDERING(uiHelper.pauseButton)->show = !paused;
}

Entity GameModeManager::createAndAddLeave(int type, const glm::vec2& position, float rotation) {
    Entity e = theEntityManager.CreateEntity("leave");
    ADD_COMPONENT(e, Transformation);
    ADD_COMPONENT(e, Rendering);
    ADD_COMPONENT(e, Twitch);
    RENDERING(e)->texture = theRenderingSystem.loadTextureFile(HeriswapGame::cellTypeToTextureNameAndRotation(type, 0));
    RENDERING(e)->show = true;
    RENDERING(e)->opaqueType = RenderingComponent::NON_OPAQUE;

    TRANSFORM(e)->size = HeriswapGame::CellSize(8, type) * HeriswapGame::CellContentScale();

    TRANSFORM(e)->position = position;
    TRANSFORM(e)->rotation = rotation;

    TRANSFORM(e)->z = glm::lerp(DL_LeafMin, DL_LeafMax, glm::linearRand(0.f, 1.f));

    return e;
}

void GameModeManager::generateLeaves(int* nb, int type) {
    for (unsigned int i=0;i<branchLeaves.size();i++)
        theEntityManager.DeleteEntity(branchLeaves[i].e);

    branchLeaves.clear();
    fillVec();

    LOGF_IF(posBranch.empty(), "posBranch isn't initialized before call to generateLeaves");

    for (int j=0;j<type;j++) {
        for (int i=0 ; i < (nb ? nb[j] : 6);i++) {
            // LOGI(j << ',' << i << " -> " << posBranch.size());
            int rnd = Random::Int(0, posBranch.size()-1);
            glm::vec2 pos = posBranch[rnd].v;
            pos.x -= PlacementHelper::GimpXToScreen(0) - -PlacementHelper::ScreenSize.x*0.5f;

            BranchLeaf bl;
            bl.e = createAndAddLeave(j, pos, posBranch[rnd].rot);
            bl.type = j;
            branchLeaves.push_back(bl);

            posBranch.erase(posBranch.begin()+rnd);
        }
    }
    //shuffle pour éviter que les mêmes couleurs soient à coté dans la liste :
    //ça sert en 100 tiles, pour que les feuilles supprimées soient d'abord ttes les
    //rouges, ensuites les jaunes etc..
    std::random_shuffle(branchLeaves.begin(), branchLeaves.end());
}

void GameModeManager::deleteLeaves(unsigned int type, int nb) {
    if (type == ~0u) {
        while (branchLeaves.size()>0 && nb) {
            theEntityManager.DeleteEntity(branchLeaves[0].e);
            branchLeaves.erase(branchLeaves.begin());
            nb--;
        }
    } else {
        for (unsigned int i=0; nb>0 && i<branchLeaves.size(); i++) {
            if (type == branchLeaves[i].type) {
                theEntityManager.DeleteEntity(branchLeaves[i].e);
                branchLeaves.erase(branchLeaves.begin()+i);
                nb--;
                i--;
            }
        }
    }
}

void GameModeManager::fillVec() {
    posBranch.clear();
    #include "PositionFeuilles.h"
    for (int i=0; i<8*6; i++) {
        glm::vec2 v((float)PlacementHelper::GimpXToScreen(pos[3*i]),
                    (float)PlacementHelper::GimpYToScreen(pos[3*i+1]));
        v.x *= PlacementHelper::ScreenSize.x / PlacementHelper::GimpWidthToScreen(800);
        Render truc = {v, glm::radians<float>(pos[3*i+2])};
        posBranch.push_back(truc);
    }
}

void GameModeManager::updateHerisson(float dt, float obj, float herissonSpeed) {
    // default herisson behavior: move to
    TransformationComponent* tc = TRANSFORM(herisson);
    float newPos = tc->position.x;
    if (herissonSpeed == 0) {
        float targetPosX = position(obj);
        float distance = targetPosX - tc->position.x;
        if (distance != 0.f) {
            herissonSpeed = (distance > 0) ? 1.0f : -1.0f;
            if (distance < 0) {
                newPos = glm::max(targetPosX, tc->position.x + herissonSpeed*dt);
            } else {
                newPos = glm::min(targetPosX, tc->position.x + herissonSpeed*dt);
            }
        }
    } else {
        newPos = tc->position.x + herissonSpeed * dt;
    }
    //set animation speed
    float newSpeed = 2.5f;
    if (dt != 0)
        newSpeed = 15*(newPos - tc->position.x)/dt;
    if (newSpeed < 1.4f) newSpeed = 2.5f;
    if (newSpeed > 4.5f) newSpeed = 4.5f;

    ANIMATION(herisson)->playbackSpeed = newSpeed;

    tc->position.x = newPos;

    if (herissonSpeed <= 0) {
        ANIMATION(herisson)->playbackSpeed = 0;
    }
}

int GameModeManager::saveInternalState(uint8_t** out) {
    int s = sizeof(time) + sizeof(limit) + sizeof(points) + sizeof(bonus) + 8 * sizeof(uint8_t);
    uint8_t* ptr = *out = new uint8_t[s];
    ptr = (uint8_t*) mempcpy(ptr, &time, sizeof(time));
    ptr = (uint8_t*) mempcpy(ptr, &limit, sizeof(limit));
    ptr = (uint8_t*) mempcpy(ptr, &points, sizeof(points));
    ptr = (uint8_t*) mempcpy(ptr, &bonus, sizeof(bonus));
    for (int i=0; i<8; i++) {
        uint8_t count = 0;
        for (unsigned int j=0; j<branchLeaves.size(); j++) {
            if (branchLeaves[j].type == (unsigned int)i)
                count++;
        }
        ptr = (uint8_t*) mempcpy(ptr, &count, sizeof(count));
    }
    return s;
}

const uint8_t* GameModeManager::restoreInternalState(const uint8_t* in, int) {
    int index = 0;
    memcpy(&time, &in[index], sizeof(time)); index += sizeof(time);
    memcpy(&limit, &in[index], sizeof(limit)); index += sizeof(limit);
    memcpy(&points, &in[index], sizeof(points)); index += sizeof(points);
    memcpy(&bonus, &in[index], sizeof(bonus)); index += sizeof(bonus);
    int nb[8];
    for (int i=0; i<8; i++) {
        uint8_t c;
        memcpy(&c, &in[index], sizeof(uint8_t)); index += sizeof(uint8_t);
        nb[i] = c;
    }
    generateLeaves(nb,8);
    LoadHerissonTexture(bonus+1);

    /*
    //ici ça depend du mode...
    //mode tiles
    generateLeaves(nb,8);
    //mode normal
    generateLeaves(nb, theHeriswapGridSystem.Types);
    */
    return &in[index];
}

#if SAC_DEBUG
void GameModeManager::toggleDebugDisplay() {
    _debug = !_debug;
    for(int i=0; i<8; i++) {
        RENDERING(debugEntities[2*i])->show = _debug;
        TEXT(debugEntities[2*i+1])->show = _debug;
    }
}
#endif

int GameModeManager::countBranchLeavesOfType(int t) const {
    int count = 0;
    for (unsigned int i=0; i<branchLeaves.size(); i++) {
        count += (t == (int)branchLeaves[i].type);
    }
    return count;
}
