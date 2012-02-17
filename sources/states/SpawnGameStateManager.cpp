#include "SpawnGameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"
#include "base/EntityManager.h"
#include "Game.h"

#include <sstream>
static void fillTheBlank(std::vector<Feuille>& spawning);
static Entity createCell(Feuille& f);
static TextureRef textureFromType(int type);

SpawnGameStateManager::SpawnGameStateManager() {

}

void SpawnGameStateManager::Setup() {
	eSpawn = theEntityManager.CreateEntity();
	ADD_COMPONENT(eSpawn, ADSR);

	ADSR(eSpawn)->idleValue = 0;
	ADSR(eSpawn)->attackValue = 0.5;
	ADSR(eSpawn)->attackTiming = 0.2;
	ADSR(eSpawn)->decayTiming = 0.2;
	ADSR(eSpawn)->sustainValue = 1.0;
	ADSR(eSpawn)->releaseTiming = 0;



}

void SpawnGameStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;


	std::vector<Combinais> c;
	fillTheBlank(spawning);
	if (spawning.size()==theGridSystem.GridSize*theGridSystem.GridSize) {
		for(int i=0; i<spawning.size(); i++) {
			createCell(spawning[i]);
		}
		do {
			c = theGridSystem.LookForCombinaison(false,true);
			// change type from cells in combi
			for(int i=0; i<c.size(); i++) {
				for(int j=0; j<c[i].points.size(); j++) {
					Entity e = theGridSystem.GetOnPos(c[i].points[j].X, c[i].points[j].Y);
					GRID(e)->type = MathUtil::RandomInt(8)+1;
					RENDERING(e)->texture = textureFromType(GRID(e)->type);
				}
			}
		} while(!c.empty());
	}
	spawning.clear();



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
				it->fe = createCell(*it);
				std::cout << "nouvelle feuille (type="<<it->type<<") en ("<<it->X<<","<<it->Y<<")\n";
			} else if (transitionCree->value == 1){
				TRANSFORM(it->fe)->rotation = 0;
			} else {
				TRANSFORM(it->fe)->rotation = -transitionCree->value*7;
			}
		}
		if (transitionCree->value == 1) {
			spawning.clear();
			std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(false,true);
			if (combinaisons.empty()) {
				if (theGridSystem.StillCombinations()) return UserInput;
				else {
					std::cout << "nouvelle grille !\n";
					theGridSystem.DeleteAll();
					fillTheBlank(spawning);
					return Spawn;
				}
			} else return Delete;

		}
	} else {
		std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(false,true);
		if (combinaisons.empty()) {
			if (theGridSystem.StillCombinations()) return UserInput;
			else {
				std::cout << "nouvelle grille 999 !\n";
				theGridSystem.DeleteAll();
				fillTheBlank(spawning);
				return Spawn;
			}
		} else return Delete;
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
				int pb=0;
				/*ne pas generer de combinaison*/
				do {
					r = MathUtil::RandomInt(8)+1;
					Entity l[5],c[5];
					for (int k=0;k<5;k++){
						 l[k] = theGridSystem.GetOnPos(i+2-k,j);
						 c[k] = theGridSystem.GetOnPos(i,j+2-k);
					 }
					if (l[0] && l[1] && GRID(l[0])->type == r && r == GRID(l[1])->type)
						pb++;
					else if (l[1] && l[3] && GRID(l[1])->type == r && r == GRID(l[3])->type)
						pb++;
					else if (l[4] && l[3] && GRID(l[3])->type == r && r == GRID(l[4])->type)
						pb++;
					else if (c[0] && c[1] && GRID(c[0])->type == r && r == GRID(c[1])->type)
						pb++;
					else if (c[1] && c[3] && GRID(c[1])->type == r && r == GRID(c[3])->type)
						pb++;
					else if (c[4] && c[3] && GRID(c[3])->type == r && r == GRID(c[4])->type)
						pb++;
					else  {
						pb=0;
					}

					if (pb >= 15){
						r = MathUtil::RandomInt(4)+1;
						while (pb!=0) {
							if (r==c[1]||r==c[3]||r==l[3]||r==l[1]) {
								r++;
							} else {
								pb=0;
							}
						}
					}
				} while (pb!=0);
				Feuille nouvfe = {i,j,0,r};
				spawning.push_back(nouvfe);
			}
		}
	}
}

static TextureRef textureFromType(int type) {
	std::stringstream s;
	s << type << ".png";
	return theRenderingSystem.loadTextureFile(s.str());
}

static Entity createCell(Feuille& f) {
	Entity e = theEntityManager.CreateEntity();
	ADD_COMPONENT(e, Transformation);
	ADD_COMPONENT(e, Rendering);
	ADD_COMPONENT(e, ADSR);
	ADD_COMPONENT(e, Grid);

	TRANSFORM(e)->position = Game::GridCoordsToPosition(f.X, f.Y);
	TRANSFORM(e)->z = 10;
	RENDERING(e)->texture = textureFromType(f.type);
	RENDERING(e)->size = Game::CellSize() * Game::CellContentScale();
	ADSR(e)->idleValue = Game::CellSize() * Game::CellContentScale();
	GRID(e)->type = f.type;
	GRID(e)->i = f.X;
	GRID(e)->j = f.Y;
	return e;
}


