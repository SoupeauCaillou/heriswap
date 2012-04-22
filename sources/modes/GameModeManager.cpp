#include "modes/GameModeManager.h"
#include "TwitchSystem.h"
#include "systems/ScrollingSystem.h"
#include <fstream>
#include "AnimedEntity.h"

static float initialHerissonPosition(Entity herisson) {
    return -PlacementHelper::ScreenWidth * 0.5 - TRANSFORM(herisson)->size.X * 0.5;
}

static float finalHerissonPosition(Entity herisson) {
    return PlacementHelper::ScreenWidth * 0.5 + TRANSFORM(herisson)->size.X * 0.5;
}


float GameModeManager::position(float t) {
	float p = 0;

	if (t<=pts[0].X) {
		p = pts[0].Y;
	} else {
		int i;
		for (i = 0; i<pts.size()-1; i++) {
			if (t>pts[i].X && t<pts[i+1].X) {
				p = MathUtil::Lerp(pts[i].Y, pts[i+1].Y, (t-pts[i].X)/(pts[i+1].X-pts[i].X));
				break;
			}
		}
		if (i == pts.size()-1) {
			p = pts[pts.size()-1].Y;
		}
	}
	return MathUtil::Lerp(initialHerissonPosition(herisson), finalHerissonPosition(herisson), p);
}

void GameModeManager::LoadHerissonTexture(int type) {
    loadHerissonTexture(type, c);
}

void GameModeManager::Setup() {
	herisson = theEntityManager.CreateEntity();
	ADD_COMPONENT(herisson, Transformation);
	ADD_COMPONENT(herisson, Rendering);
	TRANSFORM(herisson)->z = DL_Animal;
	TRANSFORM(herisson)->size = Vector2(PlacementHelper::GimpWidthToScreen(142),PlacementHelper::GimpHeightToScreen(116));
	c = new AnimatedActor();
	c->actor.e = herisson;
	c->frames=0;
	c->actor.speed = 4.1;
	LoadHerissonTexture(bonus+1);
	RENDERING(herisson)->hide = true;

	branch = theEntityManager.CreateEntity();
	ADD_COMPONENT(branch, Transformation);
	TRANSFORM(branch)->z = DL_Branch;
	TRANSFORM(branch)->size = Vector2(PlacementHelper::GimpWidthToScreen(800), PlacementHelper::GimpHeightToScreen(400.0));
	TransformationSystem::setPosition(TRANSFORM(branch), Vector2(0, PlacementHelper::GimpYToScreen(0)), TransformationSystem::N);
	ADD_COMPONENT(branch, Rendering);
	RENDERING(branch)->hide = true;
	RENDERING(branch)->texture = theRenderingSystem.loadTextureFile("branche.png");

	decor2nd = theEntityManager.CreateEntity();
	ADD_COMPONENT(decor2nd, Transformation);
	TRANSFORM(decor2nd)->z = DL_Decor2nd;
	TRANSFORM(decor2nd)->size = Vector2(PlacementHelper::GimpWidthToScreen(800), PlacementHelper::GimpWidthToScreen(470));
	TransformationSystem::setPosition(TRANSFORM(decor2nd), Vector2(0, PlacementHelper::GimpYToScreen(610)), TransformationSystem::N);
	ADD_COMPONENT(decor2nd, Scrolling);
	SCROLLING(decor2nd)->images.push_back("decor2nd_0.png");
	SCROLLING(decor2nd)->images.push_back("decor2nd_1.png");
	SCROLLING(decor2nd)->images.push_back("decor2nd_2.png");
	SCROLLING(decor2nd)->images.push_back("decor2nd_3.png");
	SCROLLING(decor2nd)->speed = Vector2(-.1, 0);
	SCROLLING(decor2nd)->displaySize = Vector2(TRANSFORM(decor2nd)->size.X * 1.01, TRANSFORM(decor2nd)->size.Y);
	SCROLLING(decor2nd)->hide = true;

	decor1er = theEntityManager.CreateEntity();
	ADD_COMPONENT(decor1er, Transformation);
	TRANSFORM(decor1er)->z = DL_Decor1er;
	TRANSFORM(decor1er)->size = Vector2(PlacementHelper::GimpWidthToScreen(800), PlacementHelper::GimpWidthToScreen(300));
	TransformationSystem::setPosition(TRANSFORM(decor1er), Vector2(0, PlacementHelper::GimpYToScreen(1280)), TransformationSystem::S);
	ADD_COMPONENT(decor1er, Scrolling);
	SCROLLING(decor1er)->images.push_back("decor1er_0.png");
	SCROLLING(decor1er)->images.push_back("decor1er_1.png");
	SCROLLING(decor1er)->images.push_back("decor1er_2.png");
	SCROLLING(decor1er)->images.push_back("decor1er_3.png");
	SCROLLING(decor1er)->speed = Vector2(-0.01, 0);
	SCROLLING(decor1er)->displaySize = Vector2(TRANSFORM(decor1er)->size.X * 1.01, TRANSFORM(decor1er)->size.Y);
	SCROLLING(decor1er)->hide = true;

	fillVec();
	
	uiHelper.build();
}

void GameModeManager::Enter() {
	RENDERING(herisson)->hide = false;
	RENDERING(branch)->hide = false;
	SCROLLING(decor2nd)->hide = false;
	SCROLLING(decor1er)->hide = false;
	
	generateLeaves(6);
	uiHelper.show();
    theGridSystem.HideAll(false);
    TRANSFORM(herisson)->position = initialHerissonPosition(herisson);
}

void GameModeManager::Exit() {
	RENDERING(herisson)->hide = true;
	RENDERING(branch)->hide = true;
	SCROLLING(decor2nd)->hide = true;
	SCROLLING(decor1er)->hide = true;
	
	uiHelper.hide();
	
	// delete leaves
	for (int az=0;az<branchLeaves.size();az++) {
		theEntityManager.DeleteEntity(branchLeaves[az].e);
	}
	branchLeaves.clear();
    theGridSystem.HideAll(true);
}

void GameModeManager::TogglePauseDisplay(bool paused) {
	theGridSystem.HideAll(paused);
}

void GameModeManager::generateLeaves(int nb) {
	for (int az=0;az<branchLeaves.size();az++)
		theEntityManager.DeleteEntity(branchLeaves[az].e);

	branchLeaves.clear();
	fillVec();
	//std::vector<Render> swapper;
	for (int i=0;i<nb;i++) {
		for (int j=0;j<8;j++) {
			Entity e = theEntityManager.CreateEntity();
			ADD_COMPONENT(e, Transformation);
			ADD_COMPONENT(e, Rendering);
            ADD_COMPONENT(e, Twitch);
			RENDERING(e)->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(j, 0));
			RENDERING(e)->hide = false;
			TRANSFORM(e)->size = Game::CellSize() * Game::CellContentScale();

			int rand = MathUtil::RandomInt(posBranch.size());
			TRANSFORM(e)->position = posBranch[rand].v;
			TRANSFORM(e)->rotation = posBranch[rand].rot;
			//swapper.push_back(posBranch[rand]);
			posBranch.erase(posBranch.begin()+rand);

			TRANSFORM(e)->z = MathUtil::Lerp(DL_LeafMin, DL_LeafMax, MathUtil::RandomFloat());
			BranchLeaf bl;
			bl.e = e;
			bl.type=j+1;
			branchLeaves.push_back(bl);
		}
	}
	//swapper.swap(posBranch);
}

void GameModeManager::deleteLeaves(int type, int nb) {
	for (int i=0; nb>0 && i<branchLeaves.size(); i++) {
		if (type == branchLeaves[i].type) {
			theEntityManager.DeleteEntity(branchLeaves[i].e);
			branchLeaves.erase(branchLeaves.begin()+i);
			nb--;
			i--;
		}
	}
}

void GameModeManager::fillVec() {
#if 0
	int t;
	std::ifstream file("position_feuilles.txt");
	int count = 0;
	std::string line;
	posBranch.clear(); // a la place de swapper ...
	while ( getline( file, line ) ){
		++count;
		if (count>t) {
			Vector2 v = Vector2::Zero;
			float r = 0.f;
			sscanf(line.c_str(), "%f %f %f", &v.X, &v.Y, &r);
			v.X =PlacementHelper::GimpXToScreen(v.X);
			v.Y =PlacementHelper::GimpYToScreen(v.Y);
			r =MathUtil::ToRadians(r);
			Render truc = {v,r};
			posBranch.push_back(truc);
			t++;
		}
	}
	while (count < t) {
		posBranch.pop_back();
		t--;
	}
#else
	posBranch.clear();
	#include "PositionFeuilles.h"
	for (int i=0; i<48; i++) {
		Vector2 v(PlacementHelper::GimpXToScreen(pos[3*i]), PlacementHelper::GimpYToScreen(pos[3*i+1]));
		Render truc = {v, MathUtil::ToRadians(pos[3*i+2])};
		posBranch.push_back(truc);
	}
#endif
}

void GameModeManager::updateHerisson(float dt, float obj, float herissonSpeed) {
	// default herisson behavior: move to
	TransformationComponent* tc = TRANSFORM(herisson);
	float newPos = tc->position.X;
	if (herissonSpeed == 0) {
		float targetPosX = position(obj);
    	float distance = targetPosX - tc->position.X;
		if (distance != 0.f) {
			herissonSpeed = (distance > 0) ? 1.0f : -1.0f;
			if (distance < 0) {
				newPos = MathUtil::Max(targetPosX, tc->position.X + herissonSpeed*dt);
			} else {
				newPos = MathUtil::Min(targetPosX, tc->position.X + herissonSpeed*dt);
			}
		}
	} else {
		newPos = tc->position.X + herissonSpeed * dt;
	}
	//set animation speed
	c->actor.speed = 15*(newPos - tc->position.X)/dt;
	if (c->actor.speed < 1.4) c->actor.speed = 1.4;
	if (c->actor.speed > 4.5) c->actor.speed = 4.5;

	tc->position.X = newPos;

	if (herissonSpeed > 0) {
		switchAnim(c);
	} else {
		RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[1]);
	}
	uiHelper.update(dt);
}

