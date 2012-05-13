#include "Game.h"
#include "Game_Private.h"
#include "states/ModeMenuStateManager.h"
#include "states/MainMenuGameStateManager.h"
#include "states/LevelStateManager.h"
#include "modes/TilesAttackModeManager.h"

void Game::stateChanged(GameState oldState, GameState newState) {
    if (newState == Unpause) {
        togglePause(false);
    } else if (oldState == Pause && newState == MainMenu) {
         LOGI("aborted. going to main menu");
         datas->mode2Manager[datas->mode]->Exit();
         newState = MainMenu;
         static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->ended = false;
     } else if (oldState == MainMenu && newState == ModeMenu) {
         datas->mode = (static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu]))->choosenGameMode;
         //reference title into mode menu from main menu
         static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->title = static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu])->eStart[datas->mode-1];
         setMode(); //on met à jour le mode de jeu dans les etats qui en ont besoin
     } else if (newState == ModeMenu) {
        datas->mode2Manager[datas->mode]->Exit();
     } else if (newState == BlackToSpawn) {
            // call Enter before starting fade-in
         datas->mode2Manager[datas->mode]->Enter();
         
         MUSIC(datas->menu)->control = MusicComponent::Stop;
         // start ingame music
	     std::vector<std::string> musics = newMusics();
	     MUSIC(datas->inGameMusic.masterTrack)->music = theMusicSystem.loadMusicFile(musics[0]);
	     MUSIC(datas->inGameMusic.masterTrack)->control = MusicComponent::Start;
	     for (int i=0; i<musics.size()-1; i++) {
		 	MUSIC(datas->inGameMusic.secondaryTracks[i])->music = theMusicSystem.loadMusicFile(musics[i+1]);
		 	MUSIC(datas->inGameMusic.secondaryTracks[i])->control = MusicComponent::Start;
	     }
     } else if (newState == LevelChanged) {
        static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->smallLevel =
        static_cast<NormalGameModeManager*> (datas->mode2Manager[Normal])->getSmallLevelEntity();

        MUSIC(datas->inGameMusic.masterTrack)->control = MusicComponent::Stop;
        for(int i=0; i<3; i++) {
            MUSIC(datas->inGameMusic.secondaryTracks[i])->control = MusicComponent::Stop;
        }
     } else if( newState == MainMenu && oldState == ModeMenu) {
        datas->state2Manager[oldState]->LateExit();
     }
}
