#include "modes/GameModeManager.h"

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
void switchAnim(GameModeManager::AnimatedActor* a) {
	a->frames++;
	if (a->frames>=30/(MathUtil::Abs(a->actor.speed))) {
		RENDERING(a->actor.e)->texture = theRenderingSystem.loadTextureFile(a->anim[a->ind]);
		a->ind++;
		if (a->ind==a->anim.size()) a->ind = 0;
		a->frames=0;
	}
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
Vector2 GameModeManager::placeOnBranch() {
	Vector2 res = Vector2::Zero; //la premiere feuille sera TOUJOURS celle ci;
	float minDis = 0;
	for (int i=0; i<10; i++) {
		Vector2 p = MathUtil::RandomVector(Vector2(-5,6),Vector2(5,8));
		float minDisForThis = 10000;
		for (int j=0; j<branchLeaves.size(); j++) {
			float d = (TRANSFORM(branchLeaves[j].e)->position.X - p.X)*(TRANSFORM(branchLeaves[j].e)->position.X - p.X)+
						(TRANSFORM(branchLeaves[j].e)->position.Y - p.Y)*(TRANSFORM(branchLeaves[j].e)->position.Y - p.Y);//norm2 x²+y²
			if (d < minDisForThis)
				minDisForThis = d;
		}
		if (minDisForThis>minDis) {
			res = p;
			minDis = minDisForThis;
		}
	}
	return res;
}

void GameModeManager::SetupCore() {
	herisson = theEntityManager.CreateEntity();
	ADD_COMPONENT(herisson, Transformation);
	ADD_COMPONENT(herisson, Rendering);
	TRANSFORM(herisson)->z = DL_Animal;
	TRANSFORM(herisson)->size = Vector2(1,1);
	TRANSFORM(herisson)->position = Vector2(-5,-7);

	c = new GameModeManager::AnimatedActor();
	c->actor.e = herisson;
	c->anim.clear();
	c->frames=0;
	c->anim.push_back("herisson_1_1.png");
	c->anim.push_back("herisson_2_1.png");
	c->anim.push_back("herisson_3_1.png");
	c->anim.push_back("herisson_2_1.png");
	c->actor.speed = 4.1;
	RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[0]);
	RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[0]);
}

void GameModeManager::generateLeaves(int nb) {
	for (int i=0;i<nb;i++) {
		for (int j=0;j<8;j++) {
			Entity e = theEntityManager.CreateEntity();
			ADD_COMPONENT(e, Transformation);
			ADD_COMPONENT(e, Rendering);
			RENDERING(e)->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(j, 0));
			TRANSFORM(e)->rotation = MathUtil::RandomFloat(2*MathUtil::Pi);
			TRANSFORM(e)->size = Game::CellSize() * Game::CellContentScale();
			TRANSFORM(e)->position = placeOnBranch();
			TRANSFORM(e)->z = DL_Hud+(i+1)*(j+1)/100.;
			BranchLeaf bl;
			bl.e = e;
			bl.type=j+1;
			branchLeaves.push_back(bl);
		}
	}
	
	uiHelper.build();
}

void GameModeManager::UpdateCore(float dt) {
	switchAnim(c);
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
