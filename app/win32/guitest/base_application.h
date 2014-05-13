#pragma once

#include <functional>


struct _env{
	double time;
	double prev_time;
	double dt;

	std::function<void(int, int)> mouse_move_cb;
	std::function<void(int, int)> mouse_button_cb;
	std::function<void(int, int)> mouse_wheel_cb;
	std::function<void()> update_cb;
	std::function<void()> render_cb;
	std::function<void(int, int)> resize_cb;
	//std::function<void(int, int)> mouse_wheel;
	//etc
};

extern _env env;

struct GLFWwindow;

class BaseApplication {
	static BaseApplication* g_Instance;// = 0;

	static void _OnWindowsizefun(GLFWwindow*, int w, int h) {
		g_Instance->onWindowSize(w,h);
	}

	static void _OnMousebuttonfun(GLFWwindow*, int button, int action, int mods) {
		g_Instance->onMousebutton(button, action);
	}

	static void _OnMouseposfun(GLFWwindow*, double x, double y) {
		g_Instance->onMousepos(x,y);
	}

	static void _OnKeyfun(GLFWwindow*, int key, int scancode, int action, int mods) {
		g_Instance->onKey(key, action);
	}

	static int  _OnWindowclosefun(void);
	static void _OnWindowrefreshfun(void);
	static void _OnMousewheelfun(int delta);	
	static void _OnCharfun( int character, int action );
	
public:
	int run();
	BaseApplication(size_t w, size_t h, const char* title);
	~BaseApplication();

protected:

	GLFWwindow* window;

	virtual void onWindowSize(int w,int h) {}
	virtual int  onWindowclose(void) { return 0;}
	virtual void onWindowrefresh(void) {}
	virtual void onMousebutton(int button, int action) {}
	virtual void onMousepos(int x, int y) {}
	virtual void onMousewheel(int delta) {}
	virtual void onKey(int key, int action ) {}
	virtual void onChar( int character, int action ) {}
	
	bool m_ready;
};