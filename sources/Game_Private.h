#pragma once

#include "base/EntityManager.h"
#include "systems/SoundSystem.h"
#include "systems/MusicSystem.h"
#include "Sound.h"
class Game;
class ScoreStorage;
class PlayerNameInputUI;
class GameStateManager;
class SuccessAPI;
#include "modes/GameModeManager.h"

#include <map>
class PrivateData {
    public:
        PrivateData(Game* game, ScoreStorage* storagee, PlayerNameInputUI* inputUI, SuccessAPI* successAPI, LocalizeAPI* lAPI);

        void Setup(int windowW, int windowH);

     //bench data
     std::map<std::string, Entity> benchTimeSystem;
     Entity benchTotalTime, targetTime;

     GameState state, stateBeforePause;
     bool stateBeforePauseNeedEnter;
     Entity sky, decord2nd, decord1er;
     std::map<GameState, GameStateManager*> state2Manager;
     std::map<GameMode, GameModeManager*> mode2Manager;
     ScoreStorage* storage;
     GameMode mode;

     Entity cursor;

     Entity soundButton;
     Entity musicMenu[2], musicStress1[2], musicStress2[2], music[8];
     Entity menu;
     struct {
	     Entity masterTrack;
	     Entity secondaryTracks[3];
	     Entity accessoryTrack;
	     Entity stressTrack;
     } inGameMusic;
};
