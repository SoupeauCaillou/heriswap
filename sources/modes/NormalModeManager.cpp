#include "NormalModeManager.h"
#include "Game.h"
#define CAMERASPEED -1.f
#include <base/Vector2.h>
#include <base/MathUtil.h>
#include <vector>


struct Actor {
	Entity e;
	float speed;
	bool visible;
};
struct AnimatedActor {
	int frames, ind;
	std::vector<std::string> anim;
	struct Actor actor;
};
void switchAnim(AnimatedActor* a) {
	a->frames++;
	if (a->frames>=30/(MathUtil::Abs(a->actor.speed)+MathUtil::Abs(CAMERASPEED))) {
		RENDERING(a->actor.e)->texture = theRenderingSystem.loadTextureFile(a->anim[a->ind]);
		a->ind++;
		if (a->ind==a->anim.size()) a->ind = 0;
		a->frames=0;
	}
}

float position(float t) {
	//may take std::vector<Vector2> pts, in args
	std::vector<Vector2> pts;
	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(15,0.125));
	pts.push_back(Vector2(25,0.25));
	pts.push_back(Vector2(35,0.5));
	pts.push_back(Vector2(45,1));

	if (t<pts[0].X) return pts[0].Y;
	for (int i = 0; i<pts.size()-1; i++) {
		if (t>pts[i].X && t<pts[i+1].X) {
			return MathUtil::Lerp(pts[i].Y, pts[i+1].Y, (t-pts[i].X)/(pts[i+1].X-pts[i].X));
		}
	}
	return pts[pts.size()-1].Y;
}


class NormalGameModeManager::HUDManagerData {
	public:
		HUDManagerData() {
			frames = 0;
			nextfps = FCRR;
			fps = 60;

			eScore = theTextRenderingSystem.CreateLocalEntity(10);
			eTime = theTextRenderingSystem.CreateLocalEntity(10);
			eFPS = theTextRenderingSystem.CreateLocalEntity(10);
			eLevel = theTextRenderingSystem.CreateLocalEntity(10);

			TRANSFORM(eLevel)->position = Vector2(5, 8);
			TRANSFORM(eScore)->position = Vector2(5, 7);
			TRANSFORM(eTime)->position = Vector2(0, 7);
			TRANSFORM(eFPS)->position = Vector2(-2.5, 8);

			TRANSFORM(eLevel)->z = DL_Hud;
			TRANSFORM(eScore)->z = DL_Hud;
			TRANSFORM(eTime)->z = DL_Hud;
			TRANSFORM(eFPS)->z = DL_Hud;

			for (int i=0;i<8;i++) {
				eObj[i] = theTextRenderingSystem.CreateLocalEntity(5);
				TRANSFORM(eObj[i])->position = Vector2(i-3.5,-6);
				TRANSFORM(eObj[i])->z = DL_Hud;
				TEXT_RENDERING(eObj[i])->charSize /= 2;
				TEXT_RENDERING(eObj[i])->color = Color(0., 0., 0.);

				fObj[i] = theEntityManager.CreateEntity();
				ADD_COMPONENT(fObj[i], Transformation);
				ADD_COMPONENT(fObj[i], Rendering);
				TRANSFORM(fObj[i])->z = DL_Hud-0.001;
				TRANSFORM(fObj[i])->size = Vector2(1,1);
				TRANSFORM(fObj[i])->position = TRANSFORM(eObj[i])->position+Vector2(-0.3,0);
				RENDERING(fObj[i])->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(i, &TRANSFORM(fObj[i])->rotation));
			}

			fBonus = theEntityManager.CreateEntity();

			ADD_COMPONENT(fBonus, Transformation);
			ADD_COMPONENT(fBonus, Rendering);
			RENDERING(fBonus)->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(0, 0));

			TRANSFORM(fBonus)->size = Vector2(2,2);
			TRANSFORM(fBonus)->position = Vector2(2,6);
			TRANSFORM(fBonus)->rotation = -.8;


			TEXT_RENDERING(eFPS)->charSize /= 2;
			TEXT_RENDERING(eFPS)->color = Color(0.1, 0.5, 0.4);

			herisson = theEntityManager.CreateEntity();
			ADD_COMPONENT(herisson, Transformation);
			ADD_COMPONENT(herisson, Rendering);
			TRANSFORM(herisson)->z = DL_Animal;
			TRANSFORM(herisson)->size = Vector2(1,1);
			TRANSFORM(herisson)->position = Vector2(-5,-7);

			c = new AnimatedActor();
			c->actor.e = herisson;
			c->anim.clear();
			c->frames=0;
			c->anim.push_back("herisson_1_1.png");
			c->anim.push_back("herisson_2_1.png");
			c->anim.push_back("herisson_3_1.png");
			c->actor.speed = CAMERASPEED+1.1;
			RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[0]);
			RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[0]);
		}
		~HUDManagerData() {
			theTextRenderingSystem.DestroyLocalEntity(eScore);
			theTextRenderingSystem.DestroyLocalEntity(eTime);
			theTextRenderingSystem.DestroyLocalEntity(eLevel);
			theTextRenderingSystem.DestroyLocalEntity(eFPS);
			theEntityManager.DeleteEntity(fBonus);
			theEntityManager.DeleteEntity(herisson);
			for(int i=0; i<8; i++) {
				theTextRenderingSystem.DestroyLocalEntity(eObj[i]);
				theEntityManager.DeleteEntity(fObj[i]);
			}
		}
		AnimatedActor* c;
		Entity eScore, eTime, eLevel, eFPS, eObj[8],fBonus, fObj[8], herisson;
		int frames;
		float nextfps, fps;
};

NormalGameModeManager::NormalGameModeManager() {
	limit = 45.0;
	time = 0.;
	datas=0;

	score=0;
	levelUp = false;
	isReadyToStart = false;
	level = 1;
	bonus = MathUtil::RandomInt(8);
	for (int i=0;i<50;i++)
		obj[i]=3+i;

	for (int i=0; i<8;i++)
		remain[i]=obj[0];
}

NormalGameModeManager::~NormalGameModeManager() {
	delete datas;
}

void NormalGameModeManager::Setup() {
	datas = new HUDManagerData();
	HideUI(true);
}



bool NormalGameModeManager::Update(float dt) {
	//on met à jour le temps si on est dans userinput
	//if (game.state(UserInput)) time += dt;

	//a changer
	time+=dt;
	LevelUp();
	return (limit - time <0);
}

void NormalGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		score += 10*level*2*nb*nb*nb/6;
	else
		score += 10*level*nb*nb*nb/6;

	remain[type] -= nb;
	time -= nb/4;
	if (time < 0)
		time = 0;

	if (remain[type]<0)
		remain[type]=0;
}

void NormalGameModeManager::LevelUp() {
	int match = 1, i=0;
	while (match && i<8) {
		if (remain[i] != 0)
			match=0;
		i++;
	}
	//si on a tous les objectifs
	if (match) {
		level++;
		levelUp = true;
		time -= 20;
		if (time < 0)
			time = 0;
		std::cout << "Level up to level " << level << std::endl;
		bonus = MathUtil::RandomInt(8);
		for (int i=0;i<8;i++)
			remain[i] = obj[level-1];
	}
}

bool NormalGameModeManager::LeveledUp() {
	bool bid = levelUp;
	levelUp = false;
	return bid;
}

void NormalGameModeManager::Reset() {
	time = 0;
	score = 0;

	isReadyToStart = false;
	level = 1;
	bonus = MathUtil::RandomInt(8);

	for (int i=0;i<8;i++) remain[i]=obj[0];

}


void NormalGameModeManager::HideUI(bool toHide) {
	if (datas) {
		TEXT_RENDERING(datas->eScore)->hide = toHide;
		TEXT_RENDERING(datas->eTime)->hide = toHide;
		TEXT_RENDERING(datas->eFPS)->hide = toHide;
		TEXT_RENDERING(datas->eLevel)->hide = toHide;
		RENDERING(datas->fBonus)->hide = toHide;
		RENDERING(datas->herisson)->hide = toHide;
		for (int i=0;i<8;i++) {
			TEXT_RENDERING(datas->eObj[i])->hide = toHide;
			RENDERING(datas->fObj[i])->hide = toHide;
		}
	}
}

void NormalGameModeManager::UpdateUI(float dt) {
	//Score
	{
	std::stringstream a;
	a.precision(0);
	a << std::fixed << score;
	TEXT_RENDERING(datas->eScore)->text = a.str();
	}
	//Temps
	{
	std::stringstream a;
	int timeA = limit - time;
	int minute = timeA/60;
	int seconde= timeA%60;

	a << minute << ":" << std::setw(2) << std::setfill('0') << seconde << " s";
	TEXT_RENDERING(datas->eTime)->text = a.str();
	TEXT_RENDERING(datas->eTime)->color = Color(1.0f,1.f,1.f,1.f);
	}
	//FPS
	{
	std::stringstream a;
	datas->nextfps-=dt;
	datas->frames++;

	if (datas->nextfps<0) {
		datas->fps = datas->frames/FCRR;
		datas->nextfps = FCRR;
		datas->frames = 0;
	}
	a << "FPS : " << datas->fps;
	TEXT_RENDERING(datas->eFPS)->text = a.str();
	}

	//Level
	{
	std::stringstream a;
	a << "Lvl : "<<level;
	TEXT_RENDERING(datas->eLevel)->text = a.str();
	}
	//Objectifs
	for (int i=0;i<8;i++)
	{
		std::stringstream a;
		a << remain[i];
		TEXT_RENDERING(datas->eObj[i])->text = a.str();
	}
	//Feuille Bonus
	{
	int type = bonus;
	RenderingComponent* rc = RENDERING(datas->fBonus);
	rc->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(type, 0));
	}
	//Hérisson
	TRANSFORM(datas->c->actor.e)->position.X = -5.5+11*position(time);
	switchAnim(datas->c);
}

std::string NormalGameModeManager::finalScore() {
	std::stringstream a;
	a << score;
	return a.str();
}

GameMode NormalGameModeManager::GetMode() {
	return Normal;
}
