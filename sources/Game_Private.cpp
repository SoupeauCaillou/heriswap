#include "Game.h"
#include "Game_Private.h"

#include <base/PlacementHelper.h>

#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"
#include "modes/TilesAttackModeManager.h"

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
#include "states/HelpStateManager.h"
#include "states/AdsStateManager.h"

#include "DepthLayer.h"
#include "GameState.h"


PrivateData::PrivateData(Game* game, StorageAPI* storagee, NameInputAPI* inputUI, SuccessManager* successMgr, LocalizeAPI* lAPI, SuccessAPI* sAPI) {
     mode = Normal;
     mode2Manager[Normal] = new NormalGameModeManager(game,successMgr);
     mode2Manager[TilesAttack] = new TilesAttackGameModeManager(game,successMgr);
     storage = storagee;

     soundButton = theEntityManager.CreateEntity();

     state = BlackToLogoState;

     state2Manager[Spawn] = new SpawnGameStateManager(successMgr);
     state2Manager[UserInput] = new UserInputGameStateManager(successMgr);
     state2Manager[Delete] = new DeleteGameStateManager(successMgr);
     state2Manager[Fall] = new FallGameStateManager();
     state2Manager[LevelChanged] = new LevelStateManager(static_cast<NormalGameModeManager*> (mode2Manager[Normal]));
     state2Manager[Pause] = new PauseStateManager(lAPI);
     state2Manager[Logo] = new LogoStateManager(LogoToBlackState);
     state2Manager[MainMenu] = new MainMenuGameStateManager(lAPI);
     state2Manager[ModeMenu] = new ModeMenuStateManager(storage,inputUI,successMgr,lAPI, sAPI);
     state2Manager[Help] = new HelpStateManager();
     state2Manager[Ads] = new AdsStateManager(storage, successMgr);

     state2Manager[BlackToLogoState] = new FadeGameStateManager(FadeIn, BlackToLogoState, Logo, state2Manager[Logo], 0);
     state2Manager[LogoToBlackState] = new FadeGameStateManager(FadeOut, LogoToBlackState, BlackToMainMenu, 0, state2Manager[Logo]);
     state2Manager[BlackToMainMenu] = new FadeGameStateManager(FadeIn, BlackToMainMenu, MainMenu, state2Manager[MainMenu], 0);
     state2Manager[ModeMenuToBlackState] = new FadeGameStateManager(FadeOut, ModeMenuToBlackState, BlackToAds, 0, state2Manager[ModeMenu]);
     state2Manager[BlackToAds] = new FadeGameStateManager(FadeIn, BlackToAds, Ads, state2Manager[Ads], 0);
     state2Manager[AdsToBlackState] = new FadeGameStateManager(FadeOut, AdsToBlackState, BlackToSpawn, 0, state2Manager[Ads]);
     state2Manager[BlackToSpawn] = new FadeGameStateManager(FadeIn, BlackToSpawn, Spawn, 0, 0);
	 state2Manager[GameToBlack] = new FadeGameStateManager(FadeOut, GameToBlack, BlackToModeMenu, 0, 0);
     state2Manager[BlackToModeMenu] = new FadeGameStateManager(FadeIn, BlackToModeMenu, ModeMenu, state2Manager[ModeMenu], 0);
 }

 void PrivateData::Setup(int windowW, int windowH) {
     BackgroundManager* bg = new BackgroundManager((10.0 * windowH) / windowW);
     bg->cloudStartX = Interval<float>(0.0,15.0);
     state2Manager[Background] = bg;

     ADD_COMPONENT(soundButton, Transformation);
     TRANSFORM(soundButton)->z = DL_MainMenuUITxt;
     TRANSFORM(soundButton)->size = Vector2(PlacementHelper::GimpWidthToScreen(67), PlacementHelper::GimpHeightToScreen(80));
     TransformationSystem::setPosition(TRANSFORM(soundButton), Vector2(PlacementHelper::GimpXToScreen(692), PlacementHelper::GimpYToScreen(1215)), TransformationSystem::W);
     ADD_COMPONENT(soundButton, Button);
     ADD_COMPONENT(soundButton, Rendering);
     RENDERING(soundButton)->color = Color(3.0/255.0, 99.0/255, 71.0/255);
     if (storage->soundEnable(false)) RENDERING(soundButton)->texture = theRenderingSystem.loadTextureFile("sound_on.png");
     else RENDERING(soundButton)->texture = theRenderingSystem.loadTextureFile("sound_off.png");
     ADD_COMPONENT(soundButton, Sound);
     RENDERING(soundButton)->hide = false;

     for(std::map<GameState, GameStateManager*>::iterator it=state2Manager.begin(); it!=state2Manager.end(); ++it)
         it->second->Setup();

     for(std::map<GameMode, GameModeManager*>::iterator it=mode2Manager.begin(); it!=mode2Manager.end(); ++it)
         it->second->Setup();

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

     menu = theEntityManager.CreateEntity();
     ADD_COMPONENT(menu, Music);
     MUSIC(menu)->control = MusicComponent::Start;
     MUSIC(menu)->loopAt = 64.0f;

     inGameMusic.masterTrack = theEntityManager.CreateEntity();
     ADD_COMPONENT(inGameMusic.masterTrack, Music);
     MUSIC(inGameMusic.masterTrack)->loopAt = 17.0f;
     for (int i=0; i<3; i++) {
     	inGameMusic.secondaryTracks[i] = theEntityManager.CreateEntity();
     	ADD_COMPONENT(inGameMusic.secondaryTracks[i], Music);
     	MUSIC(inGameMusic.secondaryTracks[i])->loopAt = 17.0f;
     	MUSIC(inGameMusic.secondaryTracks[i])->master = MUSIC(inGameMusic.masterTrack);
     }
     inGameMusic.accessoryTrack = theEntityManager.CreateEntity();
     ADD_COMPONENT(inGameMusic.accessoryTrack, Music);
     MUSIC(inGameMusic.accessoryTrack)->loopAt = 17.0f;
     MUSIC(inGameMusic.accessoryTrack)->master = MUSIC(inGameMusic.masterTrack);
     MUSIC(inGameMusic.accessoryTrack)->volume = 0.75;
     
     inGameMusic.stressTrack = theEntityManager.CreateEntity();
     ADD_COMPONENT(inGameMusic.stressTrack, Music);
     MUSIC(inGameMusic.stressTrack)->loopAt = 17.0f;
     MUSIC(inGameMusic.stressTrack)->master = MUSIC(inGameMusic.masterTrack);
     ADD_COMPONENT(inGameMusic.stressTrack, ADSR);
    ADSR(inGameMusic.stressTrack)->idleValue = 0;
	ADSR(inGameMusic.stressTrack)->attackValue = 1.0;
	ADSR(inGameMusic.stressTrack)->attackTiming = 1;
	ADSR(inGameMusic.stressTrack)->decayTiming = 0;
	ADSR(inGameMusic.stressTrack)->sustainValue = 1.0;
	ADSR(inGameMusic.stressTrack)->releaseTiming = 0.5;
	ADSR(inGameMusic.stressTrack)->attackMode = Quadratic;
	
	const float MusicFadeOut = .5f;
	std::vector<Entity> musics = theMusicSystem.RetrieveAllEntityWithComponent();
	for (int i=0; i<musics.size(); i++) {
		MUSIC(musics[i])->fadeOut = MusicFadeOut;
	}
     
     (static_cast<NormalGameModeManager*> (mode2Manager[Normal]))->stressTrack = inGameMusic.stressTrack;
	PauseStateManager* pause = static_cast<PauseStateManager*> (state2Manager[Pause]);
	pause->helpMgr = static_cast<HelpStateManager*> (state2Manager[Help]);
}
