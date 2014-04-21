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

#include "systems/HeriswapGridSystem.h"

#include "base/EntityManager.h"
#include "base/Log.h"
#include "base/TouchInputManager.h"

#include "systems/ADSRSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TransformationSystem.h"

#include "modes/NormalModeManager.h"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/compatibility.hpp>

struct UserInputScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables

    Entity swapAnimation;
    // datas
    Entity dragged;
    int originI, originJ;
    int swapI, swapJ;
    std::vector<Entity> inCombinationCells;

    // SuccessManager* successMgr;

    Entity currentCell, swappedCell;
    Entity rollback;

    UserInputScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        swapAnimation = theEntityManager.CreateEntity(HASH("swapAnimation", 0x697f9a4f),
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("swap_animation"));
        originI = originJ = -1;
        swapI = swapJ = 0;

        rollback = theEntityManager.CreateEntity(HASH("rollback", 0x8afeeda2),
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("rollback"));
    }

    static bool contains(const std::vector<Combinais>& combi, const HeriswapGridComponent* g) {
        for (unsigned int i=0; i<combi.size(); i++) {
            for (unsigned int j=0; j<combi[i].points.size(); j++) {
                const glm::vec2& p = combi[i].points[j];
                if (p.x == g->i && p.y == g->j)
                    return true;
            }
        }
        return false;
    }

    static Entity cellUnderFinger(const glm::vec2& pos, bool) {
        std::vector<Combinais> combinaisons;// = theHeriswapGridSystem.LookForCombination(false,false);
        const float maxDist = HeriswapGame::CellSize(theHeriswapGridSystem.GridSize, 0).y;

        // 4 nearest
        Entity e = 0;
        float nearestDist = maxDist;
        bool nearestInCombi = 0;

        std::vector<Entity> leaves = theHeriswapGridSystem.RetrieveAllEntityWithComponent();
        for (std::vector<Entity>::iterator it=leaves.begin(); it!=leaves.end(); ++it) {
            float sqdist = glm::distance2(pos, TRANSFORM(*it)->position);
            if (sqdist < maxDist) {
                bool inCombi = contains(combinaisons, HERISWAPGRID(*it));

                if (sqdist < nearestDist || (inCombi && !nearestInCombi)) {
                    nearestDist = sqdist;
                    nearestInCombi = inCombi;
                    e = *it;
                }
            }
        }
        return e;
    }

    static Entity moveToCell(Entity original, const glm::vec2& move,
#if SAC_ANDROID
        float threshold) {
#else
        float) {
#endif

#if SAC_ANDROID
        if (glm::length2(move) < threshold)
            return 0;
#endif

        int i = HERISWAPGRID(original)->i;
        int j = HERISWAPGRID(original)->j;

        if (glm::abs(move.x) > glm::abs(move.y)) {
            if (move.x < 0) {
                return theHeriswapGridSystem.GetOnPos(i-1,j);
            } else {
                return theHeriswapGridSystem.GetOnPos(i+1,j);
            }
        } else {
            if (move.y < 0) {
                return theHeriswapGridSystem.GetOnPos(i,j-1);
            } else {
                return theHeriswapGridSystem.GetOnPos(i,j+1);
            }
        }
    }

    static void exchangeGridCoords(Entity a, Entity b) {
        int iA = HERISWAPGRID(a)->i;
        int jA = HERISWAPGRID(a)->j;
        HERISWAPGRID(a)->i = HERISWAPGRID(b)->i;
        HERISWAPGRID(a)->j = HERISWAPGRID(b)->j;
        HERISWAPGRID(b)->i = iA;
        HERISWAPGRID(b)->j = jA;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum) override {
        auto* adsr = ADSR(swapAnimation);
        adsr->attackTiming =
            adsr->releaseTiming = game->datas->timing.swap;
        adsr->active = false;
        adsr->activationTime = 0;
        originI = originJ = -1;
        dragged = 0;

        currentCell = swappedCell = 0;

        game->datas->successMgr->timeUserInputloop = 0.f;
        game->datas->successMgr->sBimBamBoum(0);
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float dt) override {
        //get the game progress
        const float percentDone = game->datas->mode2Manager[game->datas->mode]->GameProgressPercent();

        //game ended
        if (percentDone >= 1) {
            //show one combination which remain
            if (game->datas->mode != TilesAttack) {
                theHeriswapGridSystem.ShowOneCombination();
            }
            return Scene::EndGame;
        }

        //ne pas changer la grille si fin de niveau/jeu
        if (game->datas->mode == Normal) {
            NormalGameModeManager* m = static_cast<NormalGameModeManager*> (game->datas->mode2Manager[Normal]);
            if (m->LevelUp()) {
                return Scene::LevelChanged;
            }
        }

        game->datas->successMgr->timeUserInputloop += dt;
        game->datas->successMgr->sWhatToDo(theTouchInputManager.wasTouched(0) && theTouchInputManager.isTouched(0), dt);

        if (MORPHING(rollback)->active) {
            return Scene::UserInput;
        }

        if (!currentCell) {
            // beginning of drag
            if (!theTouchInputManager.wasTouched(0) &&
                theTouchInputManager.isTouched(0)) {
                const glm::vec2& pos = theTouchInputManager.getTouchLastPosition(0);
                currentCell = cellUnderFinger(pos, true);

                if (currentCell) {
                    CombinationMark::markCellInCombination(currentCell);
                }
            }
        } else {
            const glm::vec2 posA = HeriswapGame::GridCoordsToPosition(HERISWAPGRID(currentCell)->i, HERISWAPGRID(currentCell)->j,theHeriswapGridSystem.GridSize);
            const glm::vec2& pos = theTouchInputManager.getTouchLastPosition(0);
            // compute move
            glm::vec2 move = pos - posA;

            if (theTouchInputManager.isTouched(0)) {
                // swap cell on axis
                Entity c = moveToCell(currentCell, move, TRANSFORM(currentCell)->size.x * 0.01f);

                // same cell, keep going
                if (c && (!swappedCell || swappedCell == c)) {
                    if (!swappedCell) {
                        CombinationMark::markCellInCombination(c);
                    }
                    swappedCell = c;

                    const glm::vec2 posB = HeriswapGame::GridCoordsToPosition(HERISWAPGRID(swappedCell)->i, HERISWAPGRID(swappedCell)->j,theHeriswapGridSystem.GridSize);
                    float t = glm::min(1.0f, glm::length(move));
                    TRANSFORM(currentCell)->position = glm::lerp(posA, posB, t);
                    TRANSFORM(swappedCell)->position = glm::lerp(posA, posB, 1 - t);
                } else {
                    if (swappedCell) {
                        CombinationMark::clearCellInCombination(swappedCell);
                        // different cell, restore pos
                        TRANSFORM(swappedCell)->position = HeriswapGame::GridCoordsToPosition(HERISWAPGRID(swappedCell)->i, HERISWAPGRID(swappedCell)->j,theHeriswapGridSystem.GridSize);
                    }
                    if (!c) {
                        TRANSFORM(currentCell)->position = posA;
                    } else {
                        CombinationMark::markCellInCombination(c);
                    }
                    swappedCell = c;
                }
            } else {
                // release
                CombinationMark::clearCellInCombination(currentCell);
                if (swappedCell) {
                    CombinationMark::clearCellInCombination(swappedCell);

                    if (glm::length(move) < TRANSFORM(currentCell)->size.x * 0.5) {
                        // restore position
                        TRANSFORM(currentCell)->position = posA;
                        TRANSFORM(swappedCell)->position = HeriswapGame::GridCoordsToPosition(HERISWAPGRID(swappedCell)->i, HERISWAPGRID(swappedCell)->j,theHeriswapGridSystem.GridSize);
                    } else {
                        const glm::vec2 posB = HeriswapGame::GridCoordsToPosition(HERISWAPGRID(swappedCell)->i, HERISWAPGRID(swappedCell)->j,theHeriswapGridSystem.GridSize);

                        int typeA = HERISWAPGRID(currentCell)->type;
                        int typeB = HERISWAPGRID(swappedCell)->type;
                        // exchange types
                        HERISWAPGRID(currentCell)->type = typeB;
                        HERISWAPGRID(swappedCell)->type = typeA;
                        // check combi
                        std::vector<Combinais> combinaisons = theHeriswapGridSystem.LookForCombination(false,false);
                        // restore types
                        HERISWAPGRID(currentCell)->type = typeA;
                        HERISWAPGRID(swappedCell)->type = typeB;

                        if (combinaisons.empty()) {
                            // cancel swap
                            theMorphingSystem.clear(MORPHING(rollback));
                            MORPHING(rollback)->elements.push_back(new TypedMorphElement<glm::vec2>(
                                &TRANSFORM(currentCell)->position, TRANSFORM(currentCell)->position, posA));
                            MORPHING(rollback)->elements.push_back(new TypedMorphElement<glm::vec2>(
                                &TRANSFORM(swappedCell)->position, TRANSFORM(swappedCell)->position, posB));
                            MORPHING(rollback)->active = true;
                            SOUND(swapAnimation)->sound = theSoundSystem.loadSoundFile("audio/son_descend.ogg");
                        } else {
                            HERISWAPGRID(currentCell)->checkedH  = false;
                            HERISWAPGRID(currentCell)->checkedV = false;
                            HERISWAPGRID(swappedCell)->checkedH = false;
                            HERISWAPGRID(swappedCell)->checkedV = false;
                            exchangeGridCoords(currentCell, swappedCell);
                            TRANSFORM(currentCell)->position = posB;
                            TRANSFORM(swappedCell)->position = posA;
                            return Scene::Delete;
                        }
                    }
                } else {
                    TRANSFORM(currentCell)->position = posA;
                }
                swappedCell = currentCell = 0;
            }
        }
        return Scene::UserInput;
    }

    void BackgroundUpdate(float) {
        for(int i=0; i<theHeriswapGridSystem.GridSize; i++) {
            for(int j=0; j<theHeriswapGridSystem.GridSize; j++) {
                Entity e = theHeriswapGridSystem.GetOnPos(i,j);
                if (e) {
                    glm::vec2 size = HeriswapGame::CellSize(theHeriswapGridSystem.GridSize, HERISWAPGRID(e)->type);
                    float scale = ADSR(e)->value / size.x;
                    TRANSFORM(e)->size = size * scale;
                }
            }
        }

        if (ADSR(swapAnimation)->activationTime >= 0 && originI >= 0 && originJ >= 0) {

            glm::vec2 pos1 = HeriswapGame::GridCoordsToPosition(originI, originJ, theHeriswapGridSystem.GridSize);
            glm::vec2 pos2 = HeriswapGame::GridCoordsToPosition(originI + swapI, originJ + swapJ, theHeriswapGridSystem.GridSize);

            glm::vec2 interp1 = glm::lerp(pos1, pos2, ADSR(swapAnimation)->value);
            glm::vec2 interp2 = glm::lerp(pos2, pos1, ADSR(swapAnimation)->value);

            Entity e1 = theHeriswapGridSystem.GetOnPos(originI,originJ);
            Entity e2 = theHeriswapGridSystem.GetOnPos(originI + swapI,originJ + swapJ);

            if (e1)
                TRANSFORM(e1)->position = interp1;
            if (e2)
                TRANSFORM(e2)->position = interp2;
    }
}

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum to) override {
        if (to == Scene::EndGame) {
            // setup fade out to menu
            game->datas->faderHelper.start(Fading::Out, 1.5);
        }
    }

    bool updatePreExit(Scene::Enum to, float dt) {
        if (to == Scene::EndGame) {
            return game->datas->faderHelper.update(dt);
        } else {
            return true;
        }
    }

    void onExit(Scene::Enum nextState) override {
        inCombinationCells.clear();

        //quand c'est plus au joueur de jouer, on supprime les marquages sur les feuilles
        game->toggleShowCombi(false);
        if (game->datas->mode == Normal) {
            std::vector<Entity>& leavesInHelpCombination =
                static_cast<NormalGameModeManager*> (game->datas->mode2Manager[Normal])->leavesInHelpCombination;
            if (!leavesInHelpCombination.empty()) {
                std::vector<Entity> leaves = theHeriswapGridSystem.RetrieveAllEntityWithComponent();
                for ( std::vector<Entity>::reverse_iterator it = leaves.rbegin(); it != leaves.rend(); ++it) {
                    RENDERING(*it)->effectRef = DefaultEffectRef;
                }

                leavesInHelpCombination.clear();
            }
        }

        game->datas->successMgr->sLuckyLuke();
        game->datas->successMgr->sWhatToDo(false, 0.f);

        if (nextState == Scene::Help) {
            game->datas->mode2Manager[game->datas->mode]->showGameDecor(true);
        }
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateUserInputSceneHandler(HeriswapGame* game) {
        return new UserInputScene(game);
    }
}
