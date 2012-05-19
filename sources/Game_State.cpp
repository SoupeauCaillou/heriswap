#include "Game.h"
#include "Game_Private.h"
#include "states/ModeMenuStateManager.h"
#include "states/MainMenuGameStateManager.h"
#include "states/LevelStateManager.h"

#include "states/DeleteGameStateManager.h"
#include "states/UserInputGameStateManager.h"
#include "states/SpawnGameStateManager.h"
#include "states/FallGameStateManager.h"
#include "modes/TilesAttackModeManager.h"

void Game::stopInGameMusics() {
	MUSIC(datas->inGameMusic.masterTrack)->control = MusicComponent::Stop;
	MUSIC(datas->inGameMusic.accessoryTrack)->control = MusicComponent::Stop;
	MUSIC(datas->inGameMusic.stressTrack)->control = MusicComponent::Stop;
    for(int i=0; i<3; i++) {
 	   MUSIC(datas->inGameMusic.secondaryTracks[i])->control = MusicComponent::Stop;
    }
}

void Game::stateChanged(GameState oldState, GameState newState) {
    if (newState == Unpause) {
        togglePause(false);
    } else if (oldState == Pause && newState == MainMenu) {
         LOGI("aborted. going to main menu");
         datas->state2Manager[datas->stateBeforePause]->Exit();
         datas->mode2Manager[datas->mode]->Exit();
         newState = MainMenu;
         static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->ended = false;
         stopInGameMusics();
     } else if (oldState == MainMenu && newState == ModeMenu) {
         datas->mode = (static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu]))->choosenGameMode;
         //reference title into mode menu from main menu
         static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->title = static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu])->eStart[datas->mode-1];
         setMode(); //on met à jour le mode de jeu dans les etats qui en ont besoin
     } else if (newState == ModeMenu) {
        datas->mode2Manager[datas->mode]->Exit();
        stopInGameMusics();
     } else if (newState == GameToBlack) {
	     stopInGameMusics();
     //let's play !
     } else if (newState == BlackToSpawn) {
            // call Enter before starting fade-in
         datas->mode2Manager[datas->mode]->Enter();
         MUSIC(datas->menu)->control = MusicComponent::Stop;
                  
         //update anim times
         static_cast<DeleteGameStateManager*> (datas->state2Manager[Delete])->setAnimSpeed();
         static_cast<FallGameStateManager*> (datas->state2Manager[Fall])->setAnimSpeed();
         static_cast<SpawnGameStateManager*> (datas->state2Manager[Spawn])->setAnimSpeed();
		 static_cast<UserInputGameStateManager*> (datas->state2Manager[UserInput])->setAnimSpeed();
        
     } else if (newState == LevelChanged) {
        static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->smallLevel =
        static_cast<NormalGameModeManager*> (datas->mode2Manager[Normal])->getSmallLevelEntity();
        stopInGameMusics();
     } else if( newState == MainMenu && oldState == ModeMenu) {
        datas->state2Manager[oldState]->LateExit();
     }
}
