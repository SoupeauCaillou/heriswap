/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
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
	ADSR(eGrid)->attackTiming = difficulty*0.5;
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
     	LOGI("create %u cells", spawning.size());
		for(unsigned int i=0; i<spawning.size(); i++) {
            if (spawning[i].entity == 0)
			    spawning[i].entity = createCell(spawning[i], true);
		}
		int ite=0;
		//get a new grid which has no direct combinations but still combinations to do (give up at 100 try)
		do {
			c = theGridSystem.LookForCombination(false,true);
			// change type from cells in combi
			for(unsigned int i=0; i<c.size(); i++) {
				int j = MathUtil::RandomInt(c[i].points.size());
				Entity e = theGridSystem.GetOnPos(c[i].points[j].X, c[i].points[j].Y);
				int type, iter = 0;
				do {
					type = MathUtil::RandomInt(theGridSystem.Types);
					iter++;
				} while (theGridSystem.GridPosIsInCombination(c[i].points[j].X, c[i].points[j].Y, type, 0) && iter<100);
				GRID(e)->type = type;
				RenderingComponent* rc = RENDERING(e);
				rc->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(type, &TRANSFORM(e)->rotation));
			}
			ite++;
		} while((!c.empty() || !theGridSystem.StillCombinations()) && ite<100);

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
			if (it->entity == 0) {
				it->entity = createCell(*it, fullGridSpawn);
			} else {
                GridComponent* gc = GRID(it->entity);
                if (fullGridSpawn) {
                    gc->i = gc->j = -1;
                }
				TransformationComponent* tc = TRANSFORM(it->entity);
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
	//sinon si on fait une nouvelle grille
	} else if (ADSR(eGrid)->active) {
        std::vector<Entity> feuilles = theGridSystem.RetrieveAllEntityWithComponent();
        for ( std::vector<Entity>::reverse_iterator it = feuilles.rbegin(); it != feuilles.rend(); ++it ) {
            Vector2 cellSize = Vector2(Game::CellSize(theGridSystem.GridSize) * Game::CellContentScale() * (1 - ADSR(eGrid)->value));
            ADSR(*it)->idleValue = cellSize.X;
        }
        if (ADSR(eGrid)->value == ADSR(eGrid)->sustainValue) {
            theGridSystem.DeleteAll();
            fillTheBlank(spawning);
            LOGI("nouvelle grille de %lu elements! ", spawning.size());
            successMgr->gridResetted = true;
            transitionCree->activationTime = 0;
			transitionCree->active = true;
        }
    //sinon l'etat suivant depend de la grille actuelle
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
	spawning.clear();
}


int newLeavesInSpawning(std::vector<Feuille>& spawning, int i, int j) {
	for (unsigned int k=0; k<spawning.size(); k++)
		if (spawning[k].X == i && spawning[k].Y == j)
			return spawning[k].type;
	return -1;
}

void fillTheBlank(std::vector<Feuille>& spawning)
{
	for (int i=0; i<theGridSystem.GridSize; i++){
		for (int j=0; j<theGridSystem.GridSize; j++){
			Entity e = theGridSystem.GetOnPos(i,j);
			//oh ! it misses someone on (i,j)
			if (!e){
				//get its neighboors...
				Entity voisins[8] = {
					theGridSystem.GetOnPos(i-2, j),
					theGridSystem.GetOnPos(i-1, j),
					theGridSystem.GetOnPos(i+1, j),
					theGridSystem.GetOnPos(i+2, j),
					theGridSystem.GetOnPos(i, j-2),
					theGridSystem.GetOnPos(i, j-1),
					theGridSystem.GetOnPos(i, j+1),
					theGridSystem.GetOnPos(i, j+2)
				};
				
				//and their types
				int typeVoisins[8];
				for (int k = 0; k < 4; k++) {
					typeVoisins[k] = voisins[k] ? GRID(voisins[k])->type : newLeavesInSpawning(spawning, i-2+k, j);
				} for (int k = 4; k < 8; k++)
					typeVoisins[k] = voisins[k] ? GRID(voisins[k])->type : newLeavesInSpawning(spawning, i, j-6+k);
				
				//get a type which doesn't create a combi with its neighboors
				int type, ite = 0;
				do {
					type = MathUtil::RandomInt(theGridSystem.Types);
					ite++;
				} while (theGridSystem.GridPosIsInCombination(i, j, type, typeVoisins) && ite<5000);

				Feuille nouvfe = {i,j,0,type};
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

	TRANSFORM(e)->size = Vector2(0.0f);
	ADSR(e)->idleValue = Game::CellSize(theGridSystem.GridSize) * Game::CellContentScale();
	GRID(e)->type = f.type;
	if (assignGridPos) {
		GRID(e)->i = f.X;
		GRID(e)->j = f.Y;
	}
	rc->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(f.type, &TRANSFORM(e)->rotation));
	return e;
}
