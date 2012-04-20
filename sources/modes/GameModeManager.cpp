#include "modes/GameModeManager.h"
#include "TwitchSystem.h"
#include <fstream>

struct GameModeManager::Actor {
	Entity e;
	float speed;
	bool visible;
};
struct GameModeManager::AnimatedActor {
	int frames, ind;
	std::vector<std::string> anim;
	struct GameModeManager::Actor actor;
};
void GameModeManager::switchAnim(GameModeManager::AnimatedActor* a) {
	a->frames++;
	if (a->frames>=30/(MathUtil::Abs(a->actor.speed))) {
		RENDERING(a->actor.e)->texture = theRenderingSystem.loadTextureFile(a->anim[a->ind]);
		a->ind++;
		if (a->ind==a->anim.size()) a->ind = 0;
		a->frames=0;
	}
}

float GameModeManager::position(float t, std::vector<Vector2> pts) {
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
	return MathUtil::Lerp(-PlacementHelper::ScreenWidth * 0.5 - TRANSFORM(herisson)->size.X * 0.5,
	PlacementHelper::ScreenWidth * 0.5 + TRANSFORM(herisson)->size.X * 0.5, p);
}
void GameModeManager::LoadHerissonTexture(int type) {
	std::stringstream t;
	c->anim.clear();
	t << "herisson_1_"<<type<<".png";
	c->anim.push_back(t.str());
	t.str("");
	t << "herisson_2_"<<type<<".png";
	c->anim.push_back(t.str());
	t.str("");
	t << "herisson_3_"<<type<<".png";
	c->anim.push_back(t.str());
	t.str("");
	t << "herisson_2_"<<type<<".png";
	c->anim.push_back(t.str());
}

void GameModeManager::SetupCore(int bonus) {
LOGW("%s:%d", __FUNCTION__, __LINE__);
	herisson = theEntityManager.CreateEntity();
	ADD_COMPONENT(herisson, Transformation);
	ADD_COMPONENT(herisson, Rendering);
	TRANSFORM(herisson)->z = DL_Animal;
	TRANSFORM(herisson)->size = Vector2(PlacementHelper::GimpWidthToScreen(142),PlacementHelper::GimpHeightToScreen(116));
	c = new GameModeManager::AnimatedActor();
	c->actor.e = herisson;
	c->frames=0;
	c->actor.speed = 4.1;
	LOGW("%s:%d", __FUNCTION__, __LINE__);
	LoadHerissonTexture(bonus+1);
	LOGW("%s:%d", __FUNCTION__, __LINE__);
	ResetCore(bonus);
	LOGW("%s:%d", __FUNCTION__, __LINE__);
	fillVec();
	LOGW("%s:%d", __FUNCTION__, __LINE__);
	
}
void GameModeManager::ResetCore(int bonus) {
	distance = 0.f;
	LOGW("%s:%d", __FUNCTION__, __LINE__);
	generateLeaves(6);
	LOGW("%s:%d", __FUNCTION__, __LINE__);
	TRANSFORM(herisson)->position = Vector2(-PlacementHelper::ScreenWidth * 0.5 - TRANSFORM(herisson)->size.X * 0.5, PlacementHelper::GimpYToScreen(1100));
	RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[0]);
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

			TRANSFORM(e)->z = DL_Hud+(i+1)*(j+1)/100.;
			BranchLeaf bl;
			bl.e = e;
			bl.type=j+1;
			branchLeaves.push_back(bl);
		}
	}
	//swapper.swap(posBranch);
	uiHelper.build();
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

void GameModeManager::UpdateCore(float dt, float obj, float herissonSpeed) {
	// default herisson behavior: move to
	TransformationComponent* tc = TRANSFORM(herisson);
	float newPos = tc->position.X;
	if (herissonSpeed == 0) {
		float targetPosX = GameModeManager::position(obj, pts);
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
	tc->position.X = newPos;
	
	if (herissonSpeed > 0) {
		switchAnim(c);
	} else {	
		RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[1]);
	}
	uiHelper.update(dt);
}

void GameModeManager::HideUICore(bool toHide) {
	if (herisson && RENDERING(herisson)) RENDERING(herisson)->hide = toHide;
	for (int i=0;i<branchLeaves.size();i++) RENDERING(branchLeaves[i].e)->hide = toHide;

	if (toHide)
		uiHelper.hide();
	else
		uiHelper.show();
}
