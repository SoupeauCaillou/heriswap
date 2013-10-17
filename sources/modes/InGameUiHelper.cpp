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


#include "InGameUiHelper.h"

#include <base/PlacementHelper.h>
#include <base/EntityManager.h>

#include <glm/glm.hpp>

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TextSystem.h"
#include "systems/SoundSystem.h"
#include "systems/ButtonSystem.h"

#include "DepthLayer.h"

InGameUiHelper::InGameUiHelper() : built(false) {

}

void InGameUiHelper::build() {
    if (built)
        return;

    smallLevel = theEntityManager.CreateEntity("smallLevel",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("general/smallLevel"));

    pauseButton = theEntityManager.CreateEntity("pauseButton",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("general/pauseButton"));

    scoreProgress = theEntityManager.CreateEntity("scoreProgress",
        EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("general/scoreProgress"));

    built = true;
}

void InGameUiHelper::show() {
    if (!built)
        return;

    TEXT(smallLevel)->show = true;
    RENDERING(pauseButton)->show = true;
    TEXT(scoreProgress)->show = true;
    BUTTON(pauseButton)->enabled=true;
}

void InGameUiHelper::update(float) {
    // handle button
    if (BUTTON(pauseButton)->clicked) {
        SOUND(pauseButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
        game->togglePause(true);
    }
}

void InGameUiHelper::hide() {
    if (!built)
        return;
    TEXT(smallLevel)->show = false;
    RENDERING(pauseButton)->show = false;
    BUTTON(pauseButton)->enabled = false;
    TEXT(scoreProgress)->show = false;
}

void InGameUiHelper::destroy() {
    if (!built)
        return;
    theEntityManager.DeleteEntity(smallLevel);
    theEntityManager.DeleteEntity(pauseButton);
    theEntityManager.DeleteEntity(scoreProgress);
}
