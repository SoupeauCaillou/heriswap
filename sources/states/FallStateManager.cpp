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
#include "FallStateManager.h"

#include <base/EntityManager.h>

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

#include "Game.h"
#include "GridSystem.h"
#include "CombinationMark.h"

FallGameStateManager::FallGameStateManager() {
}

void FallGameStateManager::Setup() {
	fallAnimation = theEntityManager.CreateEntity();
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

void FallGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
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
			for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV )
			{
                Entity e = theGridSystem.GetOnPos(itV->X, itV->Y);
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

GameState FallGameStateManager::Update(float dt) {
	ADSRComponent* transition = ADSR(fallAnimation);
	if (!falling.empty()) {
		transition->active = true;
		for(std::vector<CellFall>::iterator it=falling.begin(); it!=falling.end(); ++it) {
			const CellFall& f = *it;
			Vector2 targetPos = Game::GridCoordsToPosition(f.x, f.toY,theGridSystem.GridSize);
			Vector2 originPos = Game::GridCoordsToPosition(f.x, f.fromY,theGridSystem.GridSize);
			GRID(f.e)->checkedH = GRID(f.e)->checkedV = false;
			TRANSFORM(f.e)->position = MathUtil::Lerp(originPos, targetPos, transition->value);
			if (transition->value == 1.) {
				GRID(f.e)->j = f.toY;
			}
		}
		if (transition->value == 1.) {
			std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(false,true);
			if (combinaisons.empty()) return Spawn;
			else return Delete;
		}
	} else {
		std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(false,true);
		if (combinaisons.empty()) return Spawn;
		else return Delete;
	}
	return Fall;
}

void FallGameStateManager::Exit() {
	falling.clear();
	ADSR(fallAnimation)->active = false;

	LOGI("%s", __PRETTY_FUNCTION__);
}
