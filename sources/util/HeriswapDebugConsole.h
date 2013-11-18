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

#include "util/DebugConsole.h"

#include "base/StateMachine.h"

class HeriswapGame;

class HeriswapDebugConsole {
    public:
        static void init(HeriswapGame* game);
        static void callbackJumpAt9(void* arg);

    private:
        //to interact with the game
        static HeriswapGame* _game;
};

#endif
