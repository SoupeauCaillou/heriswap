#include "Game.h"
#include "Game_Private.h"

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"
#include "modes/StaticTimeModeManager.h"
#include "modes/ScoreAttackModeManager.h"
#include "modes/TilesAttackModeManager.h"
#include "GameState.h"
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


PrivateData::PrivateData(Game* game, ScoreStorage* storagee, PlayerNameInputUI* inputUI) {
     mode = Normal;
     mode2Manager[Normal] = new NormalGameModeManager(game);
     mode2Manager[ScoreAttack] = new ScoreAttackGameModeManager(game);
     mode2Manager[StaticTime] = new StaticTimeGameModeManager(game);
     mode2Manager[TilesAttack] = new TilesAttackGameModeManager(game);
     storage = storagee;

     soundButton = theEntityManager.CreateEntity();

     state = BlackToLogoState;

     state2Manager[Spawn] = new SpawnGameStateManager();
     state2Manager[UserInput] = new UserInputGameStateManager();
     state2Manager[Delete] = new DeleteGameStateManager();
     state2Manager[Fall] = new FallGameStateManager();
     state2Manager[LevelChanged] = new LevelStateManager();
     state2Manager[Pause] = new PauseStateManager();
     state2Manager[Logo] = new LogoStateManager(LogoToBlackState);
     state2Manager[MainMenu] = new MainMenuGameStateManager();
     state2Manager[ModeMenu] = new ModeMenuStateManager(storage,inputUI);
     
     state2Manager[BlackToLogoState] = new FadeGameStateManager(FadeIn, BlackToLogoState, Logo, state2Manager[Logo], 0);
     state2Manager[LogoToBlackState] = new FadeGameStateManager(FadeOut, LogoToBlackState, BlackToMainMenu, 0, state2Manager[Logo]);
     state2Manager[BlackToMainMenu] = new FadeGameStateManager(FadeIn, BlackToMainMenu, MainMenu, state2Manager[MainMenu], 0);
     state2Manager[ModeMenuToBlackState] = new FadeGameStateManager(FadeOut, ModeMenuToBlackState, BlackToSpawn, 0, state2Manager[ModeMenu]);
     state2Manager[BlackToSpawn] = new FadeGameStateManager(FadeIn, BlackToSpawn, Spawn, 0, 0);
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
