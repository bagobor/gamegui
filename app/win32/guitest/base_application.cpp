//#include "stdafx.h"

#include "base_application.h"

#include <GLFW/glfw3.h>

//extern void init_gl();

_env env = { 0 };
BaseApplication* BaseApplication::g_Instance;// = 0;

BaseApplication::BaseApplication(size_t w, size_t h, const char* title) 
: m_ready(false), window(nullptr) {
	g_Instance = this;

	if (glfwInit() != GL_TRUE) return; 
		
	{
		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
		glfwWindowHint(GLFW_DEPTH_BITS, 16);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//#define GLFW_CLIENT_API             0x00022001
//#define GLFW_CONTEXT_VERSION_MAJOR  0x00022002
//#define GLFW_CONTEXT_VERSION_MINOR  0x00022003
//#define GLFW_CONTEXT_REVISION       0x00022004
//#define GLFW_CONTEXT_ROBUSTNESS     0x00022005
//#define GLFW_OPENGL_FORWARD_COMPAT  0x00022006
//#define GLFW_OPENGL_DEBUG_CONTEXT   0x00022007
//#define GLFW_OPENGL_PROFILE         0x00022008
//
//#define GLFW_OPENGL_API             0x00030001
//#define GLFW_OPENGL_ES_API          0x00030002
//
//#define GLFW_NO_ROBUSTNESS                   0
//#define GLFW_NO_RESET_NOTIFICATION  0x00031001
//#define GLFW_LOSE_CONTEXT_ON_RESET  0x00031002
//
//#define GLFW_OPENGL_ANY_PROFILE              0
//#define GLFW_OPENGL_CORE_PROFILE    0x00032001
//#define GLFW_OPENGL_COMPAT_PROFILE  0x00032002

		window = glfwCreateWindow(w, h, title, NULL, NULL);
		/*Shut_Down(1);*/

		//glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 12);

		//#define GLFW_STEREO               0x00020011
		//glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE,0);
		//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR,4);
		//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR,2);
		//glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

		//// Disable VSync (we want to get as high FPS as possible!)
		//glfwSwapInterval( 1 );

		//GLFWAPI void GLFWAPIENTRY glfwSetWindowSizeCallback( GLFWwindowsizefun cbfun );
		//GLFWAPI void GLFWAPIENTRY glfwSetWindowCloseCallback( GLFWwindowclosefun cbfun );
		//GLFWAPI void GLFWAPIENTRY glfwSetWindowRefreshCallback( GLFWwindowrefreshfun cbfun );

/*
		glfwSetKeyCallback( GLFWkeyfun cbfun );
		glfwSetCharCallback( GLFWcharfun cbfun );
		glfwSetMouseButtonCallback( GLFWmousebuttonfun cbfun );
		glfwSetMousePosCallback( GLFWmouseposfun cbfun );
		glfwSetMouseWheelCallback( GLFWmousewheelfun cbfun );

		void glfwSetWindowCloseCallback( GLFWwindowclosefun cbfun )*/

		//glfwSetWindowTitle(title);
		if (!glfwInit())
			exit(EXIT_FAILURE);

		//if (glfwOpenWindow(w, h, 8, 8, 8, 0, 24, 0, GLFW_WINDOW) != GL_TRUE) return;
			//Shut_Down(1);
		if (!window)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
			//Shut_Down(1);
		}

		glfwMakeContextCurrent(window);

		// Window resize callback function
		glfwSetWindowSizeCallback(window, _OnWindowsizefun );
		//// Set keyboard input callback function
		glfwSetKeyCallback(window, _OnKeyfun);

		glfwSetMouseButtonCallback(window, _OnMousebuttonfun);

		glfwSetCursorPosCallback(window, _OnMouseposfun);

		//glfwEnable(GLFW_STICKY_MOUSE_BUTTONS);
		//glfwDisable(GLFW_MOUSE_CURSOR);

		//#define GLFW_MOUSE_CURSOR         0x00030001
		//#define GLFW_STICKY_KEYS          0x00030002
		//#define GLFW_STICKY_MOUSE_BUTTONS 0x00030003

		//init_gl();
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

	while (!glfwWindowShouldClose(window))
	{
		// calculate time elapsed, and the amount by which stuff rotates
		env.prev_time = env.time;
		env.time = glfwGetTime();
		env.dt = env.time - env.prev_time;

		// escape to quit, arrow keys to rotate view
		//if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
		//	break;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
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