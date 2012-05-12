#include "DeleteGameStateManager.h"
#include "TwitchSystem.h"
#include "CombinationMark.h"
#include "systems/SoundSystem.h"

DeleteGameStateManager::DeleteGameStateManager(SuccessAPI* pSuccessAPI) : successAPI(pSuccessAPI) {
	modeMgr=0;
}

DeleteGameStateManager::~DeleteGameStateManager() {
	theEntityManager.DeleteEntity(eRemove);
}

void DeleteGameStateManager::Setup() {
	eRemove = theEntityManager.CreateEntity();

	ADD_COMPONENT(eRemove, ADSR);
	ADSR(eRemove)->idleValue = 0;
	ADSR(eRemove)->attackValue = 1.0;
	ADSR(eRemove)->attackTiming = 0.3;
	ADSR(eRemove)->decayTiming = 0.;
	ADSR(eRemove)->sustainValue = 1.0;
	ADSR(eRemove)->releaseTiming = 0;
	ADD_COMPONENT(eRemove, Sound);
	SOUND(eRemove)->type = SoundComponent::EFFECT;
}

void DeleteGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	removing = theGridSystem.LookForCombination(true,true);
	if (!removing.empty()) {
	    for ( std::vector<Combinais>::reverse_iterator it = removing.rbegin(); it != removing.rend(); ++it ) {
	        for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV ) {
	            Entity e = theGridSystem.GetOnPos(itV->X,itV->Y);
	            TwitchComponent* tc = TWITCH(e);
	            if (tc->speed == 0) {
	                CombinationMark::markCellInCombination(e);
	            }
	        }
	        modeMgr->WillScore(it->points.size(), it->type, littleLeavesDeleted);

	        if (it->points.size() >= 6) {
		        successAPI->successCompleted("6 in a row", 1652152);
	        }
	    }
    	SOUND(eRemove)->sound = theSoundSystem.loadSoundFile("audio/son_monte.ogg", false);
	}
}

GameState DeleteGameStateManager::Update(float dt) {
	ADSRComponent* transitionSuppr = ADSR(eRemove);
	if (!removing.empty()) {
		transitionSuppr->active = true;
        Vector2 cellSize = Game::CellSize(theGridSystem.GridSize) * Game::CellContentScale() * (1 - transitionSuppr->value);
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
        for (int i=0; i<littleLeavesDeleted.size(); i++) {
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
