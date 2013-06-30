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

#pragma once

class HeriswapGame;

namespace Scene {
    enum Enum {
        Ads,
        CountDown,
        Delete,
        ElitePopup,
        EndGame,
        EndMenu,
        ExitState,
        Fall,
        Help,
        LevelChanged,
        Logo,
        MainMenu,
        ModeMenu,
        Pause,
        RateIt,
        // ScoreBoard,
        Spawn,
        // Unpause,
        UserInput,
    };

#define DECLARE_SCENE_HANDLER_FACTORY(name) \
    StateHandler<Scene::Enum>* Create##name##SceneHandler(HeriswapGame* game);



    DECLARE_SCENE_HANDLER_FACTORY(Ads)
    DECLARE_SCENE_HANDLER_FACTORY(CountDown)
    DECLARE_SCENE_HANDLER_FACTORY(Delete)
    DECLARE_SCENE_HANDLER_FACTORY(ElitePopup)
    DECLARE_SCENE_HANDLER_FACTORY(EndGame)
    DECLARE_SCENE_HANDLER_FACTORY(EndMenu)
    DECLARE_SCENE_HANDLER_FACTORY(ExitState)
    DECLARE_SCENE_HANDLER_FACTORY(Fall)
    DECLARE_SCENE_HANDLER_FACTORY(Help)
    DECLARE_SCENE_HANDLER_FACTORY(LevelChanged)
    DECLARE_SCENE_HANDLER_FACTORY(Logo)
    DECLARE_SCENE_HANDLER_FACTORY(MainMenu)
    DECLARE_SCENE_HANDLER_FACTORY(ModeMenu)
    DECLARE_SCENE_HANDLER_FACTORY(Pause)
    DECLARE_SCENE_HANDLER_FACTORY(RateIt)
    DECLARE_SCENE_HANDLER_FACTORY(ScoreBoard)
    DECLARE_SCENE_HANDLER_FACTORY(Spawn)
    DECLARE_SCENE_HANDLER_FACTORY(Unpause)
    DECLARE_SCENE_HANDLER_FACTORY(UserInput)
}
