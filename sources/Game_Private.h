#pragma once

#include <string>
#include <map>

#include <base/EntityManager.h>

#include "systems/SoundSystem.h"
#include "systems/MusicSystem.h"

#include "modes/GameModeManager.h"

#include "Jukebox.h"
class Game;
class ScoreStorage;
class PlayerNameInputUI;
class GameStateManager;
class SuccessAPI;
class NameInputAPI;
class AdAPI;

class PrivateData {
    public:
        PrivateData(Game* game, StorageAPI* storagee, NameInputAPI* inputUI, SuccessManager* successMgr, LocalizeAPI* lAPI, SuccessAPI* sAPI, AdAPI* ad);
        ~PrivateData();
        void Setup(int windowW, int windowH);

     //bench data
     std::map<std::string, Entity> benchTimeSystem;
     Entity benchTotalTime, targetTime;

     GameState state, stateBeforePause;
     bool stateBeforePauseNeedEnter;
     Entity sky, decord2nd, decord1er;
     std::map<GameState, GameStateManager*> state2Manager;
     std::map<GameMode, GameModeManager*> mode2Manager;
     StorageAPI* storage;
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
     Jukebox jukebox;
};
