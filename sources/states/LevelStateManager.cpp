#include "LevelStateManager.h"


LevelStateManager::LevelStateManager() {

}

LevelStateManager::~LevelStateManager() {

}

void LevelStateManager::Setup() {
	eGrid = theEntityManager.CreateEntity();
	ADD_COMPONENT(eGrid, ADSR);

	ADSR(eGrid)->idleValue = 0;
	ADSR(eGrid)->attackValue = Game::GridCoordsToPosition(0,theGridSystem.GridSize).Y+8;
	ADSR(eGrid)->attackTiming = 1.;
	ADSR(eGrid)->decayTiming = 0.;
	ADSR(eGrid)->sustainValue = ADSR(eGrid)->attackValue;
	ADSR(eGrid)->releaseTiming = 0;
	ADSR(eGrid)->active = false;
}

void LevelStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	ADSR(eGrid)->active = true;
	std::vector<Entity> ent = theGridSystem.RetrieveAllEntityWithComponent();
	for (int i=0; i<ent.size(); i++) {
		FeuilleOrientee truc = {ent[i], Game::GridCoordsToPosition(GRID(ent[i])->i, GRID(ent[i])->j), MathUtil::RandomIntInRange(-1,2)};
		feuilles.push_back(truc);
	}
}

GameState LevelStateManager::Update(float dt, GameModeManager* modeMng) {
	LOGI("%f", ADSR(eGrid)->value);
	for ( std::vector<FeuilleOrientee>::reverse_iterator it = feuilles.rbegin(); it != feuilles.rend(); ++it ) {
		TRANSFORM(it->e)->rotation = 0.3*it->sens*ADSR(eGrid)->value;
		TRANSFORM(it->e)->position = it->pos - Vector2(0,ADSR(eGrid)->value);
	}

	if (ADSR(eGrid)->value == ADSR(eGrid)->sustainValue) {
		std::cout << "nouvelle grille !\n";
		theGridSystem.DeleteAll();
		return Spawn;
	}
	return LevelChanged;
}

void LevelStateManager::Exit() {
	ADSR(eGrid)->active = false;
	feuilles.clear();
	LOGI("%s", __PRETTY_FUNCTION__);
}
