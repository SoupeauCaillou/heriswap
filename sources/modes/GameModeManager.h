#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "systems/TextRenderingSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/System.h"

#include "DepthLayer.h"
#include "Game.h"
#include "InGameUiHelper.h"
#include "base/PlacementHelper.h"
#include "AnimedEntity.h"

//FCRR : FPS Calculation Refresh Rate
#define FCRR 1.


enum GameMode {
	Normal = 1,
	ScoreAttack,
	StaticTime
};


class GameModeManager {
	public:
		struct BranchLeaf {
			Entity e;
			int type;
		};
		struct Render {
			Vector2 v;
			float rot;
		};

		GameModeManager(Game* game) { uiHelper.game = game; }
		
		virtual ~GameModeManager() {}
		
		// to be called once: create long standing entities
		virtual void Setup();
		// to be called at the beginning of each game: setup entites, scoring, etc..
		virtual void Enter();
		// to be called during the game (only in UserInput GameState) - return game completion percentage
		virtual void GameUpdate(float dt) = 0;
		// to be called during the game (only in UserInput GameState) - return game completion percentage
		virtual float GameProgressPercent() = 0;
		// to be called once per frame during game
		virtual void UiUpdate(float dt) = 0;
		// to be called after game-over occured
		virtual void Exit();
		// to be called to toggle pause mode display
		virtual void TogglePauseDisplay(bool paused);

		
		// scoring interface
		virtual void WillScore(int nb, int type, std::vector<Entity>& out) {}
		virtual void ScoreCalc(int nb, int type) = 0;
		virtual GameMode GetMode() = 0;
		virtual void LevelUp() = 0;
		virtual bool LeveledUp() = 0;
		
	protected:
		float position(float t);
		void generateLeaves(int nb);
		void LoadHerissonTexture(int type);
		void updateHerisson(float dt, float obj, float herissonSpeed);
		void deleteLeaves(int type, int nb);
		
	public:
		// game params
		float time, limit;
		int points, bonus;
        Entity sky;
	
	protected:
		// display elements
		InGameUiHelper uiHelper;
		Entity branch;
		Entity decor1er, decor2nd;
		std::vector<Vector2> pts;
		Entity herisson;
		//feuilles de l'arbre
		std::vector<BranchLeaf> branchLeaves;
	private:		
		//hérisson
		AnimatedActor* c;

	private :
		std::vector<Render> posBranch;
		void fillVec();
		float distance;
};
