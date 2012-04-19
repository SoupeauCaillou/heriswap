#pragma once

#include <sstream>

#include "base/EntityManager.h"
#include "base/MathUtil.h"

class CombinationMark {
    public:
        static void markCellInCombination(Entity e);
        static void clearCellInCombination(Entity e);

};
