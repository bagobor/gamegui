#pragma once

struct GLFWwindow;

class BaseApplicationGLFW {
public:	
	BaseApplicationGLFW(size_t w, size_t h, const char* title);
	virtual ~BaseApplicationGLFW();

	int run();

	size_t width() const { return m_width; }
	size_t height() const { return m_height; }

	inline double frameDt() const { return dt; }
	inline double frameTime() const { return time; }
	inline double prevFrameTime() const { return prev_time; }

protected:
	virtual void render() {}
	virtual void update() {}

	virtual void onWindowSize(int w, int h);
	virtual int  onWindowclose(void) { return 0;}
	virtual void onWindowrefresh(void) {}
	virtual void onMousebutton(int /*button*/, int /*action*/) {}
	virtual void onMousepos(int /*x*/, int /*y*/) {}
	virtual void onMousewheel(int /*delta*/) {}
	virtual void onKey(int /*key*/, int /*action*/ ) {}
	virtual void onChar( int /*character*/, int /*action*/ ) {}

private:
	static void OnWindowsizefun(GLFWwindow*, int w, int h);
	static void OnMousebuttonfun(GLFWwindow*, int button, int action, int mods);
	static void OnMouseposfun(GLFWwindow*, double x, double y);
	static void OnKeyfun(GLFWwindow*, int key, int scancode, int action, int mods);
	static int  OnWindowclosefun(void);
	static void OnWindowrefreshfun(void);
	static void OnMousewheelfun(int delta);
	static void OnCharfun(int character, int action);
	
	GLFWwindow* window;
	size_t m_width, m_height;

	double time;		/// current frame time
	double prev_time;	/// prev. frame time
	double dt;			/// frame delta time
};