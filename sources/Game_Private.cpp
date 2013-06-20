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

#include "DepthLayer.h"

#include "modes/GameModeManager.h"
#include "modes/Go100SecondsModeManager.h"
#include "modes/NormalModeManager.h"
#include "modes/TilesAttackModeManager.h"

#include "systems/BackgroundSystem.h"

#include <base/PlacementHelper.h>

#include "systems/ADSRSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>


PrivateData::PrivateData(HeriswapGame* game, GameContext* context, SuccessManager* successMgr) {
    mode = Normal;
    this->successMgr = successMgr;
    mode2Manager[Normal] = new NormalGameModeManager(game, successMgr, context->storageAPI);
    mode2Manager[TilesAttack] = new TilesAttackGameModeManager(game, successMgr, context->storageAPI);
    mode2Manager[Go100Seconds] = new Go100SecondsGameModeManager(game, successMgr, context->storageAPI);
    storageAPI = context->storageAPI;
}

 PrivateData::~PrivateData() {
    for(auto it : mode2Manager)
        delete it.second;
    mode2Manager.clear();
 }

 void PrivateData::Setup() {
    // BackgroundManager* bg = new BackgroundManager();
    // bg->cloudStartX = Interval<float>(0.0,15.0);
    // state2Manager[Background] = bg;

    soundButton = theEntityManager.CreateEntity("soundButton",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("general/soundButton"));

    if (storageAPI->isOption("sound", "on"))
        RENDERING(soundButton)->texture = theRenderingSystem.loadTextureFile("sound_on");
    else
        RENDERING(soundButton)->texture = theRenderingSystem.loadTextureFile("sound_off");

    socialGamNet = theEntityManager.CreateEntity("socialGamNet",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("general/socialGamNet"));

    // for(std::map<GameState, GameStateManager*>::iterator it=state2Manager.begin(); it!=state2Manager.end(); ++it)
    //     it->second->Setup();

    for (int i=0; i<3; ++i) {
        Entity e = theEntityManager.CreateEntity("cloud",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("general/cloud"));
        theBackgroundSystem.initCloud(e, i);
    }

    for(auto it : mode2Manager)
        it.second->Setup();

    // bg->cloudStartX = Interval<float>(8.0,15.0);

    // modemenu->menufg = mainmenu->menufg;
    // modemenu->menubg = mainmenu->menubg;
    // modemenu->herisson = mainmenu->herisson;
    // rateIt->menufg = mainmenu->menufg;
    // rateIt->menubg = mainmenu->menubg;

    menu = theEntityManager.CreateEntity("menu");
    ADD_COMPONENT(menu, Music);
    MUSIC(menu)->control = MusicControl::Play;
    MUSIC(menu)->loopAt = 64.0f;

    inGameMusic.masterTrack = theEntityManager.CreateEntity("masterTrack");
    ADD_COMPONENT(inGameMusic.masterTrack, Music);
    MUSIC(inGameMusic.masterTrack)->loopAt = 17.0f;

    for (int i=0; i<3; i++) {
        std::stringstream a;
        a << "" << "secondaryTracks_" << i;
        inGameMusic.secondaryTracks[i] = theEntityManager.CreateEntity(a.str());
        ADD_COMPONENT(inGameMusic.secondaryTracks[i], Music);
        MUSIC(inGameMusic.secondaryTracks[i])->loopAt = 17.0f;
        MUSIC(inGameMusic.secondaryTracks[i])->master = MUSIC(inGameMusic.masterTrack);
    }
    inGameMusic.accessoryTrack = theEntityManager.CreateEntity("accessoryTrack");
    ADD_COMPONENT(inGameMusic.accessoryTrack, Music);
    MUSIC(inGameMusic.accessoryTrack)->loopAt = 17.0f;
    MUSIC(inGameMusic.accessoryTrack)->master = MUSIC(inGameMusic.masterTrack);
    MUSIC(inGameMusic.accessoryTrack)->volume = 0.75;

    inGameMusic.stressTrack = theEntityManager.CreateEntity("stressTrack");
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
    // PauseStateManager* pause = static_cast<PauseStateManager*> (state2Manager[Pause]);
    // pause->helpMgr = static_cast<HelpStateManager*> (state2Manager[Help]);

    // static_cast<ModeMenuStateManager*>(state2Manager[ModeMenu])->helpMgr = pause->helpMgr;
}
