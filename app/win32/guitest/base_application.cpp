#include "base_application.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>

struct glfw_helper
{
	glfw_helper() {
		if (!glfwInit())
			exit(EXIT_FAILURE);
	}

	~glfw_helper() {
		glfwTerminate();
	}
} _glfw_helper;

BaseApplicationGLFW::BaseApplicationGLFW(size_t w, size_t h, const char* title) 
: window(nullptr), m_width(w), m_height(h) {
	prev_time = time = dt = 0;

	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	//glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 16);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(m_width, m_height, title, NULL, NULL);
	if (!window)
	{
		exit(EXIT_FAILURE);
	}
	
	glfwSetWindowUserPointer(window, this);

	glfwSwapInterval(1);	
	glfwMakeContextCurrent(window);

	// Window resize callback function
	glfwSetWindowSizeCallback(window, OnWindowsizefun );
	//// Set keyboard input callback function
	glfwSetKeyCallback(window, OnKeyfun);
	glfwSetMouseButtonCallback(window, OnMousebuttonfun);
	glfwSetCursorPosCallback(window, OnMouseposfun);
}

BaseApplicationGLFW::~BaseApplicationGLFW() {
	glfwDestroyWindow(window);
}

void BaseApplicationGLFW::onWindowSize(int w, int h) {
	m_width = w;
	m_height = h;
}

int BaseApplicationGLFW::run() {
	// the time of the previous frame
	prev_time = time = glfwGetTime();
	dt = 0;
	// this just loops as long as the program runs
	glfwMakeContextCurrent(window);
	GLenum err = glGetError();


	while (!glfwWindowShouldClose(window))
	{
		// calculate time elapsed, and the amount by which stuff rotates
		prev_time = time;
		time = glfwGetTime();
		dt = time - prev_time;

		// escape to quit, arrow keys to rotate view
		//if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
		//	break;
		GLenum err = glGetError();
	
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glViewport(0, 0, m_width, m_height);

		glClearColor(0.32f, 0.33f, 0.35f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update();
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
		//Sleep(0);
	}

	return 0;
}

void BaseApplicationGLFW::OnWindowsizefun(GLFWwindow* window, int w, int h) {
	BaseApplicationGLFW* app = static_cast<BaseApplicationGLFW*>(glfwGetWindowUserPointer(window));
	app->onWindowSize(w, h);
}

void BaseApplicationGLFW::OnMousebuttonfun(GLFWwindow* window, int button, int action, int mods) {
	BaseApplicationGLFW* app = static_cast<BaseApplicationGLFW*>(glfwGetWindowUserPointer(window));
	app->onMousebutton(button, action);
}

void BaseApplicationGLFW::OnMouseposfun(GLFWwindow* window, double x, double y) {
	BaseApplicationGLFW* app = static_cast<BaseApplicationGLFW*>(glfwGetWindowUserPointer(window));
	app->onMousepos(x, y);
}

void BaseApplicationGLFW::OnKeyfun(GLFWwindow* window, int key, int scancode, int action, int mods) {
	BaseApplicationGLFW* app = static_cast<BaseApplicationGLFW*>(glfwGetWindowUserPointer(window));
	app->onKey(key, action);
}