#pragma once

#include <base/EntityManager.h>

#include "Game.h"
#include "InGameUiHelper.h"
#include "AnimedEntity.h"
#include "SuccessManager.h"

enum GameMode {
	Normal = 1,
	TilesAttack
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

		GameModeManager(Game* game, SuccessManager* successMgr);

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
		virtual bool LevelUp() = 0;

        // state save/restore
        virtual int saveInternalState(uint8_t** out);
        virtual const uint8_t* restoreInternalState(const uint8_t* in, int size);
		void generateLeaves(int* nb, int type);

		float position(float t);
	protected:
		void LoadHerissonTexture(int type);
		void updateHerisson(float dt, float obj, float herissonSpeed);
		void deleteLeaves(int type, int nb);

	public:
		// game params
		float time;
		unsigned int points, bonus, limit;
        Entity sky;
		Entity herisson;
		//feuilles de l'arbre
		std::vector<BranchLeaf> branchLeaves;

	protected:
		// display elements
		InGameUiHelper uiHelper;
		Entity branch;
		Entity decor1er, decor2nd;
		std::vector<Vector2> pts;

		SuccessManager* successMgr;
		//hérisson
		AnimatedActor* c;
	private:
		std::vector<Render> posBranch;
		void fillVec();
		float distance;
};
