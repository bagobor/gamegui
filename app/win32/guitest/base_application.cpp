#include "base_application.h"
#include <GLFW/glfw3.h>


_env env = { 0 };
BaseApplication* BaseApplication::g_Instance;// = 0;

BaseApplication::BaseApplication(size_t w, size_t h, const char* title) 
: m_ready(false), window(nullptr) {
	g_Instance = this;

	if (glfwInit() != GL_TRUE) return; 
		
	{
		//glfwWindowHint(GLFW_RED_BITS, 8);
		//glfwWindowHint(GLFW_GREEN_BITS, 8);
		//glfwWindowHint(GLFW_BLUE_BITS, 8);
		//glfwWindowHint(GLFW_ALPHA_BITS, 8);
		//glfwWindowHint(GLFW_DEPTH_BITS, 16);

		//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		window = glfwCreateWindow(w, h, title, NULL, NULL);

		if (!glfwInit())
			exit(EXIT_FAILURE);

		if (!window)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(window);

		// Window resize callback function
		glfwSetWindowSizeCallback(window, _OnWindowsizefun );
		//// Set keyboard input callback function
		glfwSetKeyCallback(window, _OnKeyfun);
		glfwSetMouseButtonCallback(window, _OnMousebuttonfun);
		glfwSetCursorPosCallback(window, _OnMouseposfun);

		m_ready = true;
	}
}

BaseApplication::~BaseApplication() {
	glfwDestroyWindow(window);
	glfwTerminate();
	g_Instance = nullptr;
}


int BaseApplication::run() {
	// the time of the previous frame
	env.prev_time = env.time = glfwGetTime();
	env.dt = 0;
	// this just loops as long as the program runs
	glfwMakeContextCurrent(window);
	GLenum err = glGetError();


	while (!glfwWindowShouldClose(window))
	{
		// calculate time elapsed, and the amount by which stuff rotates
		env.prev_time = env.time;
		env.time = glfwGetTime();
		env.dt = env.time - env.prev_time;

		// escape to quit, arrow keys to rotate view
		//if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
		//	break;
		GLenum err = glGetError();

		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
		
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		//glDisable (GL_DEPTH_TEST);
		//glFrontFace(GL_CCW);
		glDisable(GL_DEPTH_TEST);

		glViewport(0, 0, 1024, 768);

		glClearColor(0.32f, 0.33f, 0.35f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (env.update_cb)
			env.update_cb();

		if (env.render_cb)
			env.render_cb();

		glfwSwapBuffers(window);
		glfwPollEvents();
		//Sleep(0);
	}

	return 0;
}