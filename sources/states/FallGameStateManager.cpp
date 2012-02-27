#include "FallGameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"
#include "base/EntityManager.h"
#include "Game.h"

FallGameStateManager::FallGameStateManager() {

}

void FallGameStateManager::Setup() {
	eFall = theEntityManager.CreateEntity();
	ADD_COMPONENT(eFall, ADSR);

	ADSR(eFall)->idleValue = 0;
	ADSR(eFall)->attackValue = 0.5;
	ADSR(eFall)->attackTiming = 0.2;
	ADSR(eFall)->decayTiming = 0.2;
	ADSR(eFall)->sustainValue = 1.0;
	ADSR(eFall)->releaseTiming = 0; 
}
	
void FallGameStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	falling = theGridSystem.TileFall();
}

GameState FallGameStateManager::Update(float dt) {
	ADSRComponent* transition = ADSR(eFall);
	if (!falling.empty()) {
		transition->active = true;
		for(std::vector<CellFall>::iterator it=falling.begin(); it!=falling.end(); ++it) {
			const CellFall& f = *it;
			Vector2 targetPos = Game::GridCoordsToPosition(f.x, f.toY);
			Vector2 originPos = Game::GridCoordsToPosition(f.x, f.fromY);
			GRID(f.e)->checkedH = GRID(f.e)->checkedV = false;
			TRANSFORM(f.e)->position = MathUtil::Lerp(originPos, targetPos, transition->value);
			if (transition->value == 1) {
				GRID(f.e)->j = f.toY;
			}
		}
		if (transition->value == 1) {
			std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(false,true);
			if (combinaisons.empty()) return Spawn;
			else return Delete;
		}
	} else {
		std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(false,true);
		if (combinaisons.empty()) return Spawn;
		else return Delete;
	}
	return Fall;
}
	
void FallGameStateManager::Exit() {
	falling.clear();
	ADSR(eFall)->active = false;
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

