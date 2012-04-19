#include "CombinationMarkSystem.h"
#include "TwitchSystem.h"
#include "DepthLayer.h"

INSTANCE_IMPL(CombinationMarkSystem);

CombinationMarkSystem::CombinationMarkSystem() : ComponentSystemImpl<CombinationMarkComponent>("combination_") {
}

/*
            float angle = TRANSFORM(e)->rotation;
            TWITCH(e)->minAngle = angle - 0.5;
            TWITCH(e)->maxAngle = angle + 0.5;
            TWITCH(e)->variance = MathUtil::RandomFloat() * 0.2;
            TWITCH(e)->speed = MathUtil::RandomFloatInRange(12, 17);
*/

void CombinationMarkSystem::NewMarks(int stat, Vector2 coord){
    Entity e = theGridSystem.GetOnPos(coord.X, coord.Y);
    if (!e || TWITCH(e)->speed > 0)
        return;
    float angle = TRANSFORM(e)->rotation;
    TWITCH(e)->minAngle = angle - 0.5;
    TWITCH(e)->maxAngle = angle + 0.5;
    TWITCH(e)->variance = MathUtil::RandomFloat() * 0.2;
    TWITCH(e)->speed = MathUtil::RandomFloatInRange(12, 17);

    return;
#if 0
	Entity e = theEntityManager.CreateEntity();
	theEntityManager.AddComponent(e, &theCombinationMarkSystem);
	theEntityManager.AddComponent(e, &theTransformationSystem);
	theEntityManager.AddComponent(e, &theADSRSystem);
	theEntityManager.AddComponent(e, &theRenderingSystem);
	TRANSFORM(e)->position = Game::GridCoordsToPosition(coord.X, coord.Y);
	TRANSFORM(e)->z = DL_CombinationMark;
	RENDERING(e)->hide = false;
	TRANSFORM(e)->size = Game::CellSize();

	switch (stat) {
		case 1 : // user input
			RENDERING(e)->color = Color(.7f,.7f,0.f,0.3f);
			break;
		case 2 : // fall
			RENDERING(e)->color = Color(1.f,0.f,0.f,0.3f);
			break;
		case 3 : // delete
			RENDERING(e)->color = Color(0.f,1.f,0.f,0.3f);
			break;
		case 4 : // magickey vert
			RENDERING(e)->color = Color(1.0f,0.0f,0.0f,0.3f);
			break;
		case 5 : // magickey hori
			RENDERING(e)->color = Color(0.0f,0.0f,1.0f,.3f);
			break;
		default :
			RENDERING(e)->color = Color(.8f,.1f,.5f,.3f);
			break;
	}

	COMBIN(e)->state = stat;
#endif
}

void CombinationMarkSystem::DeleteMarks(int stat) {
    std::vector<Entity> all = theTwitchSystem.RetrieveAllEntityWithComponent();
    for(int i=0; i<all.size(); i++) {
        // TWITCH(all[i])->speed = 0;
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

