#include "FallGameStateManager.h"

FallGameStateManager::FallGameStateManager() {

}

FallGameStateManager::~FallGameStateManager() {
	theEntityManager.DeleteEntity(eFall);
}

void FallGameStateManager::Setup() {
	eFall = theEntityManager.CreateEntity();
	ADD_COMPONENT(eFall, ADSR);

	ADSR(eFall)->idleValue = 0;
	ADSR(eFall)->attackValue = 0.5;
	ADSR(eFall)->attackTiming = .2;
	ADSR(eFall)->decayTiming = 0.2;
	ADSR(eFall)->sustainValue = 1.0;
	ADSR(eFall)->releaseTiming = 0;

	
	

	ADSR(eFall)->moding = Quadratic;
	//	ADSR(eFall)->moding = Linear;


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

	// On remet la grille comme avant
	for (std::vector<CellFall>::iterator it=falling.begin(); it!=falling.end(); ++it)
	{
		const CellFall& f = *it;
		GRID(f.e)->j = f.fromY;
	}

	// gestion des combinaisons
	if (!combinaisons.empty())
	{
		for ( std::vector<Combinais>::reverse_iterator it = combinaisons.rbegin(); it != combinaisons.rend(); ++it )
		{
			for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV )
			{
				theCombinationMarkSystem.NewMarks(2, *itV);
			}
		}
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
	theCombinationMarkSystem.DeleteMarks(2);

	LOGI("%s", __PRETTY_FUNCTION__);
}
