#include "StaticTimeModeManager.h"
#include "Game.h"

class StaticTimeGameModeManager::HUDManagerData {
	public:
		HUDManagerData() {
			frames = 0;
			nextfps = FCRR;
			fps = 60;

			eScore = theTextRenderingSystem.CreateLocalEntity(10);
			eTime = theTextRenderingSystem.CreateLocalEntity(10);
			eFPS = theTextRenderingSystem.CreateLocalEntity(10);

			TRANSFORM(eScore)->position = Vector2(5, 7);
			TRANSFORM(eTime)->position = Vector2(0, 7);
			TRANSFORM(eFPS)->position = Vector2(-2.5, 8);

			TRANSFORM(eScore)->z = DL_Hud;
			TRANSFORM(eTime)->z = DL_Hud;
			TRANSFORM(eFPS)->z = DL_Hud;

			fBonus = theEntityManager.CreateEntity();


			ADD_COMPONENT(fBonus, Transformation);
			ADD_COMPONENT(fBonus, Rendering);
			RENDERING(fBonus)->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureName(0));

			TRANSFORM(fBonus)->size = Vector2(2,2);
			TRANSFORM(fBonus)->position = Vector2(2,6);
			TRANSFORM(fBonus)->rotation = -.8;


			TEXT_RENDERING(eFPS)->charSize /= 2;
			TEXT_RENDERING(eFPS)->color = Color(0.1, 0.5, 0.4);

			eAbort = theTextRenderingSystem.CreateLocalEntity(6);
			TRANSFORM(eAbort)->z = DL_CombinationMark;
			TRANSFORM(eAbort)->position = Vector2(2, -5);
			TEXT_RENDERING(eAbort)->hide = true;
			TEXT_RENDERING(eAbort)->alignL = false;
			TEXT_RENDERING(eAbort)->text = "Abandonner";
			bAbort = theEntityManager.CreateEntity();
			TEXT_RENDERING(eAbort)->color = Color(0.f,0.f,0.f);
			ADD_COMPONENT(bAbort, Transformation);
			ADD_COMPONENT(bAbort, Container);
			ADD_COMPONENT(bAbort, Button);
			ADD_COMPONENT(bAbort, Rendering);
			BUTTON(bAbort)->clicked = false;
			RENDERING(bAbort)->color = Color(.0, 1.0, .0, .5);
			TRANSFORM(bAbort)->z = DL_CombinationMark;
			CONTAINER(bAbort)->includeChildren = true;
			CONTAINER(bAbort)->entities.push_back(eAbort);
		}
		~HUDManagerData() {
			theTextRenderingSystem.DestroyLocalEntity(eScore);
			theTextRenderingSystem.DestroyLocalEntity(eTime);
			theTextRenderingSystem.DestroyLocalEntity(eFPS);
			theTextRenderingSystem.DestroyLocalEntity(eAbort);
			theTextRenderingSystem.DestroyLocalEntity(bAbort);
			theEntityManager.DeleteEntity(fBonus);
		}
		Entity eScore, eTime, eFPS, fBonus;
		Entity eAbort, bAbort;
		int frames;
		float nextfps, fps;
};

StaticTimeGameModeManager::StaticTimeGameModeManager() {
	limit = 45.0;
	time = 0.;
	datas=0;
	abort=false;

	score=0;
	isReadyToStart = false;
	bonus = MathUtil::RandomInt(8);

}

StaticTimeGameModeManager::~StaticTimeGameModeManager() {
	delete datas;
}

void StaticTimeGameModeManager::Setup() {
	datas = new HUDManagerData();
	HideUI(true);
}



bool StaticTimeGameModeManager::Update(float dt) {
	//on met à jour le temps si on est dans userinput
	//if (game.state(UserInput)) time += dt;

	//a changer
	time+=dt;
	return (limit - time <0);
}

void StaticTimeGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		score += 10*2*nb*nb*nb/6;
	else
		score += 10*nb*nb*nb/6;
}

void StaticTimeGameModeManager::LevelUp() {
}

bool StaticTimeGameModeManager::LeveledUp() {
	return false;
}

void StaticTimeGameModeManager::Reset() {
	time = 0;
	score = 0;
	abort = false;

	isReadyToStart = false;
	bonus = MathUtil::RandomInt(8);
}


void StaticTimeGameModeManager::HideUI(bool toHide) {
	if (datas) {
		TEXT_RENDERING(datas->eAbort)->hide = toHide;
		RENDERING(datas->bAbort)->hide = toHide;
		TEXT_RENDERING(datas->eScore)->hide = toHide;
		TEXT_RENDERING(datas->eTime)->hide = toHide;
		TEXT_RENDERING(datas->eFPS)->hide = toHide;
		RENDERING(datas->fBonus)->hide = toHide;
	}
}

void StaticTimeGameModeManager::UpdateUI(float dt) {
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
	//if (state == UserInput)
	//TEXT_RENDERING(datas->eTime)->color = Color(1.0f,0.f,0.f,1.f);
	//else
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

	//Feuille Bonus
	{
	int type = bonus;
	RenderingComponent* rc = RENDERING(datas->fBonus);
	rc->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureName(type));
	}

	abort = BUTTON(datas->bAbort)->clicked;
	BUTTON(datas->bAbort)->clicked = false;
}

std::string StaticTimeGameModeManager::finalScore() {
	std::stringstream a;
	a << score;
	return a.str();
}

