#include "FallGameStateManager.h"
#include "CombinationMark.h"

FallGameStateManager::FallGameStateManager() {

}

FallGameStateManager::~FallGameStateManager() {
	theEntityManager.DeleteEntity(eFall);
}

void FallGameStateManager::Setup() {
	eFall = theEntityManager.CreateEntity();
	ADD_COMPONENT(eFall, ADSR);

	ADSR(eFall)->idleValue = 0;
	ADSR(eFall)->attackValue = 1.0;
	ADSR(eFall)->attackTiming = .3*FAST;
	ADSR(eFall)->decayTiming = 0;
	ADSR(eFall)->sustainValue = 1.0;
	ADSR(eFall)->releaseTiming = 0;
	ADSR(eFall)->attackMode = Quadratic;
	ADSR(eFall)->decayMode = Quadratic;
	ADSR(eFall)->releaseMode = Quadratic;
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
	std::cout << "Possible combinations : "<< combinaisons.size()<<"\r\n" ;

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
	ADSRComponent* transition = ADSR(eFall);
	if (!falling.empty()) {
		transition->active = true;
		for(std::vector<CellFall>::iterator it=falling.begin(); it!=falling.end(); ++it) {
			const CellFall& f = *it;
			Vector2 targetPos = Game::GridCoordsToPosition(f.x, f.toY);
			Vector2 originPos = Game::GridCoordsToPosition(f.x, f.fromY);
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
	ADSR(eFall)->active = false;

	LOGI("%s", __PRETTY_FUNCTION__);
}
