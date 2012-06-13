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
#include <UnitTest++.h>

#include "modes/NormalModeManager.h"
#include <sstream>
#include <iostream>
#include <iomanip>


TEST(SiReste3FeuillesASupprIlReste3DansLArbe) {
	for (int i=0; i<2500; i++) {
		int onBranch = 6;
		int initialInGrid = 3 + MathUtil::RandomInt(30);
		int inGrid = initialInGrid;
		int removed = 0;

		std::stringstream log;
		log << "initialInGrid: " << initialInGrid << std::endl;	
		do {
			int remove = MathUtil::RandomIntInRange(3, 7);
			inGrid = MathUtil::Max(inGrid - remove, 0);
			int rmv = NormalGameModeManager::levelToLeaveToDelete(0, remove, initialInGrid, removed, onBranch);
			removed += remove;
			onBranch = onBranch - rmv;
			log << "\tRemove: " << remove << " -> " << rmv << " = grid: " << inGrid << ", branch: " << onBranch << std::endl;
			
			if (onBranch < 0 || onBranch > 6) {
				CHECK(onBranch >= 0);
				CHECK(onBranch <= 6);
				std::cerr << log.str();
				break;
			}
			
			if (inGrid > 3) {
				if (!(inGrid >= onBranch)) {
					CHECK(inGrid >= onBranch);
					std::cerr << log.str();
					break;
				}
			} else {
				if (inGrid != onBranch) {
					CHECK_EQUAL(inGrid, onBranch);
					std::cerr << log.str();
					break;
				}
			}
		} while (inGrid > 0);
		
		if (inGrid == 0 && onBranch == 0) {
			// std::cerr << "SUCCESS:" << std::endl << log.str() << "----------------------------------" << std::endl;
		}
	}
}

TEST(SuppressionFeuilleNiveauNormalDifficulty) {
	char cleared[8];
	memset(cleared, 0, sizeof(cleared));

	for (int i=0; i<2500; i++) {
		unsigned char initialCountByType[8];
		memset(initialCountByType, 6, 8);
		int level = 1 + MathUtil::RandomInt(20);
		std::stringstream log;
		// log.setw(2);
		char countLeft[8];
		memset(countLeft, level + 2, sizeof(countLeft));
		
		log << "Level: " << level << std::endl;
		
		// PARTIE FINIE ?
		while (memcmp(countLeft, cleared, sizeof(countLeft)) != 0) {
			int color = MathUtil::RandomInt(8);
			if (countLeft[color] == 0)
				continue;

			for (int k=0; k<8; k++) {
				log << std::setw(2) << (int)countLeft[k] << " ";
			}
			log << " => left: ";
			for (int k=0; k<8; k++) {
				log << std::setw(2) << (int)initialCountByType[k] << " ";
			}
			log << std::endl;
			
			if (!memcmp(countLeft, cleared, sizeof(countLeft))) {
				CHECK(!memcmp(countLeft, cleared, sizeof(countLeft)));
				std::cerr << log.str() << std::endl;
				break;
			}
			int count = MathUtil::Min(1 + MathUtil::RandomIntInRange(2, 5), (int)countLeft[color]);
			
			int remove = NormalGameModeManager::levelToLeaveToDelete(color, count, level + 2, level + 2 - countLeft[color], initialCountByType[color]);
			initialCountByType[color] -= remove;
			
			for (int k=0; k<color; k++)
				log << "   ";
			log << " ^ -" << count << " => -" << remove << std::endl;

			countLeft[color] -= count;
			if (countLeft[color] < 0)
				countLeft[color] = 0;

			for (int i=0; i<8; i++) {
				if (initialCountByType[color] == 0) {
					if (countLeft[color] != 0) {
						CHECK_EQUAL(0, (int)countLeft[color]);
						std::cerr << log.str() << std::endl;
						break;
					}		
				} else if (initialCountByType[color] > 0) {
					CHECK(countLeft[color] > 0);
				} else {
					CHECK(false);
				}
			}
		}
		if (memcmp(initialCountByType, cleared, sizeof(countLeft))) {
			std::cerr << log.str() << std::endl;
		}
		CHECK_EQUAL(0, memcmp(initialCountByType, cleared, sizeof(countLeft)));
	}
}

