#include <sstream>

#include "base/Log.h"
#include "base/TouchInputManager.h"
#include "base/MathUtil.h"
#include "base/EntityManager.h"
#include "base/TimeUtil.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/ContainerSystem.h"

#include "GridSystem.h"
#include "Game.h"
#include "CombinationMarkSystem.h"

#include "states/GameStateManager.h"
#include "states/SpawnGameStateManager.h"
#include "states/UserInputGameStateManager.h"
#include "states/DeleteGameStateManager.h"
#include "states/FallGameStateManager.h"
#include "states/MainMenuGameStateManager.h"
#include "states/ScoreBoardStateManager.h"
#include "states/EndMenuStateManager.h"
#include "states/BackgroundManager.h"
#include "states/LevelStateManager.h"
#include "states/PauseStateManager.h"
#include "states/FadeStateManager.h"

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"
#include "modes/StaticTimeModeManager.h"
#include "modes/ScoreAttackModeManager.h"

#include "HUDManager.h"

#define GRIDSIZE 8

class Game::Data {
	public:
		/* can not use any system here */
		Data(ScoreStorage* storage) {
			hud = new HUDManager;


			mode = ScoreAttack;
			mode2Manager[Normal] = new NormalGameModeManager();
			mode2Manager[ScoreAttack] = new ScoreAttackGameModeManager();
			mode2Manager[StaticTime] = new StaticTimeGameModeManager();

			logo = theEntityManager.CreateEntity();

			state = BlackToLogoState;

			state2Manager[BlackToLogoState] = new FadeGameStateManager(logo, FadeIn, BlackToLogoState, LogoToBlackState, 1.5);
			state2Manager[LogoToBlackState] = new FadeGameStateManager(logo, FadeOut, LogoToBlackState, BlackToMainMenu);
			//to do : add entity || modif 0 du GameStateToBlack
			//hm, ça implique d'en faire un pour chaque state..
			state2Manager[BlackToMainMenu] = new FadeGameStateManager(0, FadeIn, BlackToMainMenu, MainMenu);
			state2Manager[MainMenuToBlackState] = new FadeGameStateManager(0, FadeOut, MainMenuToBlackState, BlackToSpawn);
			state2Manager[BlackToSpawn] = new FadeGameStateManager(0, FadeIn, BlackToSpawn, Spawn);
			state2Manager[MainMenu] = new MainMenuGameStateManager();

			state2Manager[Spawn] = new SpawnGameStateManager(mode2Manager[mode]);
			state2Manager[UserInput] = new UserInputGameStateManager(mode2Manager[mode]);
			state2Manager[Delete] = new DeleteGameStateManager(mode2Manager[mode]);
			state2Manager[Fall] = new FallGameStateManager();

			state2Manager[LevelChanged] = new LevelStateManager();
			state2Manager[ScoreBoard] = new ScoreBoardStateManager(storage);
			state2Manager[EndMenu] = new EndMenuStateManager(storage, mode2Manager[mode], mode);
			state2Manager[Pause] = new PauseStateManager();

			BackgroundManager* bg = new BackgroundManager();
			bg->xStartRange = Vector2(6, 8);
			bg->yRange = Vector2(-2, 9);
			bg->scaleRange = Vector2(0.4, 1.5);
			state2Manager[Background] = bg;

		}

		void Setup(int windowW, int windowH) {
			ADD_COMPONENT(logo, Rendering);
			ADD_COMPONENT(logo, Transformation);
			TRANSFORM(logo)->position = Vector2(0,0);
			TRANSFORM(logo)->size = Vector2(10,(10.0*windowH)/windowW);
			RENDERING(logo)->hide = true;
			TRANSFORM(logo)->z = 39;
			RENDERING(logo)->texture = theRenderingSystem.loadTextureFile("logo.png");

			hud->Setup(mode2Manager[mode], mode);

			for(std::map<GameState, GameStateManager*>::iterator it=state2Manager.begin(); it!=state2Manager.end(); ++it) {
				it->second->Setup();
			}

			for (int i=0; i<4; i++) {
				music[i] = theEntityManager.CreateEntity();
				ADD_COMPONENT(music[i], Sound);
				SOUND(music[i])->type = SoundComponent::MUSIC;
				SOUND(music[i])->repeat = false;
			}

			float benchPos = - 5.0*windowH/windowW + 0.5;
			benchTotalTime = theEntityManager.CreateEntity();
			ADD_COMPONENT(benchTotalTime, Rendering);
			ADD_COMPONENT(benchTotalTime, Transformation);
			TRANSFORM(benchTotalTime)->position = Vector2(0,benchPos);
			TRANSFORM(benchTotalTime)->size = Vector2(10,1);
			TRANSFORM(benchTotalTime)->z = 39;

			std::vector<std::string> allSystems = ComponentSystem::registeredSystemNames();
			for (int i = 0; i < allSystems.size() ; i ++) {
				Entity b = theEntityManager.CreateEntity();
				ADD_COMPONENT(b, Rendering);
				ADD_COMPONENT(b, Transformation);
				TRANSFORM(b)->position = Vector2(0, benchPos);
				TRANSFORM(b)->size = Vector2(.8,0.8);
				TRANSFORM(b)->z = 40;
				RENDERING(b)->color = (i % 2) ? Color(0.1, 0.1, 0.1):Color(0.8,0.8,0.8);
				benchTimeSystem[allSystems[i]] = b;
			}
		}
		//bench data
		std::map<std::string, Entity> benchTimeSystem;
		Entity benchTotalTime, targetTime;

		GameState state, stateBeforePause;
		bool stateBeforePauseNeedEnter;
		Entity logo, background, sky;
		Entity music[4];
		// drag/drop
		HUDManager* hud;
		std::map<GameState, GameStateManager*> state2Manager;
		std::map<GameMode, GameModeManager*> mode2Manager;


		GameMode mode;
};

static const float offset = 0.2;
static const float scale = 0.95;
static const float size = (10 - 2 * offset) / GRIDSIZE;

static bool inGameState(GameState state) {
	switch (state) {
		case Spawn:
		case UserInput:
		case Delete:
		case Fall:
			return true;
		default:
			return false;
	}
}

static bool pausableState(GameState state) {
	switch (state) {
		case Spawn:
		case UserInput:
		case Delete:
		case Fall:
		case LevelChanged:
		case Pause:
			return true;
		default:
			return false;
	}
}

Vector2 Game::GridCoordsToPosition(int i, int j) {
	return Vector2(
		-5 + (i + 0.5) * size + offset,
		-5 + (j + 0.5)*size + offset);
}

float Game::CellSize() {
	return size;
}

float Game::CellContentScale() {
	return scale;
}

Game::Game(ScoreStorage* storage) {
	/* create EntityManager */
	EntityManager::CreateInstance();

	/* create before system so it cannot use any of them (use Setup instead) */
	datas = new Data(storage);

	/* create systems singleton */
	TransformationSystem::CreateInstance();
	RenderingSystem::CreateInstance();
	SoundSystem::CreateInstance();
	GridSystem::CreateInstance();
	CombinationMarkSystem::CreateInstance();
	ADSRSystem::CreateInstance();
	ButtonSystem::CreateInstance();
	TextRenderingSystem::CreateInstance();
	ContainerSystem::CreateInstance();
}

void Game::init(int windowW, int windowH, const uint8_t* in, int size) {
	theRenderingSystem.init();
	theRenderingSystem.setWindowSize(windowW, windowH);

	if (in && size) {
		datas->state = Pause;
		loadState(in, size);
	} else {
		Entity eHUD = theEntityManager.CreateEntity(EntityManager::Persistent);
	}

	datas->Setup(windowW, windowH);

	theGridSystem.GridSize = GRIDSIZE;

	datas->sky = theEntityManager.CreateEntity();
	theTransformationSystem.Add(datas->sky);
	TRANSFORM(datas->sky)->z = -1;
	theRenderingSystem.Add(datas->sky);
	TRANSFORM(datas->sky)->size = Vector2(10, 10.0 * windowH / windowW);
	RENDERING(datas->sky)->texture = theRenderingSystem.loadTextureFile("sky.png");
	RENDERING(datas->sky)->hide = false;

	datas->background = theEntityManager.CreateEntity();
	ADD_COMPONENT(datas->background, Transformation);
	ADD_COMPONENT(datas->background, Rendering);
	TRANSFORM(datas->background)->z = 0;
	TRANSFORM(datas->background)->size = Vector2(10, 10.0 * windowH / windowW);
	RENDERING(datas->background)->texture = theRenderingSystem.loadTextureFile("background.png");
	RENDERING(datas->background)->hide = false;

	datas->state2Manager[datas->state]->Enter();
}

void Game::toggleShowCombi(bool forcedesactivate) {
	static bool activated;
	//on switch le bool
	activated = !activated;
	if (forcedesactivate) activated = false;
	if (datas->state != UserInput) activated = false;
	if (activated) {
		std::cout << "Affiche magique de la triche ! \n" ;
		//j=0 : vertical
		//j=1 : h
		for (int j=0;j<2;j++) {
			std::vector<Vector2> combinaisons;
			if (j) combinaisons = theGridSystem.LookForCombinationsOnSwitchHorizontal();
			else combinaisons = theGridSystem.LookForCombinationsOnSwitchVertical();
			if (!combinaisons.empty())
			{
				for ( std::vector<Vector2>::reverse_iterator it = combinaisons.rbegin(); it != combinaisons.rend(); ++it )
				{
					//rajout de 2 marques sur les elements a swich
					for (int i=0;i<2;i++) {
						if (j) theCombinationMarkSystem.NewMarks(4, Vector2(it->X+i, it->Y));
						else theCombinationMarkSystem.NewMarks(5, Vector2(it->X, it->Y+i));

					}
				}
			}
		}
	} else {
		if (theCombinationMarkSystem.NumberOfThisType(4) || theCombinationMarkSystem.NumberOfThisType(5)) {
			std::cout << "Destruction des marquages et de la triche !\n";
			theCombinationMarkSystem.DeleteMarks(4);
			theCombinationMarkSystem.DeleteMarks(5);
		}
	}
}

static void updateMusic(Entity* music) {
	/* are all started entities done yet ? */
	for (int i=0; i<4; i++) {
		if (SOUND(music[i])->sound != InvalidSoundRef)
			return;
	}

	int count = MathUtil::RandomInt(4) + 1;
	LOGW("starting %d music", count);
	std::list<char> l;
	for (int i=0; i<count; i++) {
		SoundComponent* sc = SOUND(music[i]);
		char c;
		do {
			c = MathUtil::RandomInt('G' - 'A' + 1) + 'A';
		} while (std::find(l.begin(), l.end(), c) != l.end());

		std::stringstream s;
		s << "audio/" << c << ".ogg";
		sc->sound = theSoundSystem.loadSoundFile(s.str(), true);
	}
}

void Game::togglePause(bool activate) {
	if (activate && datas->state != Pause && pausableState(datas->state)) {
		datas->stateBeforePause = datas->state;
		datas->stateBeforePauseNeedEnter = false;
		datas->state = Pause;
		datas->state2Manager[datas->state]->Enter();
	} else if (!activate) {
		datas->state2Manager[datas->state]->Exit();
		datas->state = datas->stateBeforePause;
		if (datas->stateBeforePauseNeedEnter)
			datas->state2Manager[datas->state]->Enter();
	}
}

void Game::tick(float dt) {
	{
		#define COUNT 50
		static int frameCount = 0;
		static float accum = 0, t = 0;
		frameCount++;
		accum += dt;
		if (frameCount == COUNT) {
			LOGI("%d frames: %.3f s - diff: %.3f s - ms per frame: %.3f", COUNT, accum, TimeUtil::getTime() - t, accum / COUNT);
			t = TimeUtil::getTime();
			accum = 0;
			frameCount = 0;
		}
	}

	float updateDuration = TimeUtil::getTime();
	static bool ended = false;

	GameState newState;

	theTouchInputManager.Update(dt);

	//si le chrono est fini, on passe au menu de fin
	if (ended) {
		newState = EndMenu;
		ended = false;
	} else {
	//sinon on passe a l'etat suivant
		newState = datas->state2Manager[datas->state]->Update(dt);
	}
	//si on est passé de pause à quelque chose different de pause, on desactive la pause
	if (newState != datas->state && datas->state == Pause) {
		togglePause(false);
	} else if (newState != datas->state) {
		if (newState == BlackToSpawn) {
			datas->state2Manager[Spawn]->Enter();
		}
		datas->state2Manager[datas->state]->Exit();
		datas->state = newState;
		datas->state2Manager[datas->state]->Enter();

		if (inGameState(newState)) {
			datas->hud->Hide(false);
			theGridSystem.HideAll(false);

			RENDERING(datas->benchTotalTime)->hide = false;
			for (std::map<std::string, Entity>::iterator it=datas->benchTimeSystem.begin();
				it != datas->benchTimeSystem.end(); ++it) {
				RENDERING(it->second)->hide = false;
			}
		} else {
			RENDERING(datas->benchTotalTime)->hide = true;
			for (std::map<std::string, Entity>::iterator it=datas->benchTimeSystem.begin();
				it != datas->benchTimeSystem.end(); ++it) {
				RENDERING(it->second)->hide = true;
			}

			datas->hud->Hide(true);
			if (newState != BlackToSpawn)
				theGridSystem.HideAll(true);
		}
	}

	for(std::map<GameState, GameStateManager*>::iterator it=datas->state2Manager.begin();
		it!=datas->state2Manager.end();
		++it) {
		it->second->BackgroundUpdate(dt);
	}


	//si c'est pas à l'user de jouer, on cache de force les combi
	if (newState != UserInput)
		toggleShowCombi(true);

	theADSRSystem.Update(dt);
	theButtonSystem.Update(dt);

	//si on est ingame, on affiche le HUD
	if (newState == Spawn || newState == UserInput || newState == Delete || newState == Fall || newState == LevelChanged) {
		ended = datas->mode2Manager[datas->mode]->Update(dt);
		datas->hud->Hide(false);
		datas->hud->Update(dt);
		theGridSystem.HideAll(false);
		RENDERING(datas->benchTotalTime)->hide = false;
		for (int i=0;i<9;i++)
			RENDERING(datas->benchTimeSystem[i])->hide = false;
		TEXT_RENDERING(datas->textBenchTimeSystem[0])->hide = false;
		TEXT_RENDERING(datas->textBenchTimeSystem[1])->hide = false;
	} else {
		RENDERING(datas->benchTotalTime)->hide = true;
		for (int i=0;i<9;i++)
			RENDERING(datas->benchTimeSystem[i])->hide = true;
		TEXT_RENDERING(datas->textBenchTimeSystem[0])->hide = true;
		TEXT_RENDERING(datas->textBenchTimeSystem[1])->hide = true;
		datas->hud->Hide(true);
		if (newState != BlackToSpawn)
			theGridSystem.HideAll(true);
	}

	if (newState == EndMenu) {
		theGridSystem.DeleteAll();
		datas->mode2Manager[datas->mode]->time = 0;
	} else if (newState == MainMenu) {
		datas->mode2Manager[datas->mode]->Reset();
	}

	updateMusic(datas->music);

	theCombinationMarkSystem.Update(dt);
	theTransformationSystem.Update(dt);
	theTextRenderingSystem.Update(dt);
	theContainerSystem.Update(dt);
	theRenderingSystem.Update(dt);
	theSoundSystem.Update(dt);

	//bench settings

	updateDuration = TimeUtil::getTime()-updateDuration;

	bench(true, updateDuration, dt);

}

void Game::bench(bool active, float updateDuration, float dt) {
	if (active) {

	float tt = theADSRSystem.timeSpent+theButtonSystem.timeSpent+
		theCombinationMarkSystem.timeSpent+theTransformationSystem.timeSpent+
		theTextRenderingSystem.timeSpent+theContainerSystem.timeSpent+
		theRenderingSystem.timeSpent+theSoundSystem.timeSpent; //total time in systems

	static int azazaz = 0;
	/*azazaz++;
	if (azazaz>=5*60) {
		azazaz=0;
		if (theADSRSystem.timeSpent) LOGI("theADSRSystem:%f",theADSRSystem.timeSpent);
		if (theButtonSystem.timeSpent) LOGI("theButtonSystem:%f", theButtonSystem.timeSpent);
		if (theCombinationMarkSystem.timeSpent) LOGI("theCombinationMarkSystem:%f", theCombinationMarkSystem.timeSpent);
		if (theTransformationSystem.timeSpent) LOGI("theTransformationSystem:%f", 	theTransformationSystem.timeSpent);
		if (theTextRenderingSystem.timeSpent) LOGI("theTextRenderingSystem:%f", 	theTextRenderingSystem.timeSpent);
		if (theContainerSystem.timeSpent) LOGI("theContainerSystem:%f", 	theContainerSystem.timeSpent);
		if (theRenderingSystem.timeSpent) LOGI("theRenderingSystem:%f", 	theRenderingSystem.timeSpent);
		if (theSoundSystem.timeSpent) LOGI("theSoundSystem:%f" ,	theSoundSystem.timeSpent);
		//if (thePlayerSystem.timeSpent) LOGI("thePlayerSystem:%f" ,	thePlayerSystem.timeSpent);

		LOGI("temps passer dans les systemes : %f sur %f total (%f %) (théorique : dt=%f)\n", tt, updateDuration, 100*tt/updateDuration, dt);
	}*/

	RENDERING(datas->benchTotalTime)->color = Color(5.*updateDuration, 5.*(1/5.-updateDuration), 0.3f, .3f);

	char tmp[150] = "\0"; // en attendant mieux (ie savoir cmt on garde que 2 decimales
		//avec l'autre methode
	std::stringstream s;

	for (int i=0; i<NB_SYSTEMS; i++) {
		float r = 0, g = 1;
		char buff[10];

		switch (i) {
			//temps passé en dehors des systemes
			case 0:
				r = (updateDuration-tt) / updateDuration; g = 1 - r;
				s << " out" << r/updateDuration;
				break;
			case 1:
				r = theADSRSystem.timeSpent/ updateDuration; g = 1 - r;
				s << " adsr"<< r/updateDuration;
				break;
			case 2:
				r = theButtonSystem.timeSpent/ updateDuration; g = 1 - r;
				s << "butt"<< r/updateDuration;
				break;
			case 3:
				r = theCombinationMarkSystem.timeSpent/ updateDuration; g = 1 - r;
				s << "comb"<< r/updateDuration;
				break;
			case 4:
				r = theTransformationSystem.timeSpent/ updateDuration; g = 1 - r;
				s << "tran"<< r/updateDuration;
				break;
			case 5:
				r = theTextRenderingSystem.timeSpent/ updateDuration; g = 1 - r;
				s << "txt"<< r/updateDuration;
				break;
			case 6:
				r = theContainerSystem.timeSpent/ updateDuration; g = 1 - r;
				s << "ctn"<< r/updateDuration;
				break;
			case 7:
				r = theRenderingSystem.timeSpent/ updateDuration; g = 1 - r;
				s << "rend"<< r/updateDuration;
				break;
			case 8:
				r = theSoundSystem.timeSpent/ updateDuration; g = 1 - r;
				s << "snd"<< r/updateDuration;
				break;
			case 9:
				//r = thePlayerSystem.timeSpent/ updateDuration; g = 1 - r;
				s << "plr"<< r/updateDuration;
				break;
			default:
				r = 1; g = 0;
				break;

	updateDuration = TimeUtil::getTime() - updateDuration;
	static float benchAccum = 0;
	benchAccum += dt;
	if (benchAccum>=1 && (updateDuration > 0) && !RENDERING(datas->benchTotalTime)->hide) {
		// draw update duration
		if (updateDuration > 1.0/60) {
			RENDERING(datas->benchTotalTime)->color = Color(1.0, 0,0, 1);
		} else {
			RENDERING(datas->benchTotalTime)->color = Color(0.0, 1.0,0,1);
		}
		float frameWidth = MathUtil::Min(updateDuration / (1.f/60), 1.0f) * 10;
		TRANSFORM(datas->benchTotalTime)->size.X = frameWidth;
		TRANSFORM(datas->benchTotalTime)->position.X = -5 + frameWidth * 0.5;

		// for each system adjust rectangle size/position to time spent
		float timeSpentInSystems = 0;
		float x = -5;
		for (std::map<std::string, Entity>::iterator it=datas->benchTimeSystem.begin();
				it != datas->benchTimeSystem.end(); ++it) {
			float timeSpent = ComponentSystem::Named(it->first)->timeSpent;
			timeSpentInSystems += timeSpent;
			float width = frameWidth * (timeSpent / updateDuration);
			TRANSFORM(it->second)->size.X = width;
			TRANSFORM(it->second)->position.X = x + width * 0.5;
			x += width;

			LOGI("%s: %.3f s", it->first.c_str(), timeSpent);
		}

		LOGI("temps passe dans les systemes : %f sur %f total (%f %) (théorique : dt=%f)\n", timeSpentInSystems, updateDuration, 100*timeSpentInSystems/updateDuration, dt);
		benchAccum = 0;
	}
	TEXT_RENDERING(datas->textBenchTimeSystem[0])->text = "out  adsr butt comb tran txt  ctn  rend snd plr";
	TEXT_RENDERING(datas->textBenchTimeSystem[1])->text = tmp;
	//TEXT_RENDERING(datas->textBenchTimeSystem[i])->text = s.str();
	}
}
int Game::saveState(uint8_t** out) {
	bool pausable = pausableState(datas->state);
	if (!pausable) {
		LOGI("Current state is '%d' -> nothing to save", datas->state);
		return 0;
	}

	/* save all entities/components */
	uint8_t* entities = 0;
	int eSize = theEntityManager.serialize(&entities);

	/* save System with assets ? (texture name -> texture ref map of RenderingSystem ?) */
	uint8_t* systems = 0;
	int sSize = theRenderingSystem.saveInternalState(&systems);

	/* save Game fields */
	int finalSize = sizeof(datas->stateBeforePause) + sizeof(eSize) + sizeof(sSize) + eSize + sSize;
	*out = new uint8_t[finalSize];
	uint8_t* ptr = *out;
	if (datas->state == Pause)
		ptr = (uint8_t*)mempcpy(ptr, &datas->stateBeforePause, sizeof(datas->state));
	else
		ptr = (uint8_t*)mempcpy(ptr, &datas->state, sizeof(datas->state));
	ptr = (uint8_t*)mempcpy(ptr, &eSize, sizeof(eSize));
	ptr = (uint8_t*)mempcpy(ptr, &sSize, sizeof(sSize));
	ptr = (uint8_t*)mempcpy(ptr, entities, eSize);
	ptr = (uint8_t*)mempcpy(ptr, systems, sSize);

	LOGI("%d + %d + %d + %d + %d -> %d",
		sizeof(datas->stateBeforePause), sizeof(eSize), sizeof(sSize), eSize, sSize, finalSize);
	return finalSize;
}

void Game::loadState(const uint8_t* in, int size) {
	/* restore Game fields */
	int index = 0;
	memcpy(&datas->stateBeforePause, &in[index], sizeof(datas->stateBeforePause));
	datas->state = Pause;
	datas->stateBeforePauseNeedEnter = true;
	in += sizeof(datas->stateBeforePause);
	int eSize, sSize;
	memcpy(&eSize, &in[index], sizeof(eSize));
	index += sizeof(eSize);
	memcpy(&sSize, &in[index], sizeof(sSize));
	index += sizeof(sSize);
	/* restore entities */
	theEntityManager.deserialize(&in[index], eSize);
	index += eSize;
	/* restore systems */
	theRenderingSystem.restoreInternalState(&in[index], sSize);

	LOGW("RESTORED STATE: %d", datas->stateBeforePause);
}
