#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glfw.h>
//liste des keys dans /usr/include/GL/glfw.h 
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/time.h>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <vector>

#include <base/Vector2.h>
#include <base/TouchInputManager.h>
#include <base/TimeUtil.h>
#include <base/MathUtil.h>

#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/MusicSystem.h"

#include "api/linux/MusicAPILinuxOpenALImpl.h"
#include "api/linux/AssetAPILinuxImpl.h"
#include "api/linux/SoundAPILinuxOpenALImpl.h"
#include "api/linux/LocalizeAPILinuxImpl.h"
#include "api/linux/NameInputAPILinuxImpl.h"
#include "api/linux/ExitAPILinuxImpl.h"

#include "api/linux/StorageAPILinuxImpl.h"

#include "Game.h"

#define DT 1/60.
#define MAGICKEYTIME 0.3

class MouseNativeTouchState: public NativeTouchState {
	public:
		bool isTouching(Vector2* windowCoords) const {
			int x,y;
			glfwGetMousePos(&x, &y);
			windowCoords->X = x;
			windowCoords->Y = y;

			return glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
		}
};

int main(int argc, char** argv) {
	if (!glfwInit())
		return 1;

	Vector2 reso16_9(394, 700);
	Vector2 reso16_10(430, 700);

	Vector2* reso = (argc == 1) ? &reso16_10 : &reso16_9;
	if( !glfwOpenWindow( reso->X,reso->Y, 8,8,8,8,8,8, GLFW_WINDOW ) )
		return 1;

	glewInit();


	// pose de l'origine du temps ici t = 0
	TimeUtil::init();
	uint8_t* state = 0;
	int size = 0;
	if (argc > 1 && !strcmp(argv[1], "-restore")) {
		FILE* file = fopen("dump.bin", "r+b");
		if (file) {
			std::cout << "Restoring game state from file" << std::endl;
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			fseek(file, 0, SEEK_SET);
			state = new uint8_t[size];
			fread(state, size, 1, file);
			fclose(file);
		}
	}

    StorageAPILinuxImpl* storage = new StorageAPILinuxImpl();
    storage->init();
std::cout << "Total: " << storage->getSavedGamePointsSum() << std::endl;
	Game game(new AssetAPILinuxImpl(), storage, new NameInputAPILinuxImpl(), new SuccessAPI(), new LocalizeAPILinuxImpl(), new AdAPI(), new ExitAPILinuxImpl());

theRenderingSystem.opengles2 = true;
	//theSoundSystem.init();
	theTouchInputManager.setNativeTouchStatePtr(new MouseNativeTouchState());
    theMusicSystem.musicAPI = new MusicAPILinuxOpenALImpl();
    theMusicSystem.assetAPI = new AssetAPILinuxImpl();
    theRenderingSystem.assetAPI = new AssetAPILinuxImpl();
    theSoundSystem.soundAPI = new SoundAPILinuxOpenALImpl();
    theMusicSystem.init();
    theSoundSystem.init();
    
    game.sacInit(reso->X,reso->Y);
	game.init(state, size);

	bool running = true;
	float timer = 0;
	float dtAccumuled=0, dt = 0, time = 0;

	time = TimeUtil::getTime();

	int frames = 0;
	float nextfps = time + 5;
	while(running) {

		do {
			dt = TimeUtil::getTime() - time;
			if (dt < DT) {
				struct timespec ts;
				ts.tv_sec = 0;
				ts.tv_nsec = (DT - dt) * 1000000000LL;
				nanosleep(&ts, 0);
			}
		} while (dt < DT);

		if (dt > 1./20) {
			dt = 1./20.;
		}
		dtAccumuled += dt;
		time = TimeUtil::getTime();
		while (dtAccumuled >= DT){
			dtAccumuled -= DT;
			game.tick(DT);
			running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
			bool focus = glfwGetWindowParam(GLFW_ACTIVE);
			if (focus) {
				theMusicSystem.toggleMute(theSoundSystem.mute);
			} else {
				// theMusicSystem.toggleMute(true);
			}
			//pause ?
			if (glfwGetKey( GLFW_KEY_SPACE )) {// || !focus) {
				game.togglePause(true);
			}
			//magic key?
			if ((glfwGetKey( GLFW_KEY_ENTER ) || glfwGetKey( GLFW_KEY_KP_ENTER) ) && timer<=0) {
				game.toggleShowCombi(false);
				timer = MAGICKEYTIME;
			}
			if (glfwGetKey( GLFW_KEY_BACKSPACE)) {
				game.backPressed();
			}
			if (glfwGetKey( GLFW_KEY_LSHIFT)) {
				uint8_t* state = 0;
				int size = game.saveState(&state);
				if (size) {
					LOGI("ptr: %p %d", state, size);
					FILE* file = fopen("dump.bin", "w+b");
					fwrite(state, size, 1, file);
					fclose(file);
				}
				running = false;
				break;
			}
			timer -= DT;
			frames++;
			if (time > nextfps) {
				//std::cout << "FPS: " << (frames / 5) << std::endl;
				nextfps = time + 5;
				frames = 0;
			}
		}

		theRenderingSystem.render();
		glfwSwapBuffers();
	}

	Vector2 x(Vector2::Zero);

	glfwTerminate();

	return 0;
}
