#include "SpawnGameStateManager.h"

#include <sstream>

#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/ScrollingSystem.h"

#include "Game.h"
#include "DepthLayer.h"
#include "TwitchSystem.h"
#include "CombinationMark.h"


static void fillTheBlank(std::vector<Feuille>& spawning);
static Entity createCell(Feuille& f, bool assignGridPos);

SpawnGameStateManager::SpawnGameStateManager(SuccessManager* smgr){
	successMgr = smgr;
}

void SpawnGameStateManager::setAnimSpeed() {
	int difficulty = (theGridSystem.GridSize!=8)+1; //1 : normal, 2 : easy

	ADSR(eSpawn)->idleValue = 0;
	ADSR(eSpawn)->attackValue = 1.0;
	ADSR(eSpawn)->attackTiming = difficulty*0.2;
	ADSR(eSpawn)->decayTiming = 0;
	ADSR(eSpawn)->sustainValue = 1.0;
	ADSR(eSpawn)->releaseTiming = 0;

	ADSR(eGrid)->idleValue = 0;
	ADSR(eGrid)->attackValue = 1.0;
	ADSR(eGrid)->attackTiming = difficulty*0.3;
	ADSR(eGrid)->decayTiming = 0;
	ADSR(eGrid)->sustainValue = 1.0;
	ADSR(eGrid)->releaseTiming = 0;
}

void SpawnGameStateManager::Setup() {
	eSpawn = theEntityManager.CreateEntity();
	ADD_COMPONENT(eSpawn, ADSR);


	eGrid = theEntityManager.CreateEntity();
	ADD_COMPONENT(eGrid, ADSR);

	setAnimSpeed();
}

void SpawnGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	std::vector<Combinais> c;
	fillTheBlank(spawning);
	if ((int)spawning.size()==theGridSystem.GridSize*theGridSystem.GridSize) {
     	std::cout << "create " << spawning.size() << " cells" << std::endl;
		for(unsigned int i=0; i<spawning.size(); i++) {
            if (spawning[i].fe == 0)
			    spawning[i].fe = createCell(spawning[i], true);
		}
		int ite=0;
		//get a new grid which has no direct combinations but still combinations to do (give up at 100 try)
		do {
			c = theGridSystem.LookForCombination(false,true);
			// change type from cells in combi
			for(unsigned int i=0; i<c.size(); i++) {
				for(unsigned int j=0; j<c[i].points.size(); j++) {
					Entity e = theGridSystem.GetOnPos(c[i].points[j].X, c[i].points[j].Y);
					int type = MathUtil::RandomInt(theGridSystem.Types);
					GRID(e)->type = type;
					RenderingComponent* rc = RENDERING(e);
					rc->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(type, &TRANSFORM(e)->rotation));
				}
			}
			ite++;
		} while((!c.empty() || !theGridSystem.StillCombinations()) && ite<100 );

        ADSR(eSpawn)->active = true;
	} else {
	    ADSR(eSpawn)->active = false;
    }
    ADSR(eSpawn)->activationTime = 0;

	ADSR(eGrid)->activationTime = 0;
	ADSR(eGrid)->active = false;
}

GameState SpawnGameStateManager::Update(float dt __attribute__((unused))) {
	ADSRComponent* transitionCree = ADSR(eSpawn);
	//si on doit recree des feuilles
	if (!spawning.empty()) {
        bool fullGridSpawn = (spawning.size() == (unsigned)theGridSystem.GridSize*theGridSystem.GridSize);
		transitionCree->active = true;
		for ( std::vector<Feuille>::reverse_iterator it = spawning.rbegin(); it != spawning.rend(); ++it ) {
			if (it->fe == 0) {
				it->fe = createCell(*it, fullGridSpawn);
			} else {
                GridComponent* gc = GRID(it->fe);
                if (fullGridSpawn) {
                    gc->i = gc->j = -1;
                }
				TransformationComponent* tc = TRANSFORM(it->fe);
				float s = Game::CellSize(theGridSystem.GridSize);
				if (transitionCree->value == 1){
					tc->size = Vector2(s*0.1, s);
					gc->i = it->X;
					gc->j = it->Y;
				} else {
					tc->size = Vector2(s * transitionCree->value, s * transitionCree->value);
				}
			}
		}
		if (transitionCree->value == 1) {
			spawning.clear();
			return NextState(true);
		}
	//sinon on verifie qu'il reste des combi
	} else if (ADSR(eGrid)->active) {
        std::vector<Entity> feuilles = theGridSystem.RetrieveAllEntityWithComponent();
        for ( std::vector<Entity>::reverse_iterator it = feuilles.rbegin(); it != feuilles.rend(); ++it ) {
            Vector2 cellSize = Game::CellSize(theGridSystem.GridSize) * Game::CellContentScale() * (1 - ADSR(eGrid)->value);
            ADSR(*it)->idleValue = cellSize.X;
        }
        if (ADSR(eGrid)->value == ADSR(eGrid)->sustainValue) {
            theGridSystem.DeleteAll();
            fillTheBlank(spawning);
            LOGI("nouvelle grille de %lu elements! ", spawning.size());
            successMgr->gridResetted = true;
        }
    } else {
		return NextState(false);
	}
	return Spawn;
}

GameState SpawnGameStateManager::NextState(bool marker) {
	std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(false,marker);
	//si on a des combinaisons dans la grille on passe direct à Delete
	if (!combinaisons.empty()) {
		return Delete;
	//sinon
	} else {
		//si y a des combi, c'est au player de player
		if (theGridSystem.StillCombinations()) return UserInput;
		//sinon on genere une nouvelle grille
		else {
			ADSR(eGrid)->active = true;
			std::vector<Entity> feuilles = theGridSystem.RetrieveAllEntityWithComponent();
			for ( std::vector<Entity>::reverse_iterator it = feuilles.rbegin(); it != feuilles.rend(); ++it ) {
				CombinationMark::markCellInCombination(*it);
			}
		}
	}
	return Spawn;
}

void SpawnGameStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
}

void fillTheBlank(std::vector<Feuille>& spawning)
{
	for (int i=0; i<theGridSystem.GridSize; i++){
		for (int j=0; j<theGridSystem.GridSize; j++){
			if (theGridSystem.GetOnPos(i,j) == 0){
				int r;
				int pb=0;
				/*ne pas generer de combinaison (fonctionne seulement avec les cellules deja dans la grille)*/
				do {
					r = MathUtil::RandomInt(theGridSystem.Types);
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
						r = MathUtil::RandomInt(4);
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

static Entity createCell(Feuille& f, bool assignGridPos) {
	Entity e = theEntityManager.CreateEntity(EntityManager::Persistent);
	ADD_COMPONENT(e, Transformation);
	ADD_COMPONENT(e, Rendering);
	ADD_COMPONENT(e, ADSR);
	ADD_COMPONENT(e, Grid);
    ADD_COMPONENT(e, Twitch);

	TRANSFORM(e)->position = Game::GridCoordsToPosition(f.X, f.Y, theGridSystem.GridSize);
	TRANSFORM(e)->z = DL_Cell;
	RenderingComponent* rc = RENDERING(e);
	rc->hide = false;

	TRANSFORM(e)->size = 0;
	ADSR(e)->idleValue = Game::CellSize(theGridSystem.GridSize) * Game::CellContentScale();
	GRID(e)->type = f.type;
	if (assignGridPos) {
		GRID(e)->i = f.X;
		GRID(e)->j = f.Y;
	}
	rc->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(f.type, &TRANSFORM(e)->rotation));
	return e;
}
