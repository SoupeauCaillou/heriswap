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

#include "HeriswapGame.h"
#include "Game_Private.h"
#include "CombinationMark.h"

#include "systems/HeriswapGridSystem.h"

#include <base/EntityManager.h>

#include "systems/ADSRSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

struct FallScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    Entity fallAnimation;
    std::vector<CellFall> falling;

    FallScene(HeriswapGame* game) : StateHandler<Scene::Enum>("fall_scene") {
        this->game = game;
    }

    void setup(AssetAPI*) override {
        fallAnimation = theEntityManager.CreateEntityFromTemplate("fallAnimation");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum) override {
        ADSR(fallAnimation)->attackTiming = game->datas->timing.fall;

        falling = theHeriswapGridSystem.TileFall();

        // Creation de la nouvelle grille
        for (std::vector<CellFall>::iterator it=falling.begin(); it!=falling.end(); ++it)
        {
            const CellFall& f = *it;
            HERISWAPGRID(f.e)->j = f.toY;
        }
        // Recherche de combinaison
        std::vector<Combinais> combinaisons = theHeriswapGridSystem.LookForCombination(false,false);

        // gestion des combinaisons
        if (!combinaisons.empty())
        {
            for ( std::vector<Combinais>::reverse_iterator it = combinaisons.rbegin(); it != combinaisons.rend(); ++it )
            {
                for ( std::vector<glm::vec2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV )
                {
                    Entity e = theHeriswapGridSystem.GetOnPos(itV->x, itV->y);
                    CombinationMark::markCellInCombination(e);
                }
            }
        }
        // On remet la grille comme avant
         for (std::vector<CellFall>::iterator it=falling.begin(); it!=falling.end(); ++it)
         {
             const CellFall& f = *it;
             HERISWAPGRID(f.e)->j = f.fromY;
         }
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float) override {
        ADSRComponent* transition = ADSR(fallAnimation);
        if (!falling.empty()) {
            transition->active = true;
            for(std::vector<CellFall>::iterator it=falling.begin(); it!=falling.end(); ++it) {
                const CellFall& f = *it;
                glm::vec2 targetPos = HeriswapGame::GridCoordsToPosition(f.x, f.toY,theHeriswapGridSystem.GridSize);
                glm::vec2 originPos = HeriswapGame::GridCoordsToPosition(f.x, f.fromY,theHeriswapGridSystem.GridSize);
                HERISWAPGRID(f.e)->checkedH = HERISWAPGRID(f.e)->checkedV = false;
                TRANSFORM(f.e)->position = glm::lerp(originPos, targetPos, transition->value);
                if (transition->value == 1.) {
                    HERISWAPGRID(f.e)->j = f.toY;
                }
            }
            if (transition->value == 1.) {
                std::vector<Combinais> combinaisons = theHeriswapGridSystem.LookForCombination(false, true);
                if (combinaisons.empty()) return Scene::Spawn;
                else return Scene::Delete;
            }
        } else {
            std::vector<Combinais> combinaisons = theHeriswapGridSystem.LookForCombination(false, true);
            if (combinaisons.empty()) return Scene::Spawn;
            else return Scene::Delete;
        }
        return Scene::Fall;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
    }

    void onExit(Scene::Enum) override {
        falling.clear();
        ADSR(fallAnimation)->active = false;
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateFallSceneHandler(HeriswapGame* game) {
        return new FallScene(game);
    }
}
