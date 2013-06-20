/*
This file is part of RecursiveRunner.

@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
@author Soupe au Caillou - Gautier Pelloux-Prayer

RecursiveRunner is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.

RecursiveRunner is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RecursiveRunner.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "base/StateMachine.h"

#include "Scenes.h"

#include "Game_Private.h"
#include "HeriswapGame.h"

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"
#include "CombinationMark.h"
#include "DepthLayer.h"

#include "GridSystem.h"
#include "TwitchSystem.h"

#include <base/PlacementHelper.h>

#include "systems/ADSRSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/MusicSystem.h"
#include "systems/ParticuleSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TextRenderingSystem.h"
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

    LevelChangedScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        eGrid = theEntityManager.CreateEntity("eGrid",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("grid"));

        eBigLevel = theEntityManager.CreateEntity("eBigLevel",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("big_level"));

        // TEXT_RENDERING(eBigLevel)->charHeight = PlacementHelper::GimpHeightToScreen(350);
        // TRANSFORM(eBigLevel)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(846));
        // TEXT_RENDERING(eBigLevel)->positioning = TextRenderingComponent::CENTER;

        eSnowEmitter = theEntityManager.CreateEntity("eSnowEmitter",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("snow_emiter"));

        // TRANSFORM(eSnowEmitter)->size = glm::vec2((float)PlacementHelper::ScreenWidth, 0.5f);
        // TransformationSystem::setPosition(TRANSFORM(eSnowEmitter),
        //                                   glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(0)),
        //                                   TransformationSystem::S);
        // PARTICULE(eSnowEmitter)->initialSize = Interval<float>(PlacementHelper::GimpWidthToScreen(30),
        //                                                        PlacementHelper::GimpWidthToScreen(40));

        eSnowBranch = theEntityManager.CreateEntity("eSnowBranch",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("snow_branch"));


        // TRANSFORM(eSnowBranch)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(800),
        //                                          (float)PlacementHelper::GimpHeightToScreen(218));
        // TransformationSystem::setPosition(TRANSFORM(eSnowBranch),
        //                                   glm::vec2((float)(-PlacementHelper::ScreenWidth*0.5),
        //                                             (float)PlacementHelper::GimpYToScreen(0)),
        //                                   TransformationSystem::NW);

        eSnowGround = theEntityManager.CreateEntity("eSnowGround",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("snow_ground"));

        // TRANSFORM(eSnowGround)->size = glm::vec2((float)PlacementHelper::ScreenWidth,
        //                                          (float)PlacementHelper::GimpHeightToScreen(300));
        // TransformationSystem::setPosition(TRANSFORM(eSnowGround),
        //                                   glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(1280)),
        //                                   TransformationSystem::S);
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum) override {
        LOGI("'" << __PRETTY_FUNCTION__ << "'");
        Color blue = Color(164.0/255.0, 164.0/255, 164.0/255);


        currentLevel = static_cast<NormalGameModeManager*>(game->datas->mode2Manager[game->datas->mode])->currentLevel();
        smallLevel = static_cast<NormalGameModeManager*>(game->datas->mode2Manager[game->datas->mode])->getSmallLevelEntity();

        levelState = Start;
        std::stringstream a;
        a << currentLevel;
        TEXT_RENDERING(eBigLevel)->text = a.str();
        TEXT_RENDERING(eBigLevel)->show = true;
        TEXT_RENDERING(eBigLevel)->charHeight = PlacementHelper::GimpHeightToScreen(300);
        TRANSFORM(eBigLevel)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(846));
        TRANSFORM(eBigLevel)->z = DL_Score;

        TEXT_RENDERING(eBigLevel)->color = blue;

        MUSIC(eBigLevel)->control = MusicControl::Play;
        MORPHING(eBigLevel)->timing = 1;
        MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&TEXT_RENDERING(eBigLevel)->color.a, 0, 1));
        MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&TEXT_RENDERING(smallLevel)->color.a, 1, 0));
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
        std::vector<Entity> text = theTextRenderingSystem.RetrieveAllEntityWithComponent();
        std::vector<Entity> entities = theRenderingSystem.RetrieveAllEntityWithComponent();
        for (unsigned int i=0; i<entities.size(); i++) {
            TransformationComponent* tc = TRANSFORM(entities[i]);
            {
                RenderingComponent* rc = RENDERING(entities[i]);
                if (rc->texture == branch || rc->texture == pause || rc->texture == sound1 || rc->texture == sound2) {
                    continue;
                }
                rc->effectRef = theRenderingSystem.effectLibrary.load("desaturate.fs");
            }
        }

        entities = theGridSystem.RetrieveAllEntityWithComponent();
        for (unsigned int i=0; i<entities.size(); i++) {
            CombinationMark::markCellInCombination(entities[i]);
        }
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
        std::vector<Entity> entities = theGridSystem.RetrieveAllEntityWithComponent();
        for (std::vector<Entity>::iterator it = entities.begin(); it != entities.end(); ++it ) {
            RENDERING(*it)->color.a = alpha;
            TWITCH(*it)->speed = alpha * 9;
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
            mc->elements.push_back(new TypedMorphElement<float> (&TEXT_RENDERING(eBigLevel)->charHeight,
                                                                  TEXT_RENDERING(eBigLevel)->charHeight,
                                                                  TEXT_RENDERING(smallLevel)->charHeight));
            // mc->elements.push_back(new TypedMorphElement<Color> (&TEXT_RENDERING(eBigLevel)->color, blue, Color(1,1,1,1)));
            mc->elements.push_back(new TypedMorphElement<glm::vec2> (&TRANSFORM(eBigLevel)->position,
                                                                      TRANSFORM(eBigLevel)->position,
                                                                      TRANSFORM(smallLevel)->position));
            mc->active = true;
            mc->activationTime = 0;
            mc->timing = 0.5;

            PARTICULE(eSnowEmitter)->emissionRate = 0;
            //on modifie le herisson
            TRANSFORM(game->datas->mode2Manager[game->datas->mode]->herisson)->position.x =
                game->datas->mode2Manager[game->datas->mode]->position(game->datas->mode2Manager[game->datas->mode]->time);
            RENDERING(game->datas->mode2Manager[game->datas->mode]->herisson)->color.a = 1;
            RENDERING(game->datas->mode2Manager[game->datas->mode]->herisson)->effectRef = DefaultEffectRef;
            //on genere les nouvelles feuilles
            game->datas->mode2Manager[game->datas->mode]->generateLeaves(0, theGridSystem.Types);
            for (unsigned int i=0; i<game->datas->mode2Manager[game->datas->mode]->branchLeaves.size(); i++) {
                TRANSFORM(game->datas->mode2Manager[game->datas->mode]->branchLeaves[i].e)->size =glm::vec2(0.f);
            }
        }
        if (levelState == BigScoreBeganToMove || levelState == BigScoreMoving) {
            levelState = BigScoreMoving;
            //if leaves created, make them grow !
            for (unsigned int i=0; i<game->datas->mode2Manager[game->datas->mode]->branchLeaves.size(); i++) {
                TRANSFORM(game->datas->mode2Manager[game->datas->mode]->branchLeaves[i].e)->size =
                    HeriswapGame::CellSize(8,
                            game->datas->mode2Manager[game->datas->mode]->branchLeaves[i].type) * HeriswapGame::CellContentScale() * glm::min((duration-6) / 4.f, 1.f);
            }
            RENDERING(eSnowBranch)->color.a = 1-(duration-6)/(10-6);
            RENDERING(eSnowGround)->color.a = 1-(duration-6)/(10-6.f);
        }

        duration += dt;

        //level animation ended - back to game
        if (levelState == BigScoreMoving && duration > 10) {
            if (currentLevel == 10 && theGridSystem.sizeToDifficulty() != DifficultyHard) {
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
        theGridSystem.DeleteAll();
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
        TEXT_RENDERING(eBigLevel)->show = false;
        // show small level
        TEXT_RENDERING(smallLevel)->text = TEXT_RENDERING(eBigLevel)->text;
        TEXT_RENDERING(smallLevel)->color.a = 1;
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
