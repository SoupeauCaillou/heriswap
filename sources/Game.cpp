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

#include "GridSystem.h"
#include "Game.h"
#include "TwitchSystem.h"

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

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"
#include "modes/StaticTimeModeManager.h"
#include "modes/ScoreAttackModeManager.h"

#include "DepthLayer.h"
#include "PlacementHelper.h"

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

			state2Manager[BlackToLogoState] = new FadeGameStateManager(logo, FadeIn, BlackToLogoState, LogoToBlackState, 1.5);
			state2Manager[LogoToBlackState] = new FadeGameStateManager(logo, FadeOut, LogoToBlackState, BlackToMainMenu);
			state2Manager[BlackToMainMenu] = new FadeGameStateManager(0, FadeIn, BlackToMainMenu, MainMenu);
			state2Manager[ModeMenuToBlackState] = new FadeGameStateManager(0, FadeOut, ModeMenuToBlackState, BlackToSpawn);
			state2Manager[BlackToSpawn] = new FadeGameStateManager(0, FadeIn, BlackToSpawn, Spawn);
			state2Manager[MainMenu] = new MainMenuGameStateManager();
			state2Manager[ModeMenu] = new ModeMenuStateManager(storage,inputUI);

			state2Manager[Spawn] = new SpawnGameStateManager();
			state2Manager[UserInput] = new UserInputGameStateManager();
			state2Manager[Delete] = new DeleteGameStateManager();
			state2Manager[Fall] = new FallGameStateManager();
			state2Manager[LevelChanged] = new LevelStateManager();
			state2Manager[Pause] = new PauseStateManager();
		}

		void Setup(int windowW, int windowH) {
			BackgroundManager* bg = new BackgroundManager((10.0 * windowH) / windowW);
			bg->cloudStartX = Interval<float>(8.0,25.0);
			state2Manager[Background] = bg;

			ADD_COMPONENT(logo, Rendering);
			ADD_COMPONENT(logo, Transformation);
			TRANSFORM(logo)->position = Vector2(0,0);
			TRANSFORM(logo)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::ScreenWidth);
			RENDERING(logo)->hide = true;
			TRANSFORM(logo)->z = DL_Logo;
			RENDERING(logo)->texture = theRenderingSystem.loadTextureFile("logo.png");

			logo_bg = theEntityManager.CreateEntity();
			ADD_COMPONENT(logo_bg, Rendering);
			ADD_COMPONENT(logo_bg, Transformation);
			TRANSFORM(logo_bg)->position = Vector2(0,0);
			TRANSFORM(logo_bg)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::ScreenHeight);
			RENDERING(logo_bg)->hide = false;
			RENDERING(logo_bg)->color = Color(0,0,0);
			TRANSFORM(logo_bg)->z = DL_BehindLogo;

			ADD_COMPONENT(soundButton, Transformation);
			TRANSFORM(soundButton)->z = DL_MainMenuUI;
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
				
			for (int i=0; i<8; i++) {
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
			TRANSFORM(benchTotalTime)->z = DL_Benchmark;

			std::vector<std::string> allSystems = ComponentSystem::registeredSystemNames();
			for (int i = 0; i < allSystems.size() ; i ++) {
				Entity b = theEntityManager.CreateEntity();
				ADD_COMPONENT(b, Rendering);
				ADD_COMPONENT(b, Transformation);
				TRANSFORM(b)->position = Vector2(0, benchPos);
				TRANSFORM(b)->size = Vector2(.8,0.8);
				TRANSFORM(b)->z = DL_Benchmark;
				RENDERING(b)->color = (i % 2) ? Color(0.1, 0.1, 0.1,0.5):Color(0.8,0.8,0.8,0.5);
				RENDERING(b)->hide = true;
				benchTimeSystem[allSystems[i]] = b;
			}
	#if 0
		cursor = theEntityManager.CreateEntity();
		ADD_COMPONENT(cursor, Transformation);
		TRANSFORM(cursor)->z = DL_Fading;
		ADD_COMPONENT(cursor, Particule);
		PARTICULE(cursor)->emissionRate = 50;
	    PARTICULE(cursor)->texture = InvalidTextureRef;
	    PARTICULE(cursor)->lifetime = Interval<float>(0.5f, 1.5f);
	    PARTICULE(cursor)->initialColor = Interval<Color> (Color(1.0, 0, 0, 0.7), Color(0.5, 0, 0, 0.7));
	    PARTICULE(cursor)->finalColor  = Interval<Color> (Color(0, 0, 1, 0.1), Color(0, 0, 0.5, 0.1));
		PARTICULE(cursor)->initialSize = Interval<float>(0.15, 0.2);
		PARTICULE(cursor)->finalSize = Interval<float>(0.01, 0.015);
		PARTICULE(cursor)->forceDirection = Interval<float>(0.1, 3);
		PARTICULE(cursor)->forceAmplitude  = Interval<float>(20, 100);
		PARTICULE(cursor)->mass = 1;
	#endif
		}
		//bench data
		std::map<std::string, Entity> benchTimeSystem;
		Entity benchTotalTime, targetTime;

		GameState state, stateBeforePause;
		bool stateBeforePauseNeedEnter;
		Entity logo, logo_bg, sky, decord2nd, decord1er;
		Entity music[8];
		std::map<GameState, GameStateManager*> state2Manager;
		std::map<GameMode, GameModeManager*> mode2Manager;
		ScoreStorage* storage;
		GameMode mode;

		Entity cursor;

		Entity soundButton;
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

Game::Game(ScoreStorage* storage, PlayerNameInputUI* inputUI) {
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
	theRenderingSystem.loadAtlas("alphabet");
	*/
	if (in && size) {
		datas->state = Pause;
		loadState(in, size);
	}
	datas->Setup(windowW, windowH);

	if (in && size) {
		RENDERING(datas->logo_bg)->hide = true;
	}

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

	datas->decord2nd = theEntityManager.CreateEntity();
	ADD_COMPONENT(datas->decord2nd, Transformation);
	TRANSFORM(datas->decord2nd)->z = DL_Decor2nd;
	TRANSFORM(datas->decord2nd)->size = Vector2(bgElementWidth, (bgElementWidth * 470.0) / 808.0);
	TransformationSystem::setPosition(TRANSFORM(datas->decord2nd), Vector2(0, PlacementHelper::GimpYToScreen(610)), TransformationSystem::N);
	ADD_COMPONENT(datas->decord2nd, Scrolling);
	SCROLLING(datas->decord2nd)->images.push_back("decor2nd_0.png");
	SCROLLING(datas->decord2nd)->images.push_back("decor2nd_1.png");
	SCROLLING(datas->decord2nd)->images.push_back("decor2nd_2.png");
	SCROLLING(datas->decord2nd)->images.push_back("decor2nd_3.png");
	SCROLLING(datas->decord2nd)->speed = Vector2(-.7, 0);
	SCROLLING(datas->decord2nd)->displaySize = Vector2(TRANSFORM(datas->decord2nd)->size.X * 1.01, TRANSFORM(datas->decord2nd)->size.Y);

	datas->decord1er = theEntityManager.CreateEntity();
	ADD_COMPONENT(datas->decord1er, Transformation);
	TRANSFORM(datas->decord1er)->z = DL_Decor1er;
	TRANSFORM(datas->decord1er)->size = Vector2(bgElementWidth, (bgElementWidth * 300.0) / 808.0);
	TransformationSystem::setPosition(TRANSFORM(datas->decord1er), Vector2(0, PlacementHelper::GimpYToScreen(1280)), TransformationSystem::S);
	ADD_COMPONENT(datas->decord1er, Scrolling);
	SCROLLING(datas->decord1er)->images.push_back("decor1er_0.png");
	SCROLLING(datas->decord1er)->images.push_back("decor1er_1.png");
	SCROLLING(datas->decord1er)->images.push_back("decor1er_2.png");
	SCROLLING(datas->decord1er)->images.push_back("decor1er_3.png");
	SCROLLING(datas->decord1er)->speed = Vector2(-1.5, 0);
	SCROLLING(datas->decord1er)->displaySize = Vector2(TRANSFORM(datas->decord1er)->size.X * 1.01, TRANSFORM(datas->decord1er)->size.Y);

	Entity branch = theEntityManager.CreateEntity();
	ADD_COMPONENT(branch, Transformation);
	TRANSFORM(branch)->z = DL_Branch;
	TRANSFORM(branch)->size = Vector2(bgElementWidth, (bgElementWidth * 400.0) / 800.0);
	TransformationSystem::setPosition(TRANSFORM(branch), Vector2(0, PlacementHelper::GimpYToScreen(0)), TransformationSystem::N);
	ADD_COMPONENT(branch, Rendering);
	RENDERING(branch)->hide = false;
	RENDERING(branch)->texture = theRenderingSystem.loadTextureFile("branche.png");

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

struct Compatible {
	std::vector<char> fri;
	char c;
	bool operator== (const Compatible & comp) const {
		return (c==comp.c);
	}
	bool operator>= (const Compatible & comp) const {
		return (c >= comp.c);
	}
	bool operator> (const Compatible & comp) const {
		return (c > comp.c);
	}
};

int findCompatible(char c, std::vector<Compatible> &comp) {
	for (int i=0;i<comp.size();i++)
		if (comp[i].c==c) return i;
	return comp.size();
}
std::vector<char> intersec(const std::vector<char>& c1, std::vector<char> c2) {
	std::vector<char> res;
	for (int i=0;i<c1.size();i++) {
		for (int j=0;j<c2.size();j++) {
			if (c1[i]==c2[j]) {
				res.push_back(c1[i]);
				c2.erase(c2.begin()+j);
				j--;
				break;
			}
		}
	}
	return res;
}
bool identic(const std::vector<char>& c1, std::vector<char> c2) {
	for (int i=0;i<c1.size();i++) {
		int j;
		for (j=0;j<c2.size();j++) {
			if (c1[i]==c2[j]) {
				c2.erase(c2.begin()+j);
				j--;
				break;
			}
		}
		if (j==c2.size()) return false;
	}
	return true;
}

static void updateMusic(Entity* music, float percentLeft) {
	//init
	static std::vector<char> ppal;
	if (ppal.size()==0) {
		ppal.push_back('A');
		ppal.push_back('C');
		ppal.push_back('E');
		ppal.push_back('F');
	}
	static std::vector<struct Compatible> compatible;
	if (compatible.size()==0) {
		for (int i=0;i<7;i++) {
			Compatible a;
			a.c = 'A'+i;
			for (int j=0;j<3;j++) {
				if (j!=i) a.fri.push_back('A'+j);
			}
			compatible.push_back(a);
		}
	}

	/* are all started entities done yet ? */
	/* (and percentLeft > 10%)*/
	if (percentLeft <= 0 || percentLeft > .1) {
		for (int i=4; i<8; i++) SOUND(music[i])->stop = true;
		for (int i=0; i<4; i++) {
			if (SOUND(music[i])->mainMusic && SOUND(music[i])->sound != InvalidSoundRef)
				return;
		}
	/* or maybe some stress songs...*/
	} else if (percentLeft != 0) {
		for (int i=0; i<4; i++) {
			if (SOUND(music[i])->mainMusic && SOUND(music[i])->sound != InvalidSoundRef) {
				SOUND(music[4+i])->position = SOUND(music[i])->position;
				SOUND(music[i])->stop = true;
				SOUND(music[4+i])->stop = false;
			}
		}
		for (int i=4; i<8; i++) {
			if (SOUND(music[i])->mainMusic && SOUND(music[i])->sound != InvalidSoundRef) {
				return;
			}
		}
	}

	int count = MathUtil::RandomInt(4) + 1;
	LOGW("starting %d music", count);


	std::vector<char> l; // all songs id
	std::vector<char> canPickIn; // songs which can be picked

	int cr = MathUtil::RandomInt(ppal.size());
	char c = ppal[cr]; // letter from main music
	int indice = findCompatible(c, compatible); // get his id
	std::stringstream s1, s2;
	s1<<"audio/"<<c<<".ogg";
	s2<<"audio/"<<c<<"stress.ogg";
	l.push_back(c);
	SOUND(music[4])->stop = true;
	SOUND(music[0])->stop = false;
	SOUND(music[0])->sound = theSoundSystem.loadSoundFile(s1.str(), true);
	SOUND(music[4])->sound = theSoundSystem.loadSoundFile(s2.str(), true);
	SOUND(music[0])->mainMusic = true;
	SOUND(music[4])->mainMusic = true;

	for (int i=0; i<compatible[indice].fri.size(); i++)
		canPickIn.push_back(compatible[indice].fri[i]); //copying his friends into canPickIn

	for (int i=1; i<count; i++) {
		if (identic(canPickIn,l)) {
			LOGI("no more musics can be found; abort at %d musics inserted", i);
			break;
		}
		SoundComponent* sc = SOUND(music[i]);

		do {
			c = canPickIn[MathUtil::RandomInt(canPickIn.size())];
		} while (std::find(l.begin(), l.end(), c) != l.end());

		l.push_back(c);
		std::stringstream s;
		s << "audio/" << c << ".ogg";
		sc->sound = theSoundSystem.loadSoundFile(s.str(), true);
		sc->stop = false;
		if (std::find(ppal.begin(), ppal.end(), c) != ppal.end()) {
			sc->mainMusic = true;
			std::stringstream s2;
			s2 << "audio/" << c << "stress.ogg";
			SOUND(music[4+i])->stop = true;
			SOUND(music[4+i])->sound = theSoundSystem.loadSoundFile(s2.str(), true);
		} else {
			sc->mainMusic = false;
		}
		
		indice = findCompatible(c, compatible); // get the song id
		canPickIn = intersec(compatible[indice].fri, canPickIn); //pickIn = friends[him] U friends[old ones]
	}
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
	float timeLeft; //ended = (timeLeft<=);

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
			static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->ended = true;
		} else if (newState == ModeMenu) {
			datas->mode = (static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu]))->choosenGameMode;
			setMode(); //on met à jour le mode de jeu dans les etats qui en ont besoin
		}
		
		datas->state2Manager[datas->state]->Exit();
		datas->state = newState;
		datas->state2Manager[datas->state]->Enter();

		hideEveryThing(!inGameState(newState), datas->state==BlackToSpawn);
		RENDERING(datas->logo_bg)->hide = RENDERING(datas->logo)->hide;
	}
	//si on appuye sur le bouton pause
	if (BUTTON(datas->soundButton)->clicked) {
		BUTTON(datas->soundButton)->clicked = false;
		datas->storage->soundEnable(true); //on met a jour la table sql
		theSoundSystem.mute = !theSoundSystem.mute;
		if (!theSoundSystem.mute) SOUND(datas->soundButton)->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		if (theSoundSystem.mute) RENDERING(datas->soundButton)->texture = theRenderingSystem.loadTextureFile("sound_off.png");
		else RENDERING(datas->soundButton)->texture = theRenderingSystem.loadTextureFile("sound_on.png");
	}

	//updating time
	if (datas->state == UserInput) {
		timeLeft = datas->mode2Manager[datas->mode]->Update(dt);
		ended = timeLeft <= 0;
		//si on change de niveau
		if (datas->mode2Manager[datas->mode]->LeveledUp()) {
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

	if (newState == ModeMenu) {
		theGridSystem.DeleteAll();
		datas->mode2Manager[datas->mode]->Reset();
	}

	updateMusic(datas->music, timeLeft);

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
