#include "Game.h"
#include "Game_Private.h"
#include "states/ModeMenuStateManager.h"
#include "states/MainMenuGameStateManager.h"

void Game::stateChanged(GameState oldState, GameState newState) {
    if (newState == Unpause) {
        togglePause(false);
    } else if (oldState == Pause && newState == MainMenu) {
         LOGI("aborted. going to main menu");
         datas->mode2Manager[datas->mode]->Exit();
         newState = MainMenu;
         static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->ended = false;
     } else if (newState == ModeMenu) {
         datas->mode = (static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu]))->choosenGameMode;
             //reference title into mode menu from main menu
         static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->title = static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu])->eStart[datas->mode-1];
         setMode(); //on met à jour le mode de jeu dans les etats qui en ont besoin
     } else if (newState == BlackToMainMenu) {
         RENDERING(datas->logo)->hide = true;
     } else if (newState == BlackToSpawn) {
            // call Enter before starting fade-in
         datas->mode2Manager[datas->mode]->Enter();
     }
}