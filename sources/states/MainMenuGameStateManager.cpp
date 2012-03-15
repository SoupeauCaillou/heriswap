#include "MainMenuGameStateManager.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"

MainMenuGameStateManager::MainMenuGameStateManager() {

}

MainMenuGameStateManager::~MainMenuGameStateManager() {
	theEntityManager.DeleteEntity(start);
	theEntityManager.DeleteEntity(score);
	theTextRenderingSystem.DestroyLocalEntity(eScore);
	theTextRenderingSystem.DestroyLocalEntity(eStart);
}

void MainMenuGameStateManager::Setup() {
	start = theEntityManager.CreateEntity();
	score = theEntityManager.CreateEntity();

	ADD_COMPONENT(start, Transformation);
	ADD_COMPONENT(start, Rendering);

	RENDERING(start)->texture = theRenderingSystem.loadTextureFile("feuilles.png");
	RENDERING(start)->bottomLeftUV = Vector2(0, 0);
	RENDERING(start)->topRightUV = Vector2(1 / 8.0, 1);
	TRANSFORM(start)->size = Game::CellSize() * Game::CellContentScale();
	RENDERING(start)->hide = true;
	
	ADD_COMPONENT(score, Transformation);
	ADD_COMPONENT(score, Rendering);

	RENDERING(score)->texture = theRenderingSystem.loadTextureFile("feuilles.png");
	RENDERING(score)->bottomLeftUV = Vector2(1/8.0, 0);
	RENDERING(score)->topRightUV = Vector2(2/8.0, 1);
	TRANSFORM(score)->size = Game::CellSize() * Game::CellContentScale();
	RENDERING(score)->hide = true;

	eStart = theTextRenderingSystem.CreateLocalEntity(7);
	eScore = theTextRenderingSystem.CreateLocalEntity(6);

	TRANSFORM(start)->position = Vector2(0,3);
	TRANSFORM(eStart)->position = TRANSFORM(start)->position + Vector2(1.5, 0);

	TRANSFORM(score)->position = Vector2(0,1);
	TRANSFORM(eScore)->position = TRANSFORM(score)->position + Vector2(1.5, 0);


	TEXT_RENDERING(eStart)->hide = true;
	TEXT_RENDERING(eScore)->hide = true;

	RENDERING(start)->color = Color(0,0,0,0);
	RENDERING(score)->color = Color(0,0,0,0);

	TRANSFORM(start)->rotation = 0;
	TRANSFORM(score)->rotation = 0;

	TEXT_RENDERING(eStart)->text = "Play !";
	TEXT_RENDERING(eScore)->text = "Score";
	TEXT_RENDERING(eStart)->alignL = true;
	TEXT_RENDERING(eScore)->alignL = true;
	
	bStart = theEntityManager.CreateEntity();
	ADD_COMPONENT(bStart, Transformation);
	ADD_COMPONENT(bStart, Container);
	ADD_COMPONENT(bStart, Sound);
	SOUND(bStart)->type = SoundComponent::EFFECT;
	ADD_COMPONENT(bStart, Button);
	ADD_COMPONENT(bStart, Rendering);
	RENDERING(bStart)->color = Color(1.0, .0, .0, .5);
	
	bScore = theEntityManager.CreateEntity();
	ADD_COMPONENT(bScore, Transformation);
	ADD_COMPONENT(bScore, Container);
	ADD_COMPONENT(bScore, Button);
	ADD_COMPONENT(bScore, Rendering);
	RENDERING(bScore)->color = Color(.0, 1.0, .0, .5);
}

void MainMenuGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	
	// preload sound effect
	theSoundSystem.loadSoundFile("audio/click.wav", false);

	//Pour les rotations et autres animations
	elapsedTime = 0;

	TEXT_RENDERING(eStart)->hide = false;
	TEXT_RENDERING(eScore)->hide = false;
	RENDERING(start)->hide = false;
	RENDERING(score)->hide = false;

	BUTTON(bScore)->clicked = false;
	BUTTON(bStart)->clicked = false;
	
	CONTAINER(bScore)->entities.push_back(score);
	CONTAINER(bScore)->entities.push_back(eScore);
	CONTAINER(bScore)->includeChildren = true;
	CONTAINER(bStart)->entities.push_back(start);
	CONTAINER(bStart)->entities.push_back(eStart);
	CONTAINER(bStart)->includeChildren = true;
	RENDERING(bScore)->hide = false;
	RENDERING(bStart)->hide = false;
}

GameState MainMenuGameStateManager::Update(float dt) {
	/* adjust buttons position/size to text + icon position & size */
	Vector2 startBL = TRANSFORM(start)->worldPosition - TRANSFORM(start)->size * .5;
	Vector2 startTR = TRANSFORM(start)->worldPosition - TRANSFORM(start)->size * .5;
	
	elapsedTime += dt/4.;
	if (BUTTON(bStart)->clicked){
		SOUND(bStart)->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		return MainMenuToBlackState;
	} else if (BUTTON(bScore)->clicked) {
		return ScoreBoard;
	}

	if (elapsedTime>1)
		elapsedTime = 1;

	RENDERING(start)->color = Color(elapsedTime,elapsedTime,elapsedTime,elapsedTime);
	RENDERING(score)->color = Color(elapsedTime,elapsedTime,elapsedTime,elapsedTime);

	TRANSFORM(start)->rotation += dt/elapsedTime;
	TRANSFORM(score)->rotation -= dt/elapsedTime;
	return MainMenu;
}

void MainMenuGameStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eStart)->hide = true;
	TEXT_RENDERING(eScore)->hide = true;
	RENDERING(start)->hide = true;
	RENDERING(score)->hide = true;
	RENDERING(bScore)->hide = true;
	RENDERING(bStart)->hide = true;
	
	CONTAINER(bStart)->entities.clear();
	CONTAINER(bScore)->entities.clear();
}
