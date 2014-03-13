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
#include "app/AppSetup.h"

#include <string>
#include <sstream>

#include "HeriswapGitVersion.h"

int main(int argc, char** argv) {
    #define MAX_SIZE 700
    #define RATIO (16.0 / 9.0)
    float height = MAX_SIZE;
    float width = MAX_SIZE / RATIO;
    
    // To debug ratio issue
    // expected arguments are [height] [width] (optionnel both)
    if (argc > 1) {
        std::istringstream buffer(argv[1]);
        buffer >> height;
        width = height / RATIO;
    }
    if (argc > 2) {
        std::istringstream buffer(argv[2]);
        buffer >> width;
    }

    std::string versionName = "";
    #if SAC_DEBUG
        versionName = versionName + " / " + TAG_NAME + " - " + VERSION_NAME;
    #endif
    if (initGame("Heriswap", glm::ivec2(width, height), versionName)) {
        LOGE("Failed to initialize");
        return 1;
    }
    
    return launchGame(new HeriswapGame(), argc, argv);
}
