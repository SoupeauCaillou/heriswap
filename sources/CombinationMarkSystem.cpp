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
	COMBIN(e)->state = stat;
		
	std::stringstream a;
	a << "combinationMark"<< stat<<".png";
	RENDERING(e)->texture = theRenderingSystem.loadTextureFile("combinationMark1.png");
	RENDERING(e)->color = Color(stat*25,stat*25,stat*25,255);
	RENDERING(e)->size = Game::CellSize();
}

void CombinationMarkSystem::DeleteMarks(int stat) {
	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		Entity a = (*it).first;
		CombinationMarkComponent* cmc = (*it).second;
		if (cmc->state == stat)
			theEntityManager.DeleteEntity(a);
	}
}
void CombinationMarkSystem::DoUpdate(float dt) {
}

