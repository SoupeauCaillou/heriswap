/*
This file is part of RecursiveRunner.

@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
@author Soupe au Caillou - Gautier Pelloux-Prayer

RecursiveRunner is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.

RecursiveRunner is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RecursiveRunner.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "base/StateMachine.h"

#include "Scenes.h"

#include "Game_Private.h"
#include "HeriswapGame.h"

#include "DepthLayer.h"
#include "TwitchSystem.h"
#include "CombinationMark.h"

#include "modes/GameModeManager.h"

#include "base/EntityManager.h"

#include "systems/ADSRSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ScrollingSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include <sstream>

struct SpawnScene : public StateHandler<Scene::Enum> {
	HeriswapGame* game;

	// State variables
	Entity haveToAddLeavesInGrid, replaceGrid;
	std::vector<Feuille> newLeaves;

	SpawnScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
	    this->game = game;
	}

	void setup() {
		haveToAddLeavesInGrid = theEntityManager.CreateEntity("haveToAddLeavesInGrid");
		ADD_COMPONENT(haveToAddLeavesInGrid, ADSR);

		replaceGrid = theEntityManager.CreateEntity("replaceGrid");
		ADD_COMPONENT(replaceGrid, ADSR);

		ADSR(haveToAddLeavesInGrid)->idleValue = 0;
		ADSR(haveToAddLeavesInGrid)->attackValue = 1.0;
		ADSR(haveToAddLeavesInGrid)->decayTiming = 0;
		ADSR(haveToAddLeavesInGrid)->sustainValue = 1.0;
		ADSR(haveToAddLeavesInGrid)->releaseTiming = 0;

		ADSR(replaceGrid)->idleValue = 0;
		ADSR(replaceGrid)->attackValue = 1.0;
		ADSR(replaceGrid)->decayTiming = 0;
		ADSR(replaceGrid)->sustainValue = 1.0;
		ADSR(replaceGrid)->releaseTiming = 0;
	}

	static int newLeavesInSpawning(std::vector<Feuille>& newLeaves, int i, int j) {
		for (unsigned int k=0; k<newLeaves.size(); k++)
			if (newLeaves[k].X == i && newLeaves[k].Y == j)
				return newLeaves[k].type;
		return -1;
	}

	static void fillTheBlank(std::vector<Feuille>& newLeaves)
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
						typeVoisins[k] = voisins[k] ? GRID(voisins[k])->type : newLeavesInSpawning(newLeaves, i-2+k, j);
					} for (int k = 4; k < 8; k++)
						typeVoisins[k] = voisins[k] ? GRID(voisins[k])->type : newLeavesInSpawning(newLeaves, i, j-6+k);

					//get a type which doesn't create a combi with its neighboors
					int type, ite = 0;
					do {
						type = glm::round(glm::linearRand(0.f, (float)(theGridSystem.Types-1)));
						ite++;
					} while (theGridSystem.GridPosIsInCombination(i, j, type, typeVoisins) && ite<5000);

					Feuille nouvfe = {i,j,0,type};
					newLeaves.push_back(nouvfe);
				}
			}
		}
	}

	static Entity createCell(Feuille& f, bool assignGridPos) {
		Entity e = theEntityManager.CreateEntity("Cell", EntityType::Persistent);
		ADD_COMPONENT(e, Transformation);
		ADD_COMPONENT(e, Rendering);
		ADD_COMPONENT(e, ADSR);
		ADD_COMPONENT(e, Grid);
	    ADD_COMPONENT(e, Twitch);

		TRANSFORM(e)->position = HeriswapGame::GridCoordsToPosition(f.X, f.Y, theGridSystem.GridSize);
		TRANSFORM(e)->z = DL_Cell + glm::linearRand(0.f, 1.f) * 0.001;
		RenderingComponent* rc = RENDERING(e);
		rc->show = true;

		TRANSFORM(e)->size = glm::vec2(0.f);
		ADSR(e)->idleValue = HeriswapGame::CellSize(theGridSystem.GridSize, f.type).x * HeriswapGame::CellContentScale();
		GRID(e)->type = f.type;
		if (assignGridPos) {
			GRID(e)->i = f.X;
			GRID(e)->j = f.Y;
		}
		rc->texture = theRenderingSystem.loadTextureFile(HeriswapGame::cellTypeToTextureNameAndRotation(f.type, &TRANSFORM(e)->rotation));
		return e;
	}

	void removeEntitiesInCombination() {
		std::vector<Combinais> c;
		unsigned int ite = 0;
		//remove direct combinations but keep combinations to do (give up at 100 try)
		do {
			c = theGridSystem.LookForCombination(false,true);
			// change type from cells in combi
			for(unsigned int i=0; i<c.size(); i++) {
				int j = glm::round(glm::linearRand(0.f, (float)(c[i].points.size()-1)));
				Entity e = theGridSystem.GetOnPos(c[i].points[j].x, c[i].points[j].y);
				int type, iter = 0;
				do {
					type = glm::round(glm::linearRand(0.f, (float)(theGridSystem.Types-1)));
					iter++;
				} while (theGridSystem.GridPosIsInCombination(c[i].points[j].x, c[i].points[j].y, type, 0) && iter < 100);
				GRID(e)->type = type;
				RenderingComponent* rc = RENDERING(e);
				rc->texture = theRenderingSystem.loadTextureFile(HeriswapGame::cellTypeToTextureNameAndRotation(type, &TRANSFORM(e)->rotation));
			}
			ite++;
		} while((!c.empty() || !theGridSystem.StillCombinations()) && ite<100);
	}

	Scene::Enum NextState(bool recheckEveryoneInGrid) {
		std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(false, recheckEveryoneInGrid);
		//pas de combinaisons à supprimer, qu'est-ce qu'il faut donc faire ?
		if (combinaisons.empty()) {
			//si y a plus de combi, on genere une nouvelle grille (uniquement parce que y a plus de solutions)
			if (!theGridSystem.StillCombinations()) {
				//(on doit pas etre en changement de niveau / fin de jeu)
				if (game->datas->mode == Normal && 
					game->datas->mode2Manager[game->datas->mode]->LevelUp())
					return Scene::LevelChanged;
				//inutile logiquement (on quitte avant)
				//~else if (modeMgr->GetMode() == TilesAttack && modeMgr->GameProgressPercent() == 1)
					//~return GameToBlack;

				ADSR(replaceGrid)->active = true;
				std::vector<Entity> feuilles = theGridSystem.RetrieveAllEntityWithComponent();
				for ( std::vector<Entity>::reverse_iterator it = feuilles.rbegin(); it != feuilles.rend(); ++it ) {
					CombinationMark::markCellInCombination(*it);
				}
			//sinon y a des combi, c'est au player de player
			} else {
				return Scene::UserInput;
			}
		//sinon on a des combinaisons dans la grille on passe à Delete
		} else {
			return Scene::Delete;
		}
		return Scene::Spawn;
	}

	///----------------------------------------------------------------------------//
	///--------------------- ENTER SECTION ----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreEnter(Scene::Enum) override {
	}

	void onEnter(Scene::Enum) override {
		LOGI("'" << __PRETTY_FUNCTION__ << "'");

		fillTheBlank(newLeaves);

		//we need to create the whole grid (start game and level change)
		if ((int)newLeaves.size() == theGridSystem.GridSize*theGridSystem.GridSize) {
	     	LOGI("create '" << newLeaves.size() << "' cells");
			for(unsigned int i=0; i<newLeaves.size(); i++) {
	            if (newLeaves[i].entity == 0)
				    newLeaves[i].entity = createCell(newLeaves[i], true);
			}
	        ADSR(haveToAddLeavesInGrid)->active = true;
			removeEntitiesInCombination();
		} else {
		    ADSR(haveToAddLeavesInGrid)->active = false;
	    }
	    ADSR(haveToAddLeavesInGrid)->activationTime = 0;

		ADSR(replaceGrid)->activationTime = 0;
		ADSR(replaceGrid)->active = false;
	}

	///----------------------------------------------------------------------------//
	///--------------------- UPDATE SECTION ---------------------------------------//
	///----------------------------------------------------------------------------//
	Scene::Enum update(float) override {
		//si on bouche les trous
		if (!newLeaves.empty()) {
	        bool fullGridSpawn = (newLeaves.size() == (unsigned)theGridSystem.GridSize*theGridSystem.GridSize);
			ADSR(haveToAddLeavesInGrid)->active = true;
			for ( std::vector<Feuille>::reverse_iterator it = newLeaves.rbegin(); it != newLeaves.rend(); ++it ) {
				if (it->entity == 0) {
					it->entity = createCell(*it, fullGridSpawn);
				} else {
	                GridComponent* gc = GRID(it->entity);
	                if (fullGridSpawn) {
	                    gc->i = gc->j = -1;
	                }
					TransformationComponent* tc = TRANSFORM(it->entity);
					//leaves grow up from 0 to fixed size
					glm::vec2 s = HeriswapGame::CellSize(theGridSystem.GridSize, gc->type);
					if (ADSR(haveToAddLeavesInGrid)->value == 1){
						tc->size = glm::vec2(s.x * 0.1, s.y);
						gc->i = it->X;
						gc->j = it->Y;
					} else {
						tc->size = s * ADSR(haveToAddLeavesInGrid)->value;
					}
				}
			}
			//tout le monde est en place : quel sera le prochain état ?
			if (ADSR(haveToAddLeavesInGrid)->value == 1) {
				newLeaves.clear();
				return NextState(true);
			}
		//sinon si on est en train de remplacer la grille (plus de combinaisons en cours de jeu)
		} else if (ADSR(replaceGrid)->active) {
	        std::vector<Entity> feuilles = theGridSystem.RetrieveAllEntityWithComponent();
	        //les feuilles disparaissent (taille tend vers 0)
	        for ( std::vector<Entity>::reverse_iterator it = feuilles.rbegin(); it != feuilles.rend(); ++it ) {
	            glm::vec2 cellSize = HeriswapGame::CellSize(theGridSystem.GridSize, GRID(*it)->type) * HeriswapGame::CellContentScale() * (1 - ADSR(replaceGrid)->value);
	            ADSR(*it)->idleValue = cellSize.x;
	        }
	        //les feuilles ont disparu, on les supprime et on remplit avec de nouvelles feuilles
	        if (ADSR(replaceGrid)->value == ADSR(replaceGrid)->sustainValue) {
				theGridSystem.DeleteAll();
	            fillTheBlank(newLeaves);
	            LOGI("nouvelle grille de '" << newLeaves.size() << "' elements! ");
	            game->datas->successMgr->gridResetted = true;
	            ADSR(haveToAddLeavesInGrid)->activationTime = 0;
				ADSR(haveToAddLeavesInGrid)->active = true;
	        }
	    //sinon on regarde dans quel état on arrive avec notre grille actuelle
	    } else {
			return NextState(false);
		}
		return Scene::Spawn;
	}

	///----------------------------------------------------------------------------//
	///--------------------- EXIT SECTION -----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreExit(Scene::Enum) override {
	}

	void onExit(Scene::Enum) override {
		LOGI("'" << __PRETTY_FUNCTION__ << "'");
		newLeaves.clear();
	}
};

namespace Scene {
	StateHandler<Scene::Enum>* CreateSpawnSceneHandler(HeriswapGame* game) {
    	return new SpawnScene(game);
	}
}