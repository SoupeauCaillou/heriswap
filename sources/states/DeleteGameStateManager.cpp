#include "DeleteGameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/PlayerSystem.h"
#include "base/EntityManager.h"

DeleteGameStateManager::DeleteGameStateManager() {

}

void DeleteGameStateManager::Setup() {
	eRemove = theEntityManager.CreateEntity();
	theADSRSystem.Add(eRemove);
	ADSR(eRemove)->idleValue = 0;
	ADSR(eRemove)->attackValue = 0.5;
	ADSR(eRemove)->attackTiming = 0.2;
	ADSR(eRemove)->decayTiming = 0.2;
	ADSR(eRemove)->sustainValue = 1.0;
	ADSR(eRemove)->releaseTiming = 0; 
}
	
void DeleteGameStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	removing = theGridSystem.LookForCombinaison(true);
}

GameState DeleteGameStateManager::Update(float dt) {
	ADSRComponent* transitionSuppr = ADSR(eRemove);
	if (!removing.empty()) {
		transitionSuppr->active = true;
		for ( std::vector<Combinais>::reverse_iterator it = removing.rbegin(); it != removing.rend(); ++it ) {
			if (transitionSuppr->value == 1) 
				thePlayerSystem.ScoreCalc(it->points.size(), it->type);
			for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV ) {
				Entity e = theGridSystem.GetOnPos(itV->X,itV->Y);
				TRANSFORM(e)->rotation = transitionSuppr->value*7;
				if (transitionSuppr->value == 1) {
					std::cout << "suppression en ("<<itV->X<<","<<itV->Y<<")\n";
					if (e){
						theRenderingSystem.Delete(e);
						theTransformationSystem.Delete(e);
						theADSRSystem.Delete(e);
						theGridSystem.Delete(e);
					}
				}
			}		
		}
		if (transitionSuppr->value == 1) {
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
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

