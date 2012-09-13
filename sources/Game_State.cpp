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
#include "GridSystem.h"

#include "systems/ScrollingSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/ADSRSystem.h"

#include "states/ModeMenuStateManager.h"
#include "states/MainMenuStateManager.h"
#include "states/LevelStateManager.h"
#include "states/DeleteStateManager.h"
#include "states/UserInputStateManager.h"
#include "states/SpawnStateManager.h"
#include "states/FallStateManager.h"
#include "states/HelpStateManager.h"

#include "modes/TilesAttackModeManager.h"
#include "modes/NormalModeManager.h"

void HeriswapGame::stopInGameMusics() {
	MUSIC(datas->inGameMusic.masterTrack)->control = MusicComponent::Stop;
	MUSIC(datas->inGameMusic.accessoryTrack)->control = MusicComponent::Stop;
	MUSIC(datas->inGameMusic.stressTrack)->control = MusicComponent::Stop;
    for(int i=0; i<3; i++) {
 	   MUSIC(datas->inGameMusic.secondaryTracks[i])->control = MusicComponent::Stop;
    }
}

void HeriswapGame::setupGameProp() {
	if (datas->mode == Go100Seconds) {
		ADSR((static_cast<DeleteGameStateManager*> (datas->state2Manager[Delete]))->deleteAnimation)->attackTiming = 0.2;
		ADSR((static_cast<UserInputGameStateManager*> (datas->state2Manager[UserInput]))->swapAnimation)->attackTiming = 0.03;
		ADSR((static_cast<UserInputGameStateManager*> (datas->state2Manager[UserInput]))->swapAnimation)->releaseTiming = 0.03;
		ADSR((static_cast<FallGameStateManager*> (datas->state2Manager[Fall]))->fallAnimation)->attackTiming = 0.1;
		ADSR((static_cast<SpawnGameStateManager*> (datas->state2Manager[Spawn]))->haveToAddLeavesInGrid)->attackTiming = 0.2;
		ADSR((static_cast<SpawnGameStateManager*> (datas->state2Manager[Spawn]))->replaceGrid)->attackTiming = 0.5;
		return;
	}

	//update anim times
	Difficulty difficulty = theGridSystem.sizeToDifficulty();
	if (difficulty == DifficultyEasy) {
		ADSR((static_cast<DeleteGameStateManager*> (datas->state2Manager[Delete]))->deleteAnimation)->attackTiming = 0.6;
		ADSR((static_cast<UserInputGameStateManager*> (datas->state2Manager[UserInput]))->swapAnimation)->attackTiming = 0.14;
		ADSR((static_cast<UserInputGameStateManager*> (datas->state2Manager[UserInput]))->swapAnimation)->releaseTiming = 0.14;
		ADSR((static_cast<FallGameStateManager*> (datas->state2Manager[Fall]))->fallAnimation)->attackTiming = 0.30;
		ADSR((static_cast<SpawnGameStateManager*> (datas->state2Manager[Spawn]))->haveToAddLeavesInGrid)->attackTiming = 0.40;
		ADSR((static_cast<SpawnGameStateManager*> (datas->state2Manager[Spawn]))->replaceGrid)->attackTiming = 1.;
	} else {
		ADSR((static_cast<DeleteGameStateManager*> (datas->state2Manager[Delete]))->deleteAnimation)->attackTiming = 0.3;
		ADSR((static_cast<UserInputGameStateManager*> (datas->state2Manager[UserInput]))->swapAnimation)->attackTiming = 0.07;
		ADSR((static_cast<UserInputGameStateManager*> (datas->state2Manager[UserInput]))->swapAnimation)->releaseTiming = 0.07;
		ADSR((static_cast<FallGameStateManager*> (datas->state2Manager[Fall]))->fallAnimation)->attackTiming = 0.15;
		ADSR((static_cast<SpawnGameStateManager*> (datas->state2Manager[Spawn]))->haveToAddLeavesInGrid)->attackTiming = 0.40;
		ADSR((static_cast<SpawnGameStateManager*> (datas->state2Manager[Spawn]))->replaceGrid)->attackTiming = 1.;
	}


	float avg;
	 std::vector<StorageAPI::Score> entries = datas->storage->savedScores(datas->mode, theGridSystem.sizeToDifficulty(), avg);
	 datas->bestScores.clear();
	 datas->bestScores.reserve(entries.size());
	 for (int i=0; i<entries.size(); i++) {
		datas->bestScores[i] = entries[i].points;
	 }
	 datas->scoreboardRankInSight = entries.size();
}

void HeriswapGame::stateChanged(GameState oldState, GameState newState) {
    if (newState == Unpause) {
        togglePause(false);
    //pressing "give up" button
    } else if (oldState == Pause && newState == MainMenu) {
         LOGI("aborted. going to main menu");
         RENDERING(datas->soundButton)->hide = false;
         datas->state2Manager[datas->stateBeforePause]->Exit();
         datas->mode2Manager[datas->mode]->Exit();
         newState = MainMenu;
         static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->gameOverState = ModeMenuStateManager::NoGame;
         stopInGameMusics();
     //click on a mode button
     } else if (oldState == MainMenu && newState == ModeMenu) {
         datas->mode = (static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu]))->choosenGameMode;
         //reference title into mode menu from main menu
         static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->title = static_cast<MainMenuGameStateManager*> (datas->state2Manager[MainMenu])->eStart[datas->mode];
         setMode(); //on met à jour le mode de jeu dans les etats qui en ont besoin
     //end game
     } else if (newState == BlackToModeMenu) {
        RENDERING(datas->soundButton)->hide = false;
        datas->mode2Manager[datas->mode]->Exit();
        stopInGameMusics();
     //end game, stop musics
     } else if (newState == GameToBlack) {
	     stopInGameMusics();
        static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->gameOverState = ModeMenuStateManager::GameEnded;
		// place title
		MORPHING(static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->title)->active = true;
     //let's play !
     } else if (newState == BlackToSpawn) {
        //for count down in 2nd mode
		static_cast<UserInputGameStateManager*> (datas->state2Manager[UserInput])->newGame = true;
		// call Enter before starting fade-in
         datas->mode2Manager[datas->mode]->Enter();
         datas->mode2Manager[datas->mode]->UiUpdate(0);
         MUSIC(datas->menu)->control = MusicComponent::Stop;

         setupGameProp();
     } else if (newState == LevelChanged) {
        static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->smallLevel =
        static_cast<NormalGameModeManager*> (datas->mode2Manager[Normal])->getSmallLevelEntity();
        static_cast<LevelStateManager*> (datas->state2Manager[LevelChanged])->currentLevel =
			(static_cast<NormalGameModeManager*> (datas->mode2Manager[Normal]))->currentLevel();
        stopInGameMusics();
     //back button to main
     } else if( newState == MainMenu && oldState == ModeMenu) {
         RENDERING(datas->soundButton)->hide = false;
         datas->state2Manager[oldState]->LateExit();
     } else if (newState == BlackToMainMenu) {
        SCROLLING(datas->sky)->hide = false;
        RENDERING(datas->soundButton)->hide = false;
     } else if (newState == Help) {
	     static_cast<HelpStateManager*> (datas->state2Manager[newState])->mode = datas->mode;
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
}
