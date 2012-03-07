#include "CombinationMarkSystem.h"

INSTANCE_IMPL(CombinationMarkSystem);

CombinationMarkSystem::CombinationMarkSystem() : ComponentSystemImpl<CombinationMarkComponent>("combination_") {
}

void CombinationMarkSystem::NewMarks(int stat, Vector2 coord){
	Entity e = theEntityManager.CreateEntity();
	theEntityManager.AddComponent(e, &theCombinationMarkSystem);
	theEntityManager.AddComponent(e, &theTransformationSystem);
	theEntityManager.AddComponent(e, &theADSRSystem);
	theEntityManager.AddComponent(e, &theRenderingSystem);
	TRANSFORM(e)->position = Game::GridCoordsToPosition(coord.X, coord.Y);
	TRANSFORM(e)->z = 5;
	RENDERING(e)->hide = false;
	RENDERING(e)->size = Game::CellSize();

				RENDERING(e)->texture = theRenderingSystem.loadTextureFile("combinationMark2.png");
/*
	switch (stat) {
		case 1 : // spawn
			RENDERING(e)->color = Color(255,140,0,200);
			break;
		case 2 : // fall
			RENDERING(e)->color = Color(255,0,0,200);
			break;
		case 3 : // delete
			RENDERING(e)->color = Color(0,255,0,200);
			break;
		case 4 : // magickey vert
			RENDERING(e)->color = Color(160,15,160,200);
			break;
		case 5 : // magickey hori
			RENDERING(e)->color = Color(255,255,0,200);
			break;
		default :
			RENDERING(e)->color = Color(220,44,150,200);
			break;
	}	*/	
			
	COMBIN(e)->state = stat;
}

void CombinationMarkSystem::DeleteMarks(int stat) {
	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		Entity a = (*it).first;
		CombinationMarkComponent* cmc = (*it).second;
		if (cmc->state == stat)
			theEntityManager.DeleteEntity(a);
	}
}

int CombinationMarkSystem::NumberOfThisType(int stat) {
	int cpt = 0;
	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		if (it->second->state == stat)
			cpt++;
	}
	return cpt;
}

void CombinationMarkSystem::DoUpdate(float dt) {
}

