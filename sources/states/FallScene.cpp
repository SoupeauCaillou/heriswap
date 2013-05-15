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

#include "HeriswapGame.h"

#include <base/EntityManager.h>

#include "CombinationMark.h"
#include "GridSystem.h"

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

	FallScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
	    this->game = game;
	}

	void setup() {
		fallAnimation = theEntityManager.CreateEntity("fallAnimation");
		ADD_COMPONENT(fallAnimation, ADSR);

		ADSR(fallAnimation)->idleValue = 0;
		ADSR(fallAnimation)->attackValue = 1.0;
		ADSR(fallAnimation)->decayTiming = 0;
		ADSR(fallAnimation)->sustainValue = 1.0;
		ADSR(fallAnimation)->releaseTiming = 0;
		ADSR(fallAnimation)->attackMode = Quadratic;
		ADSR(fallAnimation)->decayMode = Quadratic;
		ADSR(fallAnimation)->releaseMode = Quadratic;
	}

	///----------------------------------------------------------------------------//
	///--------------------- ENTER SECTION ----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreEnter(Scene::Enum) override {
	}

	void onEnter(Scene::Enum) override {
		LOGI("'" << __PRETTY_FUNCTION__ << "'");
		falling = theGridSystem.TileFall();

		// Creation de la nouvelle grille
		for (std::vector<CellFall>::iterator it=falling.begin(); it!=falling.end(); ++it)
		{
			const CellFall& f = *it;
			GRID(f.e)->j = f.toY;
		}
		// Recherche de combinaison
		std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(false,false);

		// gestion des combinaisons
		if (!combinaisons.empty())
		{
			for ( std::vector<Combinais>::reverse_iterator it = combinaisons.rbegin(); it != combinaisons.rend(); ++it )
			{
				for ( std::vector<glm::vec2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV )
				{
	                Entity e = theGridSystem.GetOnPos(itV->x, itV->y);
	                CombinationMark::markCellInCombination(e);
				}
			}
		}
	    // On remet la grille comme avant
	     for (std::vector<CellFall>::iterator it=falling.begin(); it!=falling.end(); ++it)
	     {
	         const CellFall& f = *it;
	         GRID(f.e)->j = f.fromY;
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
				glm::vec2 targetPos = HeriswapGame::GridCoordsToPosition(f.x, f.toY,theGridSystem.GridSize);
				glm::vec2 originPos = HeriswapGame::GridCoordsToPosition(f.x, f.fromY,theGridSystem.GridSize);
				GRID(f.e)->checkedH = GRID(f.e)->checkedV = false;
				TRANSFORM(f.e)->position = glm::lerp(originPos, targetPos, transition->value);
				if (transition->value == 1.) {
					GRID(f.e)->j = f.toY;
				}
			}
			if (transition->value == 1.) {
				std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(false, true);
				if (combinaisons.empty()) return Scene::Spawn;
				else return Scene::Delete;
			}
		} else {
			std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(false, true);
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

		LOGI("'" << __PRETTY_FUNCTION__ << "'");
	}
};

namespace Scene {
	StateHandler<Scene::Enum>* CreateFallSceneHandler(HeriswapGame* game) {
    	return new FallScene(game);
	}
}