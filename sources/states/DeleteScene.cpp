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
#include "CombinationMark.h"
#include "HeriswapGame.h"

#include "systems/TwitchSystem.h"
#include "systems/HeriswapGridSystem.h"

#include "modes/GameModeManager.h"

#include "systems/ADSRSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TransformationSystem.h"

#include <vector>

struct DeleteScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    Entity deleteAnimation;
    std::vector<Combinais> removing;
    std::vector<GameModeManager::BranchLeaf> littleLeavesDeleted;

    DeleteScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        deleteAnimation = theEntityManager.CreateEntityFromTemplate("deleteAnimation");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum from) override {
        if (from == Scene::Pause)
            return;
        ADSR(deleteAnimation)->attackTiming = game->datas->timing.deletion;

        littleLeavesDeleted.clear();
        removing = theHeriswapGridSystem.LookForCombination(true,true);
        if (!removing.empty()) {
            game->datas->successMgr->sDoubleInOne(removing);
            game->datas->successMgr->sBimBamBoum(removing.size());
            for ( std::vector<Combinais>::reverse_iterator it = removing.rbegin(); it != removing.rend(); ++it ) {
                for ( std::vector<glm::vec2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV ) {
                    Entity e = theHeriswapGridSystem.GetOnPos(itV->x,itV->y);
                    TwitchComponent* tc = TWITCH(e);
                    if (tc->speed == 0) {
                        CombinationMark::markCellInCombination(e);
                    }
                }
                game->datas->mode2Manager[game->datas->mode]->WillScore(it->points.size(), it->type, littleLeavesDeleted);

                game->datas->successMgr->s6InARow(it->points.size());
            }
            SOUND(deleteAnimation)->sound = theSoundSystem.loadSoundFile("audio/son_monte.ogg");
        }
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float) override {
        ADSRComponent* transitionSuppr = ADSR(deleteAnimation);
        if (!removing.empty()) {
            transitionSuppr->active = true;
            for ( std::vector<Combinais>::reverse_iterator it = removing.rbegin(); it != removing.rend(); ++it ) {
                const glm::vec2 cellSize = HeriswapGame::CellSize(theHeriswapGridSystem.GridSize, it->type) * HeriswapGame::CellContentScale() * (1 - transitionSuppr->value);
                if (transitionSuppr->value == transitionSuppr->sustainValue) {
                    game->datas->mode2Manager[game->datas->mode]->ScoreCalc(it->points.size(), it->type);
                }
                for ( std::vector<glm::vec2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV ) {
                    Entity e = theHeriswapGridSystem.GetOnPos(itV->x,itV->y);
                    //  TRANSFORM(e)->rotation = HeriswapGame::cellTypeToRotation(it->type) + (1 - transitionSuppr->value) * MathUtil::TwoPi;
                    ADSR(e)->idleValue = cellSize.x;
                    if (transitionSuppr->value == transitionSuppr->sustainValue) {
                        if (e)
                            theEntityManager.DeleteEntity(e);
                        littleLeavesDeleted.clear();
                    } else {
                        glm::vec2 size = HeriswapGame::CellSize(theHeriswapGridSystem.GridSize, HERISWAPGRID(e)->type);
                        float scale = ADSR(e)->value / size.x;
                        TRANSFORM(e)->size = size * scale;
                    }
                }
            }
            for (unsigned int i=0; i<littleLeavesDeleted.size(); i++) {
                const glm::vec2 littleLeavesSize = HeriswapGame::CellSize(8, littleLeavesDeleted[i].type) * HeriswapGame::CellContentScale() * (1 - transitionSuppr->value);
                TRANSFORM(littleLeavesDeleted[i].e)->size = littleLeavesSize;
            }
            if (transitionSuppr->value  == transitionSuppr->sustainValue) {
                return Scene::Fall;
            }
        } else {
            return Scene::Spawn;
        }
        return Scene::Delete;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
    }

    void onExit(Scene::Enum to) override {
        if (to == Scene::Pause)
            return;
        ADSR(deleteAnimation)->active = false;
        removing.clear();
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateDeleteSceneHandler(HeriswapGame* game) {
        return new DeleteScene(game);
    }
}
