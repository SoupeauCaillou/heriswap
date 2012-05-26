#pragma once

#include <vector>
#include <string>

class Jukebox {
    public:
        const std::vector<std::string>& pickNextSongs(int maxSongCount);

    private:
        std::vector<std::string> currentSelection;

};
