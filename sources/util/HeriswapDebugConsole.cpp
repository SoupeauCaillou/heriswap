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


#if SAC_INGAME_EDITORS && SAC_DEBUG

#include "HeriswapDebugConsole.h"

#include "systems/TextSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/GridSystem.h"

#include "base/Log.h"
#include "base/EntityManager.h"

#include "Game_Private.h"
#include "HeriswapGame.h"
#include "../modes/NormalModeManager.h"

HeriswapGame* HeriswapDebugConsole::_game = 0;

void HeriswapDebugConsole::init(HeriswapGame* game) {
    _game = game;

}

void HeriswapDebugConsole::callbackJumpAt9(void*) {
}

#endif
