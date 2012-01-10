#include <GL/glfw.h>

#include "base/Vector2.h"

int main() {
	if (!glfwInit())
		return 1;

	if( !glfwOpenWindow( 480,800, 0,0,0,0,0,0, GLFW_WINDOW ) )
		return 1;

	bool running = true;
	while(running) {
		glfwSwapBuffers();

		running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
	}
	Vector2 x(Vector2::Zero);

	glfwTerminate();

	return 0;
}
