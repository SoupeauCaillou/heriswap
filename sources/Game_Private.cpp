/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "HeriswapGame.h"
#include "Game_Private.h"

#include <base/PlacementHelper.h>

#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"

#include "modes/GameModeManager.h"
#include "modes/NormalModeManager.h"
#include "modes/TilesAttackModeManager.h"
#include "modes/Go100SecondsModeManager.h"

#include "states/StateManager.h"
#include "states/CountDownStateManager.h"
#include "states/SpawnStateManager.h"
#include "states/UserInputStateManager.h"
#include "states/DeleteStateManager.h"
#include "states/FallStateManager.h"
#include "states/MainMenuStateManager.h"
#include "states/BackgroundManager.h"
#include "states/LevelStateManager.h"
#include "states/PauseStateManager.h"
#include "states/FadeStateManager.h"
#include "states/ModeMenuStateManager.h"
#include "states/LogoStateManager.h"
#include "states/HelpStateManager.h"
#include "states/AdsStateManager.h"
#include "states/RateItStateManager.h"
#include "states/ElitePopupStateManager.h"

#include "DepthLayer.h"
#include "GameState.h"


PrivateData::PrivateData(HeriswapGame* game, StorageAPI* storagee, NameInputAPI* inputUI, SuccessManager* successMgr, LocalizeAPI* lAPI, SuccessAPI* sAPI, AdAPI* ad, CommunicationAPI* comAPI) {
     mode = Normal;
     mode2Manager[Normal] = new NormalGameModeManager(game, successMgr, storagee);
     mode2Manager[TilesAttack] = new TilesAttackGameModeManager(game, successMgr, storagee);
     mode2Manager[Go100Seconds] = new Go100SecondsGameModeManager(game, successMgr, storagee);
     storage = storagee;

     soundButton = theEntityManager.CreateEntity();
     socialGamNet = theEntityManager.CreateEntity();

     state = BlackToLogoState;

     state2Manager[CountDown] = new CountDownStateManager();
     state2Manager[Spawn] = new SpawnGameStateManager(successMgr);
     state2Manager[UserInput] = new UserInputGameStateManager(successMgr);
     state2Manager[Delete] = new DeleteGameStateManager(successMgr);
     state2Manager[Fall] = new FallGameStateManager();
     state2Manager[LevelChanged] = new LevelStateManager(static_cast<NormalGameModeManager*> (mode2Manager[Normal]));
     state2Manager[Pause] = new PauseStateManager(lAPI);
     state2Manager[Logo] = new LogoStateManager(LogoToBlackState);
     state2Manager[MainMenu] = new MainMenuGameStateManager(lAPI, sAPI);
     state2Manager[ModeMenu] = new ModeMenuStateManager(storage,inputUI,successMgr,lAPI, sAPI, comAPI);
     state2Manager[Help] = new HelpStateManager(lAPI);
     state2Manager[Ads] = new AdsStateManager(ad, storage, successMgr);
     state2Manager[RateIt] = new RateItStateManager(lAPI, comAPI);
     state2Manager[ElitePopup] = new ElitePopupStateManager(static_cast<NormalGameModeManager*>(mode2Manager[Normal]), lAPI);

     state2Manager[BlackToLogoState] = new FadeGameStateManager(0.2f, FadeIn, BlackToLogoState, Logo, state2Manager[Logo], 0);
     state2Manager[LogoToBlackState] = new FadeGameStateManager(0.3f, FadeOut, LogoToBlackState, BlackToMainMenu, 0, state2Manager[Logo]);
     state2Manager[BlackToMainMenu] = new FadeGameStateManager(0.3f, FadeIn, BlackToMainMenu, MainMenu, state2Manager[MainMenu], 0);
     state2Manager[ModeMenuToBlackState] = new FadeGameStateManager(0.2f, FadeOut, ModeMenuToBlackState, Ads, 0, state2Manager[ModeMenu]);
     state2Manager[AdsToBlackState] = new FadeGameStateManager(0.2f, FadeOut, AdsToBlackState, BlackToSpawn, 0, state2Manager[Ads]);
     state2Manager[BlackToSpawn] = new FadeGameStateManager(0.5f, FadeIn, BlackToSpawn, Spawn, 0, 0);
	 state2Manager[GameToBlack] = new FadeGameStateManager(4.f, FadeOut, GameToBlack, BlackToModeMenu, 0, 0);
     state2Manager[BlackToModeMenu] = new FadeGameStateManager(0.5f, FadeIn, BlackToModeMenu, ModeMenu, state2Manager[ModeMenu], 0);
 }

 PrivateData::~PrivateData() {
     for(std::map<GameState, GameStateManager*>::iterator it=state2Manager.begin(); it!=state2Manager.end(); ++it)
         delete it->second;
     state2Manager.clear();

     for(std::map<GameMode, GameModeManager*>::iterator it=mode2Manager.begin(); it!=mode2Manager.end(); ++it)
         delete it->second;
     mode2Manager.clear();
 }

 void PrivateData::Setup() {
     BackgroundManager* bg = new BackgroundManager();
     bg->cloudStartX = Interval<float>(0.0,15.0);
     state2Manager[Background] = bg;

     ADD_COMPONENT(soundButton, Transformation);
     TRANSFORM(soundButton)->z = DL_MainMenuUITxt;
     TRANSFORM(soundButton)->size = Vector2(PlacementHelper::GimpWidthToScreen(100), PlacementHelper::GimpHeightToScreen(95));
     TransformationSystem::setPosition(TRANSFORM(soundButton), Vector2(0 + PlacementHelper::GimpWidthToScreen(354), PlacementHelper::GimpYToScreen(1215)), TransformationSystem::E);
     ADD_COMPONENT(soundButton, Button);
     BUTTON(soundButton)->enabled =true;
     BUTTON(soundButton)->overSize = 1.3;
     ADD_COMPONENT(soundButton, Rendering);
     ADD_COMPONENT(soundButton, Sound);
     if (storage->soundEnable(false))
		RENDERING(soundButton)->texture = theRenderingSystem.loadTextureFile("sound_on");
     else
		RENDERING(soundButton)->texture = theRenderingSystem.loadTextureFile("sound_off");


     ADD_COMPONENT(socialGamNet, Transformation);
     TRANSFORM(socialGamNet)->z = DL_MainMenuUITxt;
     TRANSFORM(socialGamNet)->size = Vector2(PlacementHelper::GimpWidthToScreen(100), PlacementHelper::GimpHeightToScreen(95));
     TransformationSystem::setPosition(TRANSFORM(socialGamNet), Vector2(0 - PlacementHelper::GimpWidthToScreen(354), PlacementHelper::GimpYToScreen(1215)), TransformationSystem::W);
     ADD_COMPONENT(socialGamNet, Button);
     BUTTON(socialGamNet)->overSize = 1.3;
     ADD_COMPONENT(socialGamNet, Rendering);
     RENDERING(socialGamNet)->texture = theRenderingSystem.loadTextureFile("swarm_icon");

     for(std::map<GameState, GameStateManager*>::iterator it=state2Manager.begin(); it!=state2Manager.end(); ++it)
         it->second->Setup();

     for(std::map<GameMode, GameModeManager*>::iterator it=mode2Manager.begin(); it!=mode2Manager.end(); ++it)
         it->second->Setup();

     bg->cloudStartX = Interval<float>(8.0,15.0);

     MainMenuGameStateManager* mainmenu = static_cast<MainMenuGameStateManager*> (state2Manager[MainMenu]);
     ModeMenuStateManager* modemenu = static_cast<ModeMenuStateManager*> (state2Manager[ModeMenu]);
     RateItStateManager* rateIt = static_cast<RateItStateManager*> (state2Manager[RateIt]);
     modemenu->menufg = mainmenu->menufg;
     modemenu->menubg = mainmenu->menubg;
     modemenu->herisson = mainmenu->herisson;
     rateIt->menufg = mainmenu->menufg;
     rateIt->menubg = mainmenu->menubg;

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

	const float MusicFadeOut = 1;
	std::vector<Entity> musics = theMusicSystem.RetrieveAllEntityWithComponent();
	for (unsigned int i=0; i<musics.size(); i++) {
		MUSIC(musics[i])->fadeOut = MusicFadeOut;
	}

     (static_cast<NormalGameModeManager*> (mode2Manager[Normal]))->stressTrack = inGameMusic.stressTrack;
	PauseStateManager* pause = static_cast<PauseStateManager*> (state2Manager[Pause]);
	pause->helpMgr = static_cast<HelpStateManager*> (state2Manager[Help]);

	static_cast<ModeMenuStateManager*>(state2Manager[ModeMenu])->helpMgr = pause->helpMgr;
}
