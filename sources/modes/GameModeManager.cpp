#include "modes/GameModeManager.h"
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
void GameModeManager::ResetCore() {
	distance = 0.f;
}

float GameModeManager::position(float t, std::vector<Vector2> pts) {
	if (t<=pts[0].X) return pts[0].Y;
	for (int i = 0; i<pts.size()-1; i++) {
		if (t>pts[i].X && t<pts[i+1].X) {
			return MathUtil::Lerp(pts[i].Y, pts[i+1].Y, (t-pts[i].X)/(pts[i+1].X-pts[i].X));
		}
	}
	return pts[pts.size()-1].Y;
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
	herisson = theEntityManager.CreateEntity();
	ADD_COMPONENT(herisson, Transformation);
	ADD_COMPONENT(herisson, Rendering);
	TRANSFORM(herisson)->z = DL_Animal;
	TRANSFORM(herisson)->size = Vector2(PlacementHelper::GimpWidthToScreen(142),PlacementHelper::GimpHeightToScreen(116));
	TRANSFORM(herisson)->position = Vector2(-PlacementHelper::ScreenWidth * 0.5 - TRANSFORM(herisson)->size.X * 0.5, PlacementHelper::GimpYToScreen(1100));
	c = new GameModeManager::AnimatedActor();
	c->actor.e = herisson;
	c->anim.clear();
	c->frames=0;
	LoadHerissonTexture(bonus+1);
	c->actor.speed = 4.1;
	RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[0]);
	RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[0]);
	fillVec();
	generateLeaves(6);
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
}
void GameModeManager::UpdateCore(float dt, float obj) {
	distance = MathUtil::Lerp(-PlacementHelper::ScreenWidth * 0.5 - TRANSFORM(herisson)->size.X * 0.5,
	PlacementHelper::ScreenWidth * 0.5 + TRANSFORM(herisson)->size.X * 0.5, GameModeManager::position(obj, pts)) - TRANSFORM(herisson)->position.X;
	if (distance > 0.f) {
		float vitesse = MathUtil::Max(.1f, distance/(4*dt));
		switchAnim(c);
		TRANSFORM(herisson)->position.X += vitesse*dt;
		distance -= vitesse*dt;
	} else distance = 0;
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
