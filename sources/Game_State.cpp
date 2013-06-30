/*
    This file is part of Heriswap.

    @author Soupe au Caillou - Jordane Pelloux-Prayer
    @author Soupe au Caillou - Gautier Pelloux-Prayer
    @author Soupe au Caillou - Pierre-Eric Pelloux-Prayer

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

#include "systems/HeriswapGridSystem.h"

#include "modes/NormalModeManager.h"
#include "modes/TilesAttackModeManager.h"

#include "util/ScoreStorageProxy.h"

#include "systems/ADSRSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/ScrollingSystem.h"

#if 0
void HeriswapGame::stateChanged(GameState oldState, GameState newState) {
    if (newState == Unpause) {
        togglePause(false);
    //pressing "give up" button
    } else if (oldState == Pause && newState == MainMenu) {
         LOGI("aborted. going to main menu");
         RENDERING(datas->soundButton)->show = true;
         // datas->state2Manager[datas->stateBeforePause]->Exit();
         // datas->mode2Manager[datas->mode]->Exit();
         newState = MainMenu;
         // static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->gameOverState = ModeMenuStateManager::NoGame;
         stopInGameMusics();
     //click on a mode button
     } else if (oldState == MainMenu && newState == ModeMenu) {
         datas->mode = (static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu]))->choosenGameMode;
         //reference title into mode menu from main menu
         static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->title = static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu])->eStart[datas->mode];
         setMode(); //on met à jour le mode de jeu dans les etats qui en ont besoin
     //end game
     } else if (newState == BlackToModeMenu) {
        RENDERING(datas->soundButton)->show = false;
        datas->mode2Manager[datas->mode]->Exit();
        stopInGameMusics();
     //end game, stop musics
     } else if (newState == GameToBlack) {
         stopInGameMusics();
        static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->gameOverState = ModeMenuStateManager::GameEnded;
        // place title
        MORPHING(static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->title)->active = true;
     //let's play !
     } else if (newState == LevelChanged) {
        static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->smallLevel =
        static_cast<NormalGameModeManager*> (datas->mode2Manager[Normal])->getSmallLevelEntity();
        static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->currentLevel =
         (static_cast<NormalGameModeManager*> (datas->mode2Manager[Normal]))->currentLevel();
        stopInGameMusics();
     //back button to main
     } else if( newState == MainMenu && oldState == ModeMenu) {
         RENDERING(datas->soundButton)->show = true;
         datas->state2Manager[oldState]->LateExit();
     } else if (newState == BlackToMainMenu) {
        SCROLLING(datas->sky)->show = true;
        RENDERING(datas->soundButton)->show = true;
     } else if (newState == Help) {
        static_cast<HelpStateManager*> (datas->state2Manager[newState])->mode = datas->mode;
        datas->mode2Manager[datas->mode]->showGameDecor(true);
     } else if (newState == ExitState) {
         exitAPI->exitGame();
     } else if (newState == Spawn) {
        if (!theMusicSystem.isMuted() && datas->mode == Normal) {
            if (MUSIC(datas->inGameMusic.masterTrack)->loopNext != InvalidMusicRef) {
             if (shouldPlayPiano()) {
                 // replace master track next
                 theMusicSystem.unloadMusic(MUSIC(datas->inGameMusic.masterTrack)->loopNext);
                 MUSIC(datas->inGameMusic.masterTrack)->loopNext = theMusicSystem.loadMusicFile("audio/H.ogg");
             }
            }
         }
     }
     if (oldState == Help) {
        datas->mode2Manager[datas->mode]->showGameDecor(false);
     }
}
#endif
