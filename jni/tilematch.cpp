#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

#include "sac/base/Vector2.h"
#include "../sources/Game.h"
#include "sac/systems/RenderingSystem.h"
#include "sac/base/TouchInputManager.h"
#include "../sources/states/ScoreBoardStateManager.h"
#include <png.h>
#include <algorithm>

#include <sys/time.h>
#define DT 1.0/60.

struct saved_state {
	int scoreCount;
	int scoreboard[10];
	char name[10][64];
};

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    
    Game* game;
    
    saved_state state; 
};

struct __input {
    int touching;
    float x, y;
} input;

static struct timeval startup_time;

float timeconverter(struct timeval tv) {
	return (tv.tv_sec + tv.tv_usec / 1000000.0f);
}

float gettime() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	timersub(&tv, &startup_time, &tv);
	return timeconverter(tv);
}

static char* loadTextfile(const char* assetName);
static bool touch(Vector2* windowCoords);
static char* loadPng(const char* assetName, int* width, int* height);



SaveStateScoreStorage saveState;

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
    // initialize OpenGL ES and EGL

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	EGLint contextAttrib[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
	};
    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    context = eglCreateContext(display, config, NULL, contextAttrib);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;

    return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        // No display.
        LOGW("NO DISPLAY");
        return;
    }
	eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
    
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    		
    		if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN)
    			input.touching = 1;
    		else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_UP)
    			input.touching = 0;
    		input.x = AMotionEvent_getX(event, 0);
        input.y = AMotionEvent_getY(event, 0);
        LOGI("amoitionevnetaction: %08x -> %d (%f %f)\n", AMotionEvent_getAction(event), input.touching, input.x, input.y);
        return 1;
    }
    return 0;
}

#include <iostream>
/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
        		engine->app->savedState = new saved_state();
            *((struct saved_state*)engine->app->savedState) = engine->state;
            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
        		engine->animating = 1;
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
                engine_init_display(engine);
                LOGI("Window size: %dx%d\n", engine->width, engine->height);
                engine->game = new Game();
					 theRenderingSystem.setDecompressPNGImagePtr(&loadPng);
					 theRenderingSystem.setLoadShaderPtr(&loadTextfile);
					 theTouchInputManager.setNativeTouchStatePtr(&touch);           
	             engine->game->init(&saveState, engine->width, engine->height);
					theRenderingSystem.init();
   				theTouchInputManager.init(Vector2(10, 10. * engine->height / engine->width), Vector2(engine->width, engine->height));
                engine_draw_frame(engine);
                }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
				engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
        		engine->animating = 1;
            break;
        case APP_CMD_LOST_FOCUS:
        		engine->animating = 0;
            break;
    }
}

AAssetManager* asset = 0;
/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;

    // Make sure glue isn't stripped.
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;
    
    saveState.state = &engine.state;
    
    asset = state->activity->assetManager;

    // loop waiting for stuff to do.
    engine.game = 0;

	 gettimeofday(&startup_time,NULL);
	float dtAccumuled=0, dt = 0, time = 0;
	
    if (state->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        engine.state = *(struct saved_state*)state->savedState;
    }
    
	time = gettime();

    while (1) {
    
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;
        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.game ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {
            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            /*if (ident == LOOPER_ID_USER) {
                if (engine.accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
                            &event, 1) > 0) {
                        LOGI("accelerometer: x=%f y=%f z=%f",
                                event.acceleration.x, event.acceleration.y,
                                event.acceleration.z);
                    }
                }
            }*/

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                LOGW("return");
                return;
            }
        }

		if (engine.animating) {
			do {
				dt = gettime() - time;
				if (dt < DT) {
					struct timespec ts;
					ts.tv_sec = 0;
					ts.tv_nsec = (DT - dt) * 1000000000LL;
					nanosleep(&ts, 0);
				}
			} while (dt < DT);
		
			dtAccumuled += dt;
			time = gettime();
			// LOGW("dtAccumuled: %f (time since start:%f)\n", dtAccumuled, gettime());
			while (dtAccumuled >= DT){
					if (engine.game) {
						engine.game->tick(dtAccumuled);
						dtAccumuled = 0;
				  }
				// dtAccumuled -= DT;
			}
			engine_draw_frame(&engine);
		 } else {
		 	time = gettime();
		 }
		}
}

static char* loadPng(const char* assetName, int* width, int* height)
{
	LOGI("loadPng: %s\n", assetName);
	png_byte* PNG_image_buffer;
	AAsset* aaa = AAssetManager_open(asset, assetName, AASSET_MODE_UNKNOWN);
	if (!aaa) {
		LOGW("AAssetManager cannot open %s\n", assetName);
		return 0;
	}
	off_t start=0,length=0;
	int fd = AAsset_openFileDescriptor(aaa, &start, &length);
	LOGI("offset:%d\n", start);
	
	FILE* PNG_file = fdopen(fd, "r");
	if (PNG_file == NULL) {
		LOGW("%s not found\n", assetName);
		return 0;
	}
	fseek(PNG_file, start, SEEK_SET);
	GLubyte PNG_header[8];
		
	fread(PNG_header, 1, 8, PNG_file);
	if (png_sig_cmp(PNG_header, 0, 8) != 0) {
		LOGW("%s is not a PNG\n", assetName);
		//for(int i=0; i<8; i++) LOGW("%02x ", PNG_header[i]);
		//LOGW("\n");
		return 0;
	}
		
	png_structp PNG_reader = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (PNG_reader == NULL)
	{
		LOGW("Can't start reading %s.\n", assetName);
		fclose(PNG_file);
		return 0;
	}
		
	png_infop PNG_info = png_create_info_struct(PNG_reader);
	if (PNG_info == NULL)
	{
		LOGW("ERROR: Can't get info for %s\n", assetName);
		png_destroy_read_struct(&PNG_reader, NULL, NULL);
		fclose(PNG_file);
		return 0;
	}
		
	png_infop PNG_end_info = png_create_info_struct(PNG_reader);
	if (PNG_end_info == NULL)
	{
		LOGW("ERROR: Can't get end info for %s\n", assetName);
		png_destroy_read_struct(&PNG_reader, &PNG_info, NULL);
		fclose(PNG_file);
		return 0;
	}
		
	if (setjmp(png_jmpbuf(PNG_reader)))
	{
		LOGW("ERROR: Can't load %s\n", assetName);
		png_destroy_read_struct(&PNG_reader, &PNG_info, &PNG_end_info);
		fclose(PNG_file);
		return 0;
	}

	png_init_io(PNG_reader, PNG_file);
	png_set_sig_bytes(PNG_reader, 8);
		
	png_read_info(PNG_reader, PNG_info);
		
	*width = png_get_image_width(PNG_reader, PNG_info);
	*height = png_get_image_height(PNG_reader, PNG_info);

	png_uint_32 bit_depth, color_type;
	bit_depth = png_get_bit_depth(PNG_reader, PNG_info);
	color_type = png_get_color_type(PNG_reader, PNG_info);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(PNG_reader);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) 
	{
		png_set_expand_gray_1_2_4_to_8(PNG_reader);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(PNG_reader);
	}

	if (png_get_valid(PNG_reader, PNG_info, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(PNG_reader);
	}
	else
	{
		png_set_filler(PNG_reader, 0xff, PNG_FILLER_AFTER);
	}

	if (bit_depth == 16)
	{
		png_set_strip_16(PNG_reader);
	}

	png_read_update_info(PNG_reader, PNG_info);

	PNG_image_buffer = (png_byte*)malloc(4 * (*width) * (*height));
	png_byte** PNG_rows = (png_byte**)malloc(*height * sizeof(png_byte*));

	unsigned int row;
	for (row = 0; row < *height; ++row) {
		PNG_rows[*height - 1 - row] = PNG_image_buffer + (row * 4 * *width);
	}

	png_read_image(PNG_reader, PNG_rows);

	free(PNG_rows);

	png_destroy_read_struct(&PNG_reader, &PNG_info, &PNG_end_info);
	fclose(PNG_file);

	AAsset_close(aaa);
	return (char*)PNG_image_buffer;
}

static char* loadTextfile(const char* assetName)
{
	LOGI("loadTextFile: %s", assetName);
	AAsset* aaa = AAssetManager_open(asset, assetName, AASSET_MODE_UNKNOWN);
	if (!aaa)
		LOGW("asset not found");
	off_t size = AAsset_getLength(aaa);
	char* output = (char*) malloc(size * sizeof(char) + 1);
	AAsset_read(aaa, output, size);
	output[size] = '\0';
	AAsset_close(aaa);
	return output;
}

static bool touch(Vector2* windowCoords) {
	windowCoords->X = input.x;
	windowCoords->Y = input.y;

	return input.touching;
}



