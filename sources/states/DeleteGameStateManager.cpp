#include "DeleteGameStateManager.h"

#include "systems/SoundSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

#include "modes/GameModeManager.h"

#include "TwitchSystem.h"
#include "CombinationMark.h"
#include "Game.h"

DeleteGameStateManager::DeleteGameStateManager(SuccessManager* sMgr) : successMgr(sMgr) {
	modeMgr=0;
}

void DeleteGameStateManager::setAnimSpeed() {
	int difficulty = (theGridSystem.GridSize!=8)+1; //1 : normal, 2 : easy

	ADSR(eRemove)->idleValue = 0;
	ADSR(eRemove)->attackValue = 1.0;
	ADSR(eRemove)->attackTiming = difficulty*0.3;
	ADSR(eRemove)->decayTiming = 0.;
	ADSR(eRemove)->sustainValue = 1.0;
	ADSR(eRemove)->releaseTiming = 0;
}

void DeleteGameStateManager::Setup() {
	eRemove = theEntityManager.CreateEntity();
	ADD_COMPONENT(eRemove, ADSR);
	ADD_COMPONENT(eRemove, Sound);

	setAnimSpeed();
}

void DeleteGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

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
    	SOUND(eRemove)->sound = theSoundSystem.loadSoundFile("audio/son_monte.ogg");
	}
}

GameState DeleteGameStateManager::Update(float dt __attribute__((unused))) {
	ADSRComponent* transitionSuppr = ADSR(eRemove);
	if (!removing.empty()) {
		transitionSuppr->active = true;
        Vector2 cellSize = Vector2(Game::CellSize(theGridSystem.GridSize) * Game::CellContentScale() * (1 - transitionSuppr->value));
    	for ( std::vector<Combinais>::reverse_iterator it = removing.rbegin(); it != removing.rend(); ++it ) {
    		if (transitionSuppr->value == transitionSuppr->sustainValue) {
    			modeMgr->ScoreCalc(it->points.size(), it->type);
			}
    		for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV ) {
    			Entity e = theGridSystem.GetOnPos(itV->X,itV->Y);
    			//  TRANSFORM(e)->rotation = Game::cellTypeToRotation(it->type) + (1 - transitionSuppr->value) * MathUtil::TwoPi;
    			ADSR(e)->idleValue = cellSize.X;
    			if (transitionSuppr->value == transitionSuppr->sustainValue) {
    				if (e)
    					theEntityManager.DeleteEntity(e);
                    littleLeavesDeleted.clear();
    			}
    		}
    	}
        for (unsigned int i=0; i<littleLeavesDeleted.size(); i++) {
            TRANSFORM(littleLeavesDeleted[i])->size = cellSize;
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
	ADSR(eRemove)->active = false;
	removing.clear();
	LOGI("%s", __PRETTY_FUNCTION__);
}
