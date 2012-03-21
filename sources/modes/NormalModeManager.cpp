#include "NormalModeManager.h"


class NormalGameModeManager::HUDManagerData {
	public:
		HUDManagerData() {
				frames = 0;
				nextfps = FCRR;
				fps = 60;
		}
		~HUDManagerData() {
			theTextRenderingSystem.DestroyLocalEntity(eScore);
			theTextRenderingSystem.DestroyLocalEntity(eTime);
			theTextRenderingSystem.DestroyLocalEntity(eLevel);
			theTextRenderingSystem.DestroyLocalEntity(eFPS);
			theEntityManager.DeleteEntity(fBonus);
			for(int i=0; i<8; i++) {
				theTextRenderingSystem.DestroyLocalEntity(eObj[i]);
				theEntityManager.DeleteEntity(fObj[i]);
			}
		}
		Entity eScore, eTime, eLevel, eFPS, eObj[8],fBonus, fObj[8];
		int frames;
		float nextfps, fps;
};

NormalGameModeManager::NormalGameModeManager() {
	limit = 45.0;
	time = 0.;


	score=0;
	levelUp = false;
	isReadyToStart = false;
	level = 1;
	bonus = MathUtil::RandomInt(8);
	for (int i=0;i<50;i++)
		obj[i]=3+i;

	for (int i=0; i<8;i++)
		remain[i]=obj[0];
	datas = new HUDManagerData();
	Setup();
}

NormalGameModeManager::~NormalGameModeManager() {
	delete datas;
}

void NormalGameModeManager::Setup() {
	datas->eScore = theTextRenderingSystem.CreateLocalEntity(10);
	datas->eTime = theTextRenderingSystem.CreateLocalEntity(10);
	datas->eFPS = theTextRenderingSystem.CreateLocalEntity(10);
	datas->eLevel = theTextRenderingSystem.CreateLocalEntity(10);

	TRANSFORM(datas->eLevel)->position = Vector2(5, 8);
	TRANSFORM(datas->eScore)->position = Vector2(5, 7);
	TRANSFORM(datas->eTime)->position = Vector2(0, 7);
	TRANSFORM(datas->eFPS)->position = Vector2(-2.5, 8);

	TRANSFORM(datas->eLevel)->z = 6;
	TRANSFORM(datas->eScore)->z = 6;
	TRANSFORM(datas->eTime)->z = 6;
	TRANSFORM(datas->eFPS)->z = 6;

	for (int i=0;i<8;i++) {
		datas->eObj[i] = theTextRenderingSystem.CreateLocalEntity(5);
		TRANSFORM(datas->eObj[i])->position = Vector2(i-3.5,-6);
		TRANSFORM(datas->eObj[i])->z = 6;
		TEXT_RENDERING(datas->eObj[i])->charSize /= 2;
		TEXT_RENDERING(datas->eObj[i])->color = Color(0., 0., 0.);

		datas->fObj[i] = theEntityManager.CreateEntity();
		ADD_COMPONENT(datas->fObj[i], Transformation);
		ADD_COMPONENT(datas->fObj[i], Rendering);
		TRANSFORM(datas->fObj[i])->z = 5;
		TRANSFORM(datas->fObj[i])->size = Vector2(1,1);
		TRANSFORM(datas->fObj[i])->position = TRANSFORM(datas->eObj[i])->position+Vector2(-0.3,0);
		RENDERING(datas->fObj[i])->bottomLeftUV = Vector2(i / 8.0, 0);
		RENDERING(datas->fObj[i])->topRightUV = RENDERING(datas->fObj[i])->bottomLeftUV + Vector2(1 / 8.0, 1);
		RENDERING(datas->fObj[i])->texture = theRenderingSystem.loadTextureFile("feuilles.png");

	}

	datas->fBonus = theEntityManager.CreateEntity();


	ADD_COMPONENT(datas->fBonus, Transformation);
	ADD_COMPONENT(datas->fBonus, Rendering);
	RENDERING(datas->fBonus)->texture = theRenderingSystem.loadTextureFile("feuilles.png");

	TRANSFORM(datas->fBonus)->size = Vector2(2,2);
	TRANSFORM(datas->fBonus)->position = Vector2(2,6);
	TRANSFORM(datas->fBonus)->rotation = -.8;


	TEXT_RENDERING(datas->eFPS)->charSize /= 2;
	TEXT_RENDERING(datas->eFPS)->color = Color(0.1, 0.5, 0.4);

	HideUI(true);
}



bool NormalGameModeManager::Update(float dt) {
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

int NormalGameModeManager::GetBonus() {
	return bonus;
}

int NormalGameModeManager::GetRemain(int type) {
	return remain[type];
}

int NormalGameModeManager::GetObj() {
	return obj[level-1];
}

int NormalGameModeManager::GetLevel() {
	return level;
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
	TEXT_RENDERING(datas->eScore)->hide = toHide;
	TEXT_RENDERING(datas->eTime)->hide = toHide;
	TEXT_RENDERING(datas->eFPS)->hide = toHide;
	TEXT_RENDERING(datas->eLevel)->hide = toHide;
	RENDERING(datas->fBonus)->hide = toHide;
	for (int i=0;i<8;i++) {
		TEXT_RENDERING(datas->eObj[i])->hide = toHide;
		RENDERING(datas->fObj[i])->hide = toHide;
	}
}

void NormalGameModeManager::UpdateUI(float dt, GameState state) {
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
	int time = limit - time;
	int minute = time/60;
	int seconde= time%60;

	a << minute << ":" << std::setw(2) << std::setfill('0') << seconde << " s";
	TEXT_RENDERING(datas->eTime)->text = a.str();
	//if (state == UserInput)
	if (true)
	TEXT_RENDERING(datas->eTime)->color = Color(1.0f,0.f,0.f,1.f);
	else  TEXT_RENDERING(datas->eTime)->color = Color(1.0f,1.f,1.f,1.f);
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
	a << "Lvl : "<<GetLevel();
	TEXT_RENDERING(datas->eLevel)->text = a.str();
	}
	//Objectifs
	for (int i=0;i<8;i++)
	{
	std::stringstream a;
	a << GetRemain(i);
	TEXT_RENDERING(datas->eObj[i])->text = a.str();
	}
	//Feuille Bonus
	{
	int type = GetBonus();
	RenderingComponent* rc = RENDERING(datas->fBonus);
	rc->bottomLeftUV = Vector2(type / 8.0, 0);
	rc->topRightUV = rc->bottomLeftUV + Vector2(1 / 8.0, 1);
	}
}
