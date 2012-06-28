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
#include "AnimedEntity.h"

#include <sstream>

#include <base/MathUtil.h>

#include "systems/RenderingSystem.h"


void updateAnim(AnimatedActor* a, float dt) {
	a->timeElapsed+=dt;
	if (a->timeElapsed < 1/60.f)
		return;

	a->timeElapsed = 0.f;
	a->frames++;
	if (a->frames>=30/(MathUtil::Abs(a->actor.speed))) {
		RENDERING(a->actor.e)->texture = theRenderingSystem.loadTextureFile(a->anim[a->ind]);
		a->ind++;
		if (a->ind == (int)a->anim.size()) a->ind = 0;
		a->frames=0;
	}
}

void loadHerissonTexture(int type, AnimatedActor* c) {
	std::stringstream t;
	c->frames = 30;
	c->ind = 0;
	c->anim.clear();
	t << "herisson_1_"<<type;
	c->anim.push_back(t.str());
	t.str("");
	t << "herisson_2_"<<type;
	c->anim.push_back(t.str());
	t.str("");
	t << "herisson_3_"<<type;
	c->anim.push_back(t.str());
	t.str("");
	t << "herisson_2_"<<type;
	c->anim.push_back(t.str());
}
