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
    newGame = false;
}

 PrivateData::~PrivateData() {
    for(auto it : mode2Manager)
        delete it.second;
    mode2Manager.clear();
 }

 void PrivateData::Setup() {
    soundButton = theEntityManager.CreateEntity("soundButton",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("general/soundButton"));

    if (storageAPI->isOption("sound", "on"))
        RENDERING(soundButton)->texture = theRenderingSystem.loadTextureFile("sound_on");
    else
        RENDERING(soundButton)->texture = theRenderingSystem.loadTextureFile("sound_off");

    socialGamNet = theEntityManager.CreateEntity("socialGamNet",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("general/socialGamNet"));

    for (int i=0; i<3; ++i) {
        Entity e = theEntityManager.CreateEntity("cloud",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("general/cloud"));
        theBackgroundSystem.initCloud(e, i);
    }

    for(auto it : mode2Manager)
        it.second->Setup();

    menu = theEntityManager.CreateEntity("menu",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("music/menuTrack"));

    inGameMusic.masterTrack = theEntityManager.CreateEntity("masterTrack",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("music/masterTrack"));

    for (int i=0; i<3; i++) {
        std::stringstream a;
        a << "" << "secondaryTracks_" << i;
        inGameMusic.secondaryTracks[i] = theEntityManager.CreateEntity(a.str(),
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("music/secondaryTrack"));
        MUSIC(inGameMusic.secondaryTracks[i])->master = MUSIC(inGameMusic.masterTrack);
    }
    inGameMusic.accessoryTrack = theEntityManager.CreateEntity("accessoryTrack",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("music/accessoryTrack"));
    MUSIC(inGameMusic.accessoryTrack)->master = MUSIC(inGameMusic.masterTrack);

    inGameMusic.stressTrack = theEntityManager.CreateEntity("stressTrack",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("music/stressTrack"));
    MUSIC(inGameMusic.stressTrack)->master = MUSIC(inGameMusic.masterTrack);

    const float MusicFadeOut = 1;
    theMusicSystem.forEachECDo([&MusicFadeOut] (Entity, MusicComponent *mc) -> void {
        mc->fadeOut = MusicFadeOut;
    });

    (static_cast<NormalGameModeManager*> (mode2Manager[Normal]))->stressTrack = inGameMusic.stressTrack;
    // PauseStateManager* pause = static_cast<PauseStateManager*> (state2Manager[Pause]);
    // pause->helpMgr = static_cast<HelpStateManager*> (state2Manager[Help]);

    // static_cast<ModeMenuStateManager*>(state2Manager[ModeMenu])->helpMgr = pause->helpMgr;
}
