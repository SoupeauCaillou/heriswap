#include "SpawnGameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"
#include "base/EntityManager.h"
#include "Game.h"

#include <sstream>
static void fillTheBlank(std::vector<Feuille>& spawning);

SpawnGameStateManager::SpawnGameStateManager() {

}

void SpawnGameStateManager::Setup() {
	eSpawn = theEntityManager.CreateEntity();
	theADSRSystem.Add(eSpawn);
	ADSR(eSpawn)->idleValue = 0;
	ADSR(eSpawn)->attackValue = 0.5;
	ADSR(eSpawn)->attackTiming = 0.2;
	ADSR(eSpawn)->decayTiming = 0.2;
	ADSR(eSpawn)->sustainValue = 1.0;
	ADSR(eSpawn)->releaseTiming = 0;
}
	
void SpawnGameStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	ADSRComponent* transitionCree = ADSR(eSpawn);
	transitionCree->activationTime = 0;
	transitionCree->active = false;

	fillTheBlank(spawning);
}

GameState SpawnGameStateManager::Update(float dt) {
	ADSRComponent* transitionCree = ADSR(eSpawn);

	if (!spawning.empty()) {
		transitionCree->active = true;
		for ( std::vector<Feuille>::reverse_iterator it = spawning.rbegin(); it != spawning.rend(); ++it ) {
			if (it->fe == 0) {
				Entity e = theEntityManager.CreateEntity();
				theTransformationSystem.Add(e);
				TRANSFORM(e)->position = Game::GridCoordsToPosition(it->X, it->Y);
				theRenderingSystem.Add(e);
				std::stringstream s;
				s << it->type << ".png";
				RENDERING(e)->texture = theRenderingSystem.loadTextureFile(s.str());
				RENDERING(e)->size = Game::CellSize() * Game::CellContentScale();
				theADSRSystem.Add(e);
				ADSR(e)->idleValue = Game::CellSize() * Game::CellContentScale();
				theGridSystem.Add(e);
				GRID(e)->type = it->type;
				GRID(e)->i = it->X;
				GRID(e)->j = it->Y;
				it->fe = e;
				std::cout << "nouvelle feuille en ("<<it->X<<","<<it->Y<<")\n";	
			} else if (transitionCree->value == 1){
				TRANSFORM(it->fe)->rotation = 0;
			} else {
				TRANSFORM(it->fe)->rotation = transitionCree->value*7;
			}
		}
		if (transitionCree->value == 1) {
			spawning.clear();
			return UserInput;
		}
	} else {
		return UserInput;
	}
	return Spawn;
}
	
void SpawnGameStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void fillTheBlank(std::vector<Feuille>& spawning)
{
	for (int i=0; i<theGridSystem.GridSize; i++){
		for (int j=0; j<theGridSystem.GridSize; j++){
			if (theGridSystem.GetOnPos(i,j) == 0){
				int r;
				int pb;
				/*ne pas generer de combinaison*/
				do {	
					pb = 0;
					r = MathUtil::RandomInt(8);
					Entity l[5],c[5];
					for (int k=0;k<5;k++){
						 l[k] = theGridSystem.GetOnPos(i+2-k,j);
						 c[k] = theGridSystem.GetOnPos(i,j+2-k);
					 }
					if (l[0] && l[1] && GRID(l[0])->type == r && r == GRID(l[1])->type)
						pb++;
					if (l[1] && l[3] && GRID(l[1])->type == r && r == GRID(l[3])->type)
						pb++;
					if (l[4] && l[3] && GRID(l[3])->type == r && r == GRID(l[4])->type)
						pb++;
					if (c[0] && c[1] && GRID(c[0])->type == r && r == GRID(c[1])->type)
						pb++;
					if (c[1] && c[3] && GRID(c[1])->type == r && r == GRID(c[3])->type)
						pb++;
					if (c[4] && c[3] && GRID(c[3])->type == r && r == GRID(c[4])->type)
						pb++;
					
				} while (pb!=0 && pb<15);
				
				Feuille nouvfe = {i,j,0,r};
				spawning.push_back(nouvfe);
			}
		}	
	}							
}

