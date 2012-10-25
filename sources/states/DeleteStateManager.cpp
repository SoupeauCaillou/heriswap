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
#include "DeleteStateManager.h"

#include "systems/SoundSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

#include "modes/GameModeManager.h"

#include "TwitchSystem.h"
#include "CombinationMark.h"

DeleteGameStateManager::DeleteGameStateManager(SuccessManager* sMgr) : successMgr(sMgr) {
	modeMgr=0;
}

void DeleteGameStateManager::Setup() {
	deleteAnimation = theEntityManager.CreateEntity();
	ADD_COMPONENT(deleteAnimation, ADSR);
	ADD_COMPONENT(deleteAnimation, Sound);

	ADSR(deleteAnimation)->idleValue = 0;
	ADSR(deleteAnimation)->attackValue = 1.0;
	ADSR(deleteAnimation)->decayTiming = 0.;
	ADSR(deleteAnimation)->sustainValue = 1.0;
	ADSR(deleteAnimation)->releaseTiming = 0;
}

void DeleteGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	littleLeavesDeleted.clear();
	removing = theGridSystem.LookForCombination(true,true);
	if (!removing.empty()) {
		successMgr->sDoubleInOne(removing);
		successMgr->sBimBamBoum(removing.size());
	    for ( std::vector<Combinais>::reverse_iterator it = removing.rbegin(); it != removing.rend(); ++it ) {
	        for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV ) {
	            Entity e = theGridSystem.GetOnPos(itV->X,itV->Y);
	            TwitchComponent* tc = TWITCH(e);
	            if (tc->speed == 0) {
	                CombinationMark::markCellInCombination(e);
	            }
	        }
	        modeMgr->WillScore(it->points.size(), it->type, littleLeavesDeleted);

			successMgr->s6InARow(it->points.size());
	    }
    	SOUND(deleteAnimation)->sound = theSoundSystem.loadSoundFile("audio/son_monte.ogg");
	}
}

GameState DeleteGameStateManager::Update(float dt __attribute__((unused))) {
	ADSRComponent* transitionSuppr = ADSR(deleteAnimation);
	if (!removing.empty()) {
		transitionSuppr->active = true;
        for ( std::vector<Combinais>::reverse_iterator it = removing.rbegin(); it != removing.rend(); ++it ) {
    	    const Vector2 cellSize = HeriswapGame::CellSize(theGridSystem.GridSize, it->type) * HeriswapGame::CellContentScale() * (1 - transitionSuppr->value);
        	if (transitionSuppr->value == transitionSuppr->sustainValue) {
    			modeMgr->ScoreCalc(it->points.size(), it->type);
			}
    		for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV ) {
    			Entity e = theGridSystem.GetOnPos(itV->X,itV->Y);
    			//  TRANSFORM(e)->rotation = HeriswapGame::cellTypeToRotation(it->type) + (1 - transitionSuppr->value) * MathUtil::TwoPi;
    			ADSR(e)->idleValue = cellSize.X;
    			if (transitionSuppr->value == transitionSuppr->sustainValue) {
    				if (e)
    					theEntityManager.DeleteEntity(e);
                    littleLeavesDeleted.clear();
    			}
    		}
    	}
    	for (unsigned int i=0; i<littleLeavesDeleted.size(); i++) {
            const Vector2 littleLeavesSize = HeriswapGame::CellSize(8, littleLeavesDeleted[i].type) * HeriswapGame::CellContentScale() * (1 - transitionSuppr->value);
            TRANSFORM(littleLeavesDeleted[i].e)->size = littleLeavesSize;
        }
    	if (transitionSuppr->value  == transitionSuppr->sustainValue) {
    		return Fall;
        }
	} else {
		return Spawn;
	}
	return Delete;
}

void DeleteGameStateManager::Exit() {
	ADSR(deleteAnimation)->active = false;
	removing.clear();
	LOGI("%s", __PRETTY_FUNCTION__);
}
