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
#include "systems/PhysicsSystem.h"
#include "systems/ParticuleSystem.h"
#include "systems/ScrollingSystem.h"
#include "systems/MorphingSystem.h"

#include "states/GameStateManager.h"
#include "states/SpawnGameStateManager.h"
#include "states/UserInputGameStateManager.h"
#include "states/DeleteGameStateManager.h"
#include "states/FallGameStateManager.h"
#include "states/MainMenuGameStateManager.h"
#include "states/BackgroundManager.h"
#include "states/LevelStateManager.h"
#include "states/PauseStateManager.h"
#include "states/FadeStateManager.h"
#include "states/ModeMenuStateManager.h"
#include "states/LogoStateManager.h"

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"
#include "modes/StaticTimeModeManager.h"
#include "modes/ScoreAttackModeManager.h"

#include "DepthLayer.h"
#include "PlacementHelper.h"
#include "GridSystem.h"
#include "Game.h"
#include "TwitchSystem.h"
#include "Sound.h"

#define GRIDSIZE 8

class Game::Data {
	public:
		/* can not use any system here */
		Data(ScoreStorage* storagee, PlayerNameInputUI* inputUI) {
			mode = Normal;
			mode2Manager[Normal] = new NormalGameModeManager();
			mode2Manager[ScoreAttack] = new ScoreAttackGameModeManager();
			mode2Manager[StaticTime] = new StaticTimeGameModeManager();
			storage = storagee;

			logo = theEntityManager.CreateEntity();
			soundButton = theEntityManager.CreateEntity();

			state = BlackToLogoState;

			state2Manager[Spawn] = new SpawnGameStateManager();
			state2Manager[UserInput] = new UserInputGameStateManager();
			state2Manager[Delete] = new DeleteGameStateManager();
			state2Manager[Fall] = new FallGameStateManager();
			state2Manager[LevelChanged] = new LevelStateManager();
			state2Manager[Pause] = new PauseStateManager();
			state2Manager[Logo] = new LogoStateManager(LogoToBlackState, logo);
			state2Manager[MainMenu] = new MainMenuGameStateManager();
			state2Manager[ModeMenu] = new ModeMenuStateManager(storage,inputUI);

			state2Manager[BlackToLogoState] = new FadeGameStateManager(FadeIn, BlackToLogoState, Logo, state2Manager[Logo]);
			state2Manager[LogoToBlackState] = new FadeGameStateManager(FadeOut, LogoToBlackState, BlackToMainMenu, 0);
			state2Manager[BlackToMainMenu] = new FadeGameStateManager(FadeIn, BlackToMainMenu, MainMenu, state2Manager[MainMenu]);
			state2Manager[ModeMenuToBlackState] = new FadeGameStateManager(FadeOut, ModeMenuToBlackState, BlackToSpawn, 0);
			state2Manager[BlackToSpawn] = new FadeGameStateManager(FadeIn, BlackToSpawn, Spawn, 0);
		}

		void Setup(int windowW, int windowH) {
			BackgroundManager* bg = new BackgroundManager((10.0 * windowH) / windowW);
			bg->cloudStartX = Interval<float>(0.0,15.0);
			state2Manager[Background] = bg;
			ADD_COMPONENT(logo, Rendering);
			ADD_COMPONENT(logo, Transformation);
			TRANSFORM(logo)->position = Vector2(0,0);
			TRANSFORM(logo)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::GimpHeightToScreen(869));
			RENDERING(logo)->hide = false;
			TRANSFORM(logo)->z = DL_Logo;
			RENDERING(logo)->texture = theRenderingSystem.loadTextureFile("soupe_logo.png");
			logo_bg = theEntityManager.CreateEntity();
			ADD_COMPONENT(logo_bg, Rendering);
			ADD_COMPONENT(logo_bg, Transformation);
			TRANSFORM(logo_bg)->position = Vector2(0,0);
			TRANSFORM(logo_bg)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::ScreenHeight);
			RENDERING(logo_bg)->hide = false;
			RENDERING(logo_bg)->color = Color(0,0,0);
			TRANSFORM(logo_bg)->z = DL_BehindLogo;
			ADD_COMPONENT(soundButton, Transformation);
			TRANSFORM(soundButton)->z = DL_MainMenuUITxt;
			TRANSFORM(soundButton)->size = Vector2(PlacementHelper::GimpWidthToScreen(80), PlacementHelper::GimpHeightToScreen(80));
			TransformationSystem::setPosition(TRANSFORM(soundButton), Vector2(PlacementHelper::GimpXToScreen(692), PlacementHelper::GimpYToScreen(1215)), TransformationSystem::W);
			ADD_COMPONENT(soundButton, Button);
			ADD_COMPONENT(soundButton, Rendering);
			if (storage->soundEnable(false)) RENDERING(soundButton)->texture = theRenderingSystem.loadTextureFile("sound_on.png");
			else RENDERING(soundButton)->texture = theRenderingSystem.loadTextureFile("sound_off.png");
			ADD_COMPONENT(soundButton, Sound);
			SOUND(soundButton)->type = SoundComponent::EFFECT;
			RENDERING(soundButton)->hide = false;

			for(std::map<GameState, GameStateManager*>::iterator it=state2Manager.begin(); it!=state2Manager.end(); ++it)
				it->second->Setup();

			for(std::map<GameMode, GameModeManager*>::iterator it=mode2Manager.begin(); it!=mode2Manager.end(); ++it)
				it->second->Setup();

			for (int i=0; i<2; i++) {
				musicStress2[i] = theEntityManager.CreateEntity();
				ADD_COMPONENT(musicStress2[i], Sound);
				SOUND(musicStress2[i])->type = SoundComponent::MUSIC;

				musicStress1[i] = theEntityManager.CreateEntity();
				ADD_COMPONENT(musicStress1[i], Sound);
				SOUND(musicStress1[i])->type = SoundComponent::MUSIC;

				musicMenu[i] = theEntityManager.CreateEntity();
				ADD_COMPONENT(musicMenu[i], Sound);
				SOUND(musicMenu[i])->type = SoundComponent::MUSIC;
			}
			for (int i=0; i<8; i++) {
				music[i] = theEntityManager.CreateEntity();
				ADD_COMPONENT(music[i], Sound);
				SOUND(music[i])->type = SoundComponent::MUSIC;
				SOUND(music[i])->repeat = false;
			}

			canalMenu.timeLoop = 64.;
			canalMenu.musicLength=65.;
			canalMenu.sounds=musicMenu;
			canalMenu.name="audio/musique_menu.ogg";

			canalStress1.sounds=musicStress1;
			canalStress1.name="audio/E.ogg";

			canalStress2.sounds=musicStress2;
			canalStress2.name="audio/F.ogg";

			for (int i=0;i<4;i++) {
				canal[i].sounds=music+2*i;
				canal[i].multipleStrings = true;
			}


			float benchPos = - 4.75;
			benchTotalTime = theEntityManager.CreateEntity();
			ADD_COMPONENT(benchTotalTime, Rendering);
			ADD_COMPONENT(benchTotalTime, Transformation);
			TRANSFORM(benchTotalTime)->position = Vector2(0,benchPos);
			TRANSFORM(benchTotalTime)->size = Vector2(10,0.5);
			TRANSFORM(benchTotalTime)->z = DL_Benchmark;

			std::vector<std::string> allSystems = ComponentSystem::registeredSystemNames();
			for (int i = 0; i < allSystems.size() ; i ++) {
				Entity b = theEntityManager.CreateEntity();
				ADD_COMPONENT(b, Rendering);
				ADD_COMPONENT(b, Transformation);
				TRANSFORM(b)->position = Vector2(0, benchPos);
				TRANSFORM(b)->size = Vector2(.8,0.4);
				TRANSFORM(b)->z = DL_Benchmark;
				RENDERING(b)->color = (i % 2) ? Color(0.1, 0.1, 0.1,0.5):Color(0.8,0.8,0.8,0.5);
				RENDERING(b)->hide = true;
				benchTimeSystem[allSystems[i]] = b;
			}
			bg->cloudStartX = Interval<float>(8.0,15.0);

			MainMenuGameStateManager* mainmenu = static_cast<MainMenuGameStateManager*> (state2Manager[MainMenu]);
			ModeMenuStateManager* modemenu = static_cast<ModeMenuStateManager*> (state2Manager[ModeMenu]);
			modemenu->menufg = mainmenu->menufg;
			modemenu->menubg = mainmenu->menubg;
			modemenu->herisson = mainmenu->herisson;
		}
		//bench data
		std::map<std::string, Entity> benchTimeSystem;
		Entity benchTotalTime, targetTime;

		GameState state, stateBeforePause;
		bool stateBeforePauseNeedEnter;
		Entity logo, logo_bg, sky, decord2nd, decord1er;
		std::map<GameState, GameStateManager*> state2Manager;
		std::map<GameMode, GameModeManager*> mode2Manager;
		ScoreStorage* storage;
		GameMode mode;

		Entity cursor;

		Entity soundButton;
		Canal canalMenu, canalStress1, canalStress2, canal[4];
		Entity musicMenu[2], musicStress1[2], musicStress2[2], music[8];
};

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

static bool fadeLogoState(GameState state) {
	switch (state) {
		case BlackToLogoState:
		case LogoToBlackState:
		case BlackToMainMenu:
		case Logo:
			return true;
		default:
			return false;
	}
}

static const float offset = 0.2;
static const float scale = 0.95;
static const float size = (10 - 2 * offset) / GRIDSIZE;

// grid: [48, 302] -> [752, 1006]  in gimp
Vector2 Game::GridCoordsToPosition(int i, int j) {
	float startX = PlacementHelper::GimpXToScreen(48);
	float startY = PlacementHelper::GimpYToScreen(1006);
	float size = PlacementHelper::GimpWidthToScreen((752 - 48) / 8);

	return Vector2(
		startX + (i + 0.5) * size,
		startY + (j + 0.5) * size);
}

float Game::CellSize() {
	return PlacementHelper::GimpWidthToScreen((752 - 48) / 8);
}

float Game::CellContentScale() {
	return scale;
}

Game::Game(NativeAssetLoader* ploader, ScoreStorage* storage, PlayerNameInputUI* inputUI) {
	this->loader = ploader;

	/* create EntityManager */
	EntityManager::CreateInstance();

	/* create before system so it cannot use any of them (use Setup instead) */
	datas = new Data(storage, inputUI);
	datas->mode2Manager[Normal]->uiHelper.game = this;
	datas->mode2Manager[ScoreAttack]->uiHelper.game = this;
	datas->mode2Manager[StaticTime]->uiHelper.game = this;

	/* create systems singleton */
	TransformationSystem::CreateInstance();
	RenderingSystem::CreateInstance();
	SoundSystem::CreateInstance();
	GridSystem::CreateInstance();
	ADSRSystem::CreateInstance();
	ButtonSystem::CreateInstance();
	TextRenderingSystem::CreateInstance();
	ContainerSystem::CreateInstance();
	PhysicsSystem::CreateInstance();
    ParticuleSystem::CreateInstance();
    ScrollingSystem::CreateInstance();
    MorphingSystem::CreateInstance();
    TwitchSystem::CreateInstance();
}

void Game::loadFont(const std::string& name) {
	char* font = loader->loadShaderFile(name);
	std::stringstream sfont;
	sfont << font;
	std::string line;
	std::map<unsigned char, float> h2wratio;
	while (getline(sfont, line)) {
		if (line[0] == '#')
			continue;
		int c, w, h;
		sscanf(line.c_str(), "%d,%d,%d", &c, &w, &h);
		h2wratio[c] = (float)w / h;
	}
	delete[] font;
	h2wratio[' '] = h2wratio['a'];
	theTextRenderingSystem.registerFont("typo", h2wratio);
}

void Game::init(int windowW, int windowH, const uint8_t* in, int size) {
    PlacementHelper::ScreenHeight = 10;
    PlacementHelper::ScreenWidth = PlacementHelper::ScreenHeight * windowW / (float)windowH;
    PlacementHelper::WindowWidth = windowW;
    PlacementHelper::WindowHeight = windowH;


	theRenderingSystem.setWindowSize(windowW, windowH, PlacementHelper::ScreenWidth, PlacementHelper::ScreenHeight);
	theTouchInputManager.init(Vector2(PlacementHelper::ScreenWidth, PlacementHelper::ScreenHeight), Vector2(windowW, windowH));

	theRenderingSystem.init();

	/*
	theRenderingSystem.loadAtlas("sprites");
	theRenderingSystem.loadAtlas("animals");
	*/
	theRenderingSystem.loadAtlas("alphabet");

	if (in && size) {
		datas->state = Pause;
		loadState(in, size);
	}

	datas->Setup(windowW, windowH);

	if (in && size) {
		RENDERING(datas->logo_bg)->hide = true;
	}

	// init font
	loadFont("typo.desc");

	theGridSystem.GridSize = GRIDSIZE;
	theSoundSystem.mute = !datas->storage->soundEnable(false);

	float bgElementWidth = PlacementHelper::GimpWidthToScreen(800);
	datas->sky = theEntityManager.CreateEntity();
	ADD_COMPONENT(datas->sky, Transformation);
	TRANSFORM(datas->sky)->z = DL_Sky;
	TRANSFORM(datas->sky)->size = Vector2(bgElementWidth, (bgElementWidth * 833.0) / 808.0);
	TransformationSystem::setPosition(TRANSFORM(datas->sky), Vector2(0, PlacementHelper::GimpYToScreen(0)), TransformationSystem::N);
	ADD_COMPONENT(datas->sky, Scrolling);
	SCROLLING(datas->sky)->images.push_back("ciel0.png");
	SCROLLING(datas->sky)->images.push_back("ciel1.png");
	SCROLLING(datas->sky)->images.push_back("ciel2.png");
	SCROLLING(datas->sky)->images.push_back("ciel3.png");
	SCROLLING(datas->sky)->speed = Vector2(-0.3, 0);
	SCROLLING(datas->sky)->displaySize = Vector2(TRANSFORM(datas->sky)->size.X * 1.01, TRANSFORM(datas->sky)->size.Y);
	static_cast<BackgroundManager*> (datas->state2Manager[Background])->skySpeed = -0.3;

	datas->mode2Manager[Normal]->sky = datas->sky;
	static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->smallLevel = datas->mode2Manager[Normal]->uiHelper.smallLevel;

	datas->state2Manager[datas->state]->Enter();
}

void Game::setMode() {
	datas->state2Manager[Delete]->modeMgr = datas->mode2Manager[datas->mode];
	datas->state2Manager[ModeMenu]->modeMgr = datas->mode2Manager[datas->mode];
}

void Game::toggleShowCombi(bool forcedesactivate) {
 #if 0
	static bool activated;
	//on switch le bool
	activated = !activated;
	if (forcedesactivate) activated = false;
	if (datas->state != UserInput) activated = false;
	if (activated) {
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
						if (j) CombinationMark::markCellInCombination(4, Vector2(it->X+i, it->Y));
						else theCombinationMarkSystem.NewMarks(5, Vector2(it->X, it->Y+i));

					}
				}
			}
		}
	} else {
		theCombinationMarkSystem.DeleteMarks(4);
		theCombinationMarkSystem.DeleteMarks(5);
	}
#endif
}

void Game::togglePause(bool activate) {
	if (activate && datas->state != Pause && pausableState(datas->state)) {
		datas->stateBeforePause = datas->state;
		datas->stateBeforePauseNeedEnter = false;
		datas->state = Pause;
		hideEveryThing(true, false);
		datas->state2Manager[datas->state]->Enter();
	} else if (!activate) {
		datas->state2Manager[datas->state]->Exit();
		hideEveryThing(false, false);
		datas->state = datas->stateBeforePause;
		if (datas->stateBeforePauseNeedEnter)
			datas->state2Manager[datas->state]->Enter();
	}
}

void Game::hideEveryThing(bool toHide, bool blacktospawn) {
	if (!toHide) {
		RENDERING(datas->benchTotalTime)->hide = false;
		datas->mode2Manager[datas->mode]->HideUI(false);
		for (std::map<std::string, Entity>::iterator it=datas->benchTimeSystem.begin();
			it != datas->benchTimeSystem.end(); ++it) {
			RENDERING(it->second)->hide = false;
		}
		theGridSystem.HideAll(false);
	} else {
		RENDERING(datas->benchTotalTime)->hide = true;
		datas->mode2Manager[datas->mode]->HideUI(true);
		for (std::map<std::string, Entity>::iterator it=datas->benchTimeSystem.begin();
			it != datas->benchTimeSystem.end(); ++it) {
			RENDERING(it->second)->hide = true;
		}
		if (!blacktospawn)
			theGridSystem.HideAll(true);
	}
}

void Game::tick(float dt) {
	{
		#define COUNT 250
		static int frameCount = 0;
		static float accum = 0, t = 0;
		frameCount++;
		accum += dt;
		if (frameCount == COUNT) {
			//LOGI("%d frames: %.3f s - diff: %.3f s - ms per frame: %.3f", COUNT, accum, TimeUtil::getTime() - t, accum / COUNT);
			t = TimeUtil::getTime();
			accum = 0;
			frameCount = 0;
		}
	}
	float updateDuration = TimeUtil::getTime();
	static bool ended = false;
	float percentDone=0; //ended = (percentDone>=1);
	static float timeMusicLoop = 65.f; //premier lancement

	GameState newState;

	theTouchInputManager.Update(dt);

	//si le chrono est fini, on passe au menu de fin
	if (ended) {
		newState = ModeMenu;
		static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->ended = true;
		ended = false;
	} else {
	//sinon on passe a l'etat suivant
		newState = datas->state2Manager[datas->state]->Update(dt);
	}
	//si on est passé de pause à quelque chose different de pause, on desactive la pause
	if (newState == Unpause) {
		togglePause(false);
	//sinon si on a change d'etat
	} else if (newState != datas->state) {
		if (newState == Abort) {
			LOGI("aborted. going to main menu");
			hideEveryThing(true, false);
			newState = MainMenu;
			static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->ended = false;
		} else if (newState == ModeMenu) {
			datas->mode = (static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu]))->choosenGameMode;
				//reference title into mode menu from main menu
			static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->title = static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu])->eStart[datas->mode-1];
			setMode(); //on met à jour le mode de jeu dans les etats qui en ont besoin
		} else if (newState == BlackToMainMenu) {
			RENDERING(datas->logo)->hide = true;
		} else if (newState == ModeMenuToBlackState) {
			datas->mode2Manager[datas->mode]->HideUI(false);
		}

		datas->state2Manager[datas->state]->Exit();
		datas->state = newState;
		datas->state2Manager[datas->state]->Enter();

		hideEveryThing(!inGameState(newState) && newState != ModeMenuToBlackState , datas->state==BlackToSpawn);
		RENDERING(datas->logo_bg)->hide = RENDERING(datas->logo)->hide;
	}
	//si on appuye sur le bouton pause
	if (BUTTON(datas->soundButton)->clicked) {
		BUTTON(datas->soundButton)->clicked = false;
		datas->storage->soundEnable(true); //on met a jour la table sql
		theSoundSystem.mute = !theSoundSystem.mute;
		if (!theSoundSystem.mute) SOUND(datas->soundButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		if (theSoundSystem.mute) RENDERING(datas->soundButton)->texture = theRenderingSystem.loadTextureFile("sound_off.png");
		else RENDERING(datas->soundButton)->texture = theRenderingSystem.loadTextureFile("sound_on.png");
	}

	//updating time
	if (datas->state == UserInput) {
		percentDone = datas->mode2Manager[datas->mode]->Update(dt);
		ended = (percentDone >= 1);
		//si on change de niveau
		if (datas->mode2Manager[datas->mode]->LeveledUp()) {
			NormalGameModeManager* m = static_cast<NormalGameModeManager*> (datas->mode2Manager[datas->mode]);
			static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->currentLevel = m->currentLevel();

			datas->state2Manager[datas->state]->Exit();
			datas->state = LevelChanged;
			datas->state2Manager[datas->state]->Enter();
		}
	}//updating HUD
	if (inGameState(newState)) {
		datas->mode2Manager[datas->mode]->UpdateUI(dt);
	}

	for(std::map<GameState, GameStateManager*>::iterator it=datas->state2Manager.begin();
		it!=datas->state2Manager.end();
		++it) {
		it->second->BackgroundUpdate(dt);
	}

	// TRANSFORM(datas->cursor)->position = theTouchInputManager.getTouchLastPosition();

	//si c'est pas à l'user de jouer, on cache de force les combi
	if (newState != UserInput)
		toggleShowCombi(true);


	theADSRSystem.Update(dt);
	theGridSystem.Update(dt);
	theButtonSystem.Update(dt);
    theParticuleSystem.Update(dt);

	if (datas->state != newState && newState == ModeMenu) {
		theGridSystem.DeleteAll();
		datas->mode2Manager[datas->mode]->Reset();
	}


	//update music
	if (pausableState(datas->state) && datas->state != LevelChanged) { //si on joue
		datas->canalMenu.pause();
		updateMusic(datas->canal, &datas->canalStress1, &datas->canalStress2, percentDone, dt);
	} else if (!pausableState(datas->state) && !fadeLogoState(datas->state)) { //dans les menus
		for (int i=0;i<4;i++) datas->canal[i].pause();
		datas->canalStress1.pause();
		datas->canalStress2.pause();
		datas->canalMenu.update(dt);
	}

	theMorphingSystem.Update(dt);
    theTwitchSystem.Update(dt);
	thePhysicsSystem.Update(dt);
	theTransformationSystem.Update(dt);
	theScrollingSystem.Update(dt);
	theContainerSystem.Update(dt);
	theTextRenderingSystem.Update(dt);
	theSoundSystem.Update(dt);
	theRenderingSystem.Update(dt);

	//bench settings

	updateDuration = TimeUtil::getTime()-updateDuration;

	bench(true, updateDuration, dt);
}

void Game::bench(bool active, float updateDuration, float dt) {
	if (active) {
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
				float width = 10 * (timeSpent / updateDuration);
				TRANSFORM(it->second)->size.X = width;
				TRANSFORM(it->second)->position.X = x + width * 0.5;
				RENDERING(it->second)->hide = false;
				x += width;

				// LOGI("%s: %.3f s", it->first.c_str(), timeSpent);
			}

			// LOGI("temps passe dans les systemes : %f sur %f total (%f %) (théorique : dt=%f)\n", timeSpentInSystems, updateDuration, 100*timeSpentInSystems/updateDuration, dt);
			benchAccum = 0;
		}
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
	ptr = (uint8_t*)mempcpy(ptr, &datas->mode, sizeof(datas->mode));
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
	memcpy(&datas->mode, &in[index], sizeof(datas->mode));
	in += sizeof(datas->mode);
	datas->mode2Manager[datas->mode]->Setup();
	setMode();
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

static float rotations[] = {
	MathUtil::PiOver4,
	-MathUtil::PiOver2,
	0,
	-3*MathUtil::PiOver4,
	3*MathUtil::PiOver4,
	MathUtil::PiOver2,
	-MathUtil::Pi,
	-MathUtil::PiOver4
};

std::string Game::cellTypeToTextureNameAndRotation(int type, float* rotation) {
	if (rotation)
		*rotation = rotations[type];

	std::stringstream s;
	s << "feuille" << (type+1) << ".png";
	return s.str();
}

float Game::cellTypeToRotation(int type) {
	return rotations[type];
}
