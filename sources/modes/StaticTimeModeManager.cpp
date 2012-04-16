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
			RENDERING(fBonus)->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(0, 0));
			TRANSFORM(fBonus)->size = Vector2(2,2);
			TRANSFORM(fBonus)->position = Vector2(2,6);
			TRANSFORM(fBonus)->rotation = -.8;
			TRANSFORM(fBonus)->z = DL_Hud;


			TEXT_RENDERING(eFPS)->charSize /= 2;
			TEXT_RENDERING(eFPS)->color = Color(0.1, 0.5, 0.4);
		}
		~HUDManagerData() {
			theTextRenderingSystem.DestroyLocalEntity(eScore);
			theTextRenderingSystem.DestroyLocalEntity(eTime);
			theTextRenderingSystem.DestroyLocalEntity(eFPS);
			theEntityManager.DeleteEntity(fBonus);
		}
		Entity eScore, eTime, eFPS, fBonus;
		int frames;
		float nextfps, fps;
};

StaticTimeGameModeManager::StaticTimeGameModeManager() {
	limit = 45.0;
	time = 0.;
	datas=0;
	points=0;
	bonus = MathUtil::RandomInt(8);

	pts.push_back(Vector2(0,0));
	pts.push_back(Vector2(15,0.125));
	pts.push_back(Vector2(25,0.25));
	pts.push_back(Vector2(35,0.5));
	pts.push_back(Vector2(45,1));
}

StaticTimeGameModeManager::~StaticTimeGameModeManager() {
	delete datas;
}

void StaticTimeGameModeManager::Setup() {
	datas = new HUDManagerData();
	SetupCore();
	generateLeaves(6);
	HideUI(true);
}



bool StaticTimeGameModeManager::Update(float dt) {
	time+=dt;
	return (limit - time <0);
}

void StaticTimeGameModeManager::ScoreCalc(int nb, int type) {
	if (type == bonus)
		points += 10*2*nb*nb*nb/6;
	else
		points += 10*nb*nb*nb/6;
}

void StaticTimeGameModeManager::LevelUp() {
}

bool StaticTimeGameModeManager::LeveledUp() {
	return false;
}

void StaticTimeGameModeManager::Reset() {
	time = 0;
	points = 0;
	bonus = MathUtil::RandomInt(8);
}


void StaticTimeGameModeManager::HideUI(bool toHide) {
	if (datas) {
		TEXT_RENDERING(datas->eScore)->hide = toHide;
		TEXT_RENDERING(datas->eTime)->hide = toHide;
		TEXT_RENDERING(datas->eFPS)->hide = toHide;
		RENDERING(datas->fBonus)->hide = toHide;
	}
	HideUICore(toHide);
}

void StaticTimeGameModeManager::UpdateUI(float dt) {
	//Score
	{
	std::stringstream a;
	a.precision(0);
	a << std::fixed << points;
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
	rc->texture = theRenderingSystem.loadTextureFile(Game::cellTypeToTextureNameAndRotation(type, 0));
	}
	//Hérisson
	UpdateCore(dt);
	TRANSFORM(herisson)->position.X = -5.5+11*GameModeManager::position(time, pts);
}

GameMode StaticTimeGameModeManager::GetMode() {
	return StaticTime;
}
