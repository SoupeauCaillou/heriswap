#include "DeleteGameStateManager.h"


DeleteGameStateManager::DeleteGameStateManager() {
	modeMng = 0;
}

DeleteGameStateManager::~DeleteGameStateManager() {
	theEntityManager.DeleteEntity(eRemove);
}

void DeleteGameStateManager::Setup() {
	eRemove = theEntityManager.CreateEntity();

	ADD_COMPONENT(eRemove, ADSR);

	ADSR(eRemove)->idleValue = 0;
	ADSR(eRemove)->attackValue = 1.0;
	ADSR(eRemove)->attackTiming = 0.3*FAST;
	ADSR(eRemove)->decayTiming = 0.;
	ADSR(eRemove)->sustainValue = 1.0;
	ADSR(eRemove)->releaseTiming = 0;
}

void DeleteGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	removing = theGridSystem.LookForCombination(true,true);
}

GameState DeleteGameStateManager::Update(float dt) {
	ADSRComponent* transitionSuppr = ADSR(eRemove);
	if (!removing.empty()) {
		transitionSuppr->active = true;
		for ( std::vector<Combinais>::reverse_iterator it = removing.rbegin(); it != removing.rend(); ++it ) {
			if (transitionSuppr->value == transitionSuppr->sustainValue && modeMng)
				modeMng->ScoreCalc(it->points.size(), it->type);
			for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV ) {
				Entity e = theGridSystem.GetOnPos(itV->X,itV->Y);
				TRANSFORM(e)->rotation = transitionSuppr->value*7;
				if (transitionSuppr->value == transitionSuppr->sustainValue) {
					std::cout << "suppression en ("<<itV->X<<","<<itV->Y<<")\n";
					if (e){
						theEntityManager.DeleteEntity(e);
					}
				}
			}
		}
		if (transitionSuppr->value  == transitionSuppr->sustainValue) {
			theCombinationMarkSystem.DeleteMarks(3);
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
