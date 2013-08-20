#pragma once

namespace gui
{
	class Renderer;
	class System;
	class Font;
	typedef boost::shared_ptr<Font> FontPtr;
}

struct lua_State;

class TestWindow : public rgde::core::windows::window
{
public:
	TestWindow(int x, int y, int w, int h, const std::wstring& title);
	~TestWindow();

	void run();

	void createGUISystem();
	void resetGUISystem();

	virtual bool isFinished();

	bool isInitialized() { return m_system != 0; }

	void handleViewportChange();

	void load(const std::string& xml);

	virtual HRESULT OnResetDevice();
	virtual void	OnLostDevice();

protected:
	void reinit();

	void resize_scene(unsigned int width, unsigned int height);
	bool do_events();

	void update(float delta);
	void render();

	virtual rgde::core::windows::result wnd_proc(rgde::ushort message, rgde::uint wparam, long lparam );

protected:
	std::string m_filename;

	rgde::render::device m_render_device;
	rgde::core::vfs::system m_filesystem;

	gui::Renderer* m_render;
	gui::System* m_system;

	unsigned int m_framecount;
	HANDLE m_hFile;

	bool m_active;
	double m_elapsed;
	boost::timer m_timer;

	lua_State* m_state;

	gui::FontPtr m_font;
};
