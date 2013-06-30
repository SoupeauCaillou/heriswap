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

#include "modes/GameModeManager.h"

#include "systems/HeriswapGridSystem.h"

#include "util/StorageProxy.h"

#include <list>
#include <map>
#include <queue>
#include <string>

//to be moved
struct Score {
    int points;
    int level;
    float time;
    std::string name;
    GameMode mode;
    Difficulty difficulty;
};


class ScoreStorageProxy : public StorageProxy<Score> {
    public:
        ScoreStorageProxy();

        std::string getValue(const std::string& columnName);

        void setValue(const std::string& columnName, const std::string& value, bool pushNewElement = false);
};
