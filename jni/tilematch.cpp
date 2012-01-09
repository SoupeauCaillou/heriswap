#include <jni.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>

#include "importgl.h"

#include "sac/base/Vector2.h"

void
Java_net_damsy_soupeaucaillou_tilematch_DemoRenderer_nativeInit( JNIEnv*  env )
{
	Vector2 x(1, 2);
	x = x - Vector2::UnitX;
}

void
Java_net_damsy_soupeaucaillou_tilematch_DemoRenderer_nativeResize( JNIEnv*  env, jobject  thiz, jint w, jint h )
{
    __android_log_print(ANDROID_LOG_INFO, "Tilematch", "resize w=%d h=%d", w, h);
}

/* Call to finalize the graphics state */
void
Java_net_damsy_soupeaucaillou_tilematch_DemoRenderer_nativeDone( JNIEnv*  env )
{
	
}

/* This is called to indicate to the render loop that it should
 * stop as soon as possible.
 */
void
Java_net_damsy_soupeaucaillou_tilematch_DemoGLSurfaceView_nativePause( JNIEnv*  env )
{
}

/* Call to render the next GL frame */
void
Java_net_damsy_soupeaucaillou_tilematch_DemoRenderer_nativeRender( JNIEnv*  env )
{
}
