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


#include "base/StateMachine.h"

#include "Scenes.h"

#include "Game_Private.h"
#include "HeriswapGame.h"
#include "CombinationMark.h"
#include "DepthLayer.h"

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"

#include "systems/HeriswapGridSystem.h"
#include "systems/TwitchSystem.h"

#include "base/PlacementHelper.h"

#include "systems/ADSRSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/MusicSystem.h"
#include "systems/ParticuleSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TextSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

#include <algorithm>
#include <sstream>
#include <vector>

typedef struct FeuilleOrientee {
    Entity e;
    glm::vec2 pos;
    int sens;
} FeuilleOrientee;

struct LevelChangedScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    int currentLevel;
    Entity smallLevel;
    Entity eBigLevel;
    Entity eGrid;
    std::vector<FeuilleOrientee> feuilles;
    Entity eSnowEmitter;
    Entity eSnowBranch;
    Entity eSnowGround;

    float duration;

    enum levelState {
        Start,
        GridHided,
        MusicStarted,
        BigScoreBeganToMove,
        BigScoreMoving
    } levelState;

    LevelChangedScene(HeriswapGame* game) : StateHandler<Scene::Enum>("level_changed_scene") {
        this->game = game;
    }

    void setup(AssetAPI*) override {
        eGrid = theEntityManager.CreateEntityFromTemplate("grid");
        eBigLevel = theEntityManager.CreateEntityFromTemplate("big_level");
        eSnowEmitter = theEntityManager.CreateEntityFromTemplate("snow_emiter");
        eSnowBranch = theEntityManager.CreateEntityFromTemplate("snow_branch");
        eSnowGround = theEntityManager.CreateEntityFromTemplate("snow_ground");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum) override {
        LOGI("'" << __PRETTY_FUNCTION__ << "'");
        Color blue = Color(164.0f / 255.0f, 164.0f / 255.f, 164.0f / 255.f);

        currentLevel = static_cast<NormalGameModeManager*>(game->datas->mode2Manager[game->datas->mode])->currentLevel();
        smallLevel = static_cast<NormalGameModeManager*>(game->datas->mode2Manager[game->datas->mode])->getSmallLevelEntity();

        levelState = Start;
        std::stringstream a;
        a << currentLevel;
        TEXT(eBigLevel)->text = a.str();
        TEXT(eBigLevel)->show = true;
        TEXT(eBigLevel)->charHeight = PlacementHelper::GimpHeightToScreen(300);
        TRANSFORM(eBigLevel)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(846));
        TRANSFORM(eBigLevel)->z = DL_Score;

        TEXT(eBigLevel)->color = blue;

        MUSIC(eBigLevel)->control = MusicControl::Play;
        MORPHING(eBigLevel)->timing = 1;
        MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&TEXT(eBigLevel)->color.a, 0, 1));
        MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&TEXT(smallLevel)->color.a, 1, 0));
        MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&RENDERING(eSnowGround)->color.a, 0, 1));
        MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&RENDERING(eSnowBranch)->color.a, 0, 2));
        MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&RENDERING(game->datas->mode2Manager[game->datas->mode]->herisson)->color.a, 1, 0));
        MORPHING(eBigLevel)->active = true;

        PARTICULE(eSnowEmitter)->emissionRate = 50;
        RENDERING(eSnowBranch)->show = true;
        RENDERING(eSnowGround)->show = true;

        duration = 0;

        // desaturate everyone except the branch, mute, pause and text elements
        TextureRef branch = theRenderingSystem.loadTextureFile("branche");
        TextureRef pause = theRenderingSystem.loadTextureFile("pause");
        TextureRef sound1 = theRenderingSystem.loadTextureFile("sound_on");
        TextureRef sound2 = theRenderingSystem.loadTextureFile("sound_off");
        std::vector<Entity> entities = theRenderingSystem.RetrieveAllEntityWithComponent();
        for (auto e : entities) {
            RenderingComponent* rc = RENDERING(e);
            if (rc->texture == branch || rc->texture == pause || rc->texture == sound1 || rc->texture == sound2) {
                continue;
            }
            rc->effectRef = theRenderingSystem.effectLibrary.load("desaturate.fs");
        }

        entities = theHeriswapGridSystem.RetrieveAllEntityWithComponent();
        for (auto e: entities) {
            CombinationMark::markCellInCombination(e);
        }

        game->stopInGameMusics();
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float dt) override {
        //set grid alpha to 0
        if (levelState == Start && duration > 0.15) {
            ADSR(eGrid)->active = true;
            levelState = GridHided;
        }

        float alpha = 1 - ADSR(eGrid)->value;
        std::vector<Entity> entities = theHeriswapGridSystem.RetrieveAllEntityWithComponent();
        for (auto e : entities) {
            RENDERING(e)->color.a = alpha;
            TWITCH(e)->speed = alpha * 9;
        }

        //start music at 0.5 s
        if (levelState == GridHided && duration > 0.5) {
            levelState = MusicStarted;
            if (MUSIC(eBigLevel)->music == InvalidMusicRef && !theMusicSystem.isMuted())
                MUSIC(eBigLevel)->music = theMusicSystem.loadMusicFile("audio/level_up.ogg");
        }
        //move big score + hedgehog
        //generate new leaves
        if (levelState == MusicStarted && duration > 6) {
            levelState = BigScoreBeganToMove;
            MorphingComponent* mc = MORPHING(eBigLevel);
            for (unsigned int i=0; i<mc->elements.size(); i++) {
                delete mc->elements[i];
            }
            mc->elements.clear();
            // move big score to small score
            //Color blue = Color(164.0/255.0, 164.0/255, 164.0/255);
            mc->elements.push_back(new TypedMorphElement<float> (&TEXT(eBigLevel)->charHeight,
                                                                  TEXT(eBigLevel)->charHeight,
                                                                  TEXT(smallLevel)->charHeight));
            // mc->elements.push_back(new TypedMorphElement<Color> (&TEXT(eBigLevel)->color, blue, Color(1,1,1,1)));
            mc->elements.push_back(new TypedMorphElement<glm::vec2> (&TRANSFORM(eBigLevel)->position,
                                                                      TRANSFORM(eBigLevel)->position,
                                                                      TRANSFORM(smallLevel)->position));
            mc->active = true;
            mc->activationTime = 0;
            mc->timing = 0.5;

            PARTICULE(eSnowEmitter)->emissionRate = 0;
            //modifying the hedgehog
            TRANSFORM(game->datas->mode2Manager[game->datas->mode]->herisson)->position.x =
                game->datas->mode2Manager[game->datas->mode]->position(
                    game->datas->mode2Manager[game->datas->mode]->time / game->datas->mode2Manager[game->datas->mode]->limit);
            RENDERING(game->datas->mode2Manager[game->datas->mode]->herisson)->color.a = 1;
            RENDERING(game->datas->mode2Manager[game->datas->mode]->herisson)->effectRef = DefaultEffectRef;
            // generating the brand-new leaves
            game->datas->mode2Manager[game->datas->mode]->generateLeaves(0, theHeriswapGridSystem.Types);
            for (auto s : game->datas->mode2Manager[game->datas->mode]->branchLeaves) {
                TRANSFORM(s.e)->size = glm::vec2(0.f);
            }
        }
        if (levelState == BigScoreBeganToMove || levelState == BigScoreMoving) {
            levelState = BigScoreMoving;
            //if leaves created, make them grow!
            for (auto s : game->datas->mode2Manager[game->datas->mode]->branchLeaves) {
                TRANSFORM(s.e)->size =
                    HeriswapGame::CellSize(8,
                            s.type) * HeriswapGame::CellContentScale() * glm::min((duration-6) / 4.f, 1.f);
            }
            RENDERING(eSnowBranch)->color.a = 1-(duration-6)/(10-6);
            RENDERING(eSnowGround)->color.a = 1-(duration-6)/(10-6.f);
        }

        duration += dt;

        //level animation ended - back to game
        if (levelState == BigScoreMoving && duration > 10) {
            theHeriswapGridSystem.DeleteAll();
            if (currentLevel == 10 && theHeriswapGridSystem.sizeToDifficulty() != DifficultyHard) {
                return Scene::ElitePopup;
            }
            return Scene::Spawn;
        }

        return Scene::LevelChanged;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
    }

    void onExit(Scene::Enum) override {
        ADSR(eGrid)->active = false;
        feuilles.clear();
        LOGI("'" << __PRETTY_FUNCTION__ << "'");
        PARTICULE(eSnowEmitter)->emissionRate = 0;
        RENDERING(eSnowBranch)->show = false;
        RENDERING(eSnowGround)->show = false;

        MorphingComponent* mc = MORPHING(eBigLevel);
        for (unsigned int i=0; i<mc->elements.size(); i++) {
            delete mc->elements[i];
        }
        for (unsigned int i=0; i<game->datas->mode2Manager[game->datas->mode]->branchLeaves.size(); i++) {
            TRANSFORM(game->datas->mode2Manager[game->datas->mode]->branchLeaves[i].e)->size =
                HeriswapGame::CellSize(8, game->datas->mode2Manager[game->datas->mode]->branchLeaves[i].type) * HeriswapGame::CellContentScale();
        }
        mc->elements.clear();
        // hide big level
        TEXT(eBigLevel)->show = false;
        // show small level
        TEXT(smallLevel)->text = TEXT(eBigLevel)->text;
        TEXT(smallLevel)->color.a = 1;
        RENDERING(game->datas->mode2Manager[game->datas->mode]->herisson)->color.a = 1;

        std::vector<Entity> ent = theRenderingSystem.RetrieveAllEntityWithComponent();
        for (unsigned int i=0; i<ent.size(); i++) {
            RENDERING(ent[i])->effectRef = DefaultEffectRef;
        }
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateLevelChangedSceneHandler(HeriswapGame* game) {
        return new LevelChangedScene(game);
    }
}
