#pragma once

namespace gui
{

class  EventArgs
{
public:
	enum ButtonState 
	{
		Up, Down
	};

	enum MouseButtons
	{
		Left	= 1,
		Middle	= 2,
		Right	= 4	
	};

	enum Keys
	{
		K_LBUTTON = 0x01, //Left mouse button
		K_RBUTTON = 0x02, //Right mouse button
		K_CANCEL = 0x03, //Control-break processing
		K_MBUTTON = 0x04, //Middle mouse button (three-button mouse)
		K_XBUTTON1 = 0x05, //Windows 2000/XP: X1 mouse button
		K_XBUTTON2 = 0x06, //Windows 2000/XP: X2 mouse button
		K_BACK = 0x08, //BACKSPACE key
		K_TAB = 0x09, //TAB key
		K_CLEAR = 0x0C, //CLEAR key
		K_RETURN = 0x0D, //ENTER key
		K_SHIFT = 0x10, //SHIFT key
		K_CONTROL = 0x11, //CTRL key
		K_MENU = 0x12, //ALT key
		K_PAUSE = 0x13, //PAUSE key
		K_CAPITAL = 0x14, //CAPS LOCK key
		K_KANA = 0x15, //Input Method Editor (IME) Kana mode
		K_HANGUEL = 0x15, //IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
		K_HANGUL = 0x15, //IME Hangul mode
		K_JUNJA = 0x17, //IME Junja mode
		K_FINAL = 0x18, //IME final mode
		K_HANJA = 0x19, //IME Hanja mode
		K_KANJI = 0x19, //IME Kanji mode
		K_ESCAPE = 0x1B, //ESC key
		K_CONVERT = 0x1C, //IME convert
		K_NONCONVERT = 0x1D, //IME nonconvert
		K_ACCEPT = 0x1E, //IME accept
		K_MODECHANGE = 0x1F, //IME mode change request
		K_SPACE = 0x20, //SPACEBAR
		K_PRIOR = 0x21, //PAGE UP key
		K_NEXT = 0x22, //PAGE DOWN key
		K_END = 0x23, //END key
		K_HOME = 0x24, //HOME key
		K_LEFT = 0x25, //LEFT ARROW key
		K_UP = 0x26, //UP ARROW key
		K_RIGHT = 0x27, //RIGHT ARROW key
		K_DOWN = 0x28, //DOWN ARROW key
		K_SELECT = 0x29, //SELECT key
		K_PRINT = 0x2A, //PRINT key
		K_EXECUTE = 0x2B, //EXECUTE key
		K_SNAPSHOT = 0x2C, //PRINT SCREEN key
		K_INSERT = 0x2D, //INS key
		K_DELETE = 0x2E, //DEL key
		K_HELP = 0x2F, //HELP key
		K_0 = 0x30, //0 key
		K_1 = 0x31, //1 key
		K_2 = 0x32, //2 key
		K_3 = 0x33, //3 key
		K_4 = 0x34, //4 key
		K_5 = 0x35, //5 key
		K_6 = 0x36, //6 key
		K_7 = 0x37, //7 key
		K_8 = 0x38, //8 key
		K_9 = 0x39, //9 key
		K_A = 0x41, //A key
		K_B = 0x42, //B key
		K_C = 0x43, //C key
		K_D = 0x44, //D key
		K_E = 0x45, //E key
		K_F = 0x46, //F key
		K_G = 0x47, //G key
		K_H = 0x48, //H key
		K_I = 0x49, //I key
		K_J = 0x4A, //J key
		K_K = 0x4B, //K key
		K_L = 0x4C, //L key
		K_M = 0x4D, //M key
		K_N = 0x4E, //N key
		K_O = 0x4F, //O key
		K_P = 0x50, //P key
		K_Q = 0x51, //Q key
		K_R = 0x52, //R key
		K_S = 0x53, //S key
		K_T = 0x54, //T key
		K_U = 0x55, //U key
		K_V = 0x56, //V key
		K_W = 0x57, //W key
		K_X = 0x58, //X key
		K_Y = 0x59, //Y key
		K_Z = 0x5A, //Z key
		K_LWIN  = 0x5B, //Left Windows key (Microsoft Natural keyboard)
		K_RWIN  = 0x5C, //Right Windows key (Natural keyboard)
		K_APPS  = 0x5D, //Applications key (Natural keyboard)
		K_SLEEP  = 0x5F, //Computer Sleep key
		K_NUMPAD0  = 0x60, //Numeric keypad 0 key
		K_NUMPAD1  = 0x61, //Numeric keypad 1 key
		K_NUMPAD2  = 0x62, //Numeric keypad 2 key
		K_NUMPAD3  = 0x63, //Numeric keypad 3 key
		K_NUMPAD4  = 0x64, //Numeric keypad 4 key
		K_NUMPAD5  = 0x65, //Numeric keypad 5 key
		K_NUMPAD6  = 0x66, //Numeric keypad 6 key
		K_NUMPAD7  = 0x67, //Numeric keypad 7 key
		K_NUMPAD8  = 0x68, //Numeric keypad 8 key
		K_NUMPAD9  = 0x69, //Numeric keypad 9 key
		K_MULTIPLY  = 0x6A, //Multiply key
		K_ADD  = 0x6B, //Add key
		K_SEPARATOR  = 0x6C, //Separator key
		K_SUBTRACT  = 0x6D, //Subtract key
		K_DECIMAL  = 0x6E, //Decimal key
		K_DIVIDE  = 0x6F, //Divide key
		K_F1  = 0x70, //F1 key
		K_F2  = 0x71, //F2 key
		K_F3  = 0x72, //F3 key
		K_F4  = 0x73, //F4 key
		K_F5  = 0x74, //F5 key
		K_F6  = 0x75, //F6 key
		K_F7  = 0x76, //F7 key
		K_F8  = 0x77, //F8 key
		K_F9  = 0x78, //F9 key
		K_F10  = 0x79, //F10 key
		K_F11  = 0x7A, //F11 key
		K_F12  = 0x7B, //F12 key
		K_F13  = 0x7C, //F13 key
		K_F14  = 0x7D, //F14 key
		K_F15  = 0x7E, //F15 key
		K_F16  = 0x7F, //F16 key
		K_F17  = 0x80, //F17 key
		K_F18  = 0x81, //F18 key
		K_F19  = 0x82, //F19 key
		K_F20  = 0x83, //F20 key
		K_F21  = 0x84, //F21 key
		K_F22  = 0x85, //F22 key
		K_F23  = 0x86, //F23 key
		K_F24  = 0x87, //F24 key
		K_NUMLOCK  = 0x90, //NUM LOCK key
		K_SCROLL  = 0x91, //SCROLL LOCK key
		K_LSHIFT  = 0xA0, //Left SHIFT key
		K_RSHIFT  = 0xA1, //Right SHIFT key
		K_LCONTROL  = 0xA2, //Left CONTROL key
		K_RCONTROL  = 0xA3, //Right CONTROL key
		K_LMENU  = 0xA4, //Left MENU key
		K_RMENU  = 0xA5, //Right MENU key
		K_BROWSER_BACK  = 0xA6, //Windows 2000/XP: Browser Back key
		K_BROWSER_FORWARD  = 0xA7, //Windows 2000/XP: Browser Forward key
		K_BROWSER_REFRESH  = 0xA8, //Windows 2000/XP: Browser Refresh key
		K_BROWSER_STOP  = 0xA9, //Windows 2000/XP: Browser Stop key
		K_BROWSER_SEARCH  = 0xAA, //Windows 2000/XP: Browser Search key 
		K_BROWSER_FAVORITES  = 0xAB, //Windows 2000/XP: Browser Favorites key
		K_BROWSER_HOME  = 0xAC, //Windows 2000/XP: Browser Start and Home key
		K_VOLUME_MUTE  = 0xAD, //Windows 2000/XP: Volume Mute key
		K_VOLUME_DOWN  = 0xAE, //Windows 2000/XP: Volume Down key
		K_VOLUME_UP  = 0xAF, //Windows 2000/XP: Volume Up key
		K_MEDIA_NEXT_TRACK  = 0xB0, //Windows 2000/XP: Next Track key
		K_MEDIA_PREV_TRACK  = 0xB1, //Windows 2000/XP: Previous Track key
		K_MEDIA_STOP  = 0xB2, //Windows 2000/XP: Stop Media key
		K_MEDIA_PLAY_PAUSE  = 0xB3, //Windows 2000/XP: Play/Pause Media key
		K_LAUNCH_MAIL  = 0xB4, //Windows 2000/XP: Start Mail key
		K_LAUNCH_MEDIA_SELECT  = 0xB5, //Windows 2000/XP: Select Media key
		K_LAUNCH_APP1  = 0xB6, //Windows 2000/XP: Start Application 1 key
		K_LAUNCH_APP2  = 0xB7, //Windows 2000/XP: Start Application 2 key
		K_OEM_1  = 0xBA, //Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ';:' key 
		K_OEM_PLUS  = 0xBB, //Windows 2000/XP: For any country/region, the '+' key
		K_OEM_COMMA  = 0xBC, //Windows 2000/XP: For any country/region, the ',' key
		K_OEM_MINUS  = 0xBD, //Windows 2000/XP: For any country/region, the '-' key
		K_OEM_PERIOD  = 0xBE, //Windows 2000/XP: For any country/region, the '.' key
		K_OEM_2  = 0xBF, //Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '/?' key 
		K_OEM_3  = 0xC0, //Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '`~' key 
		K_OEM_4  = 0xDB, //Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '[{' key
		K_OEM_5  = 0xDC, //Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '\|' key
		K_OEM_6  = 0xDD, //Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ']}' key
		K_OEM_7  = 0xDE, //Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the 'single-quote/double-quote' key
		K_OEM_8  = 0xDF, //Used for miscellaneous characters; it can vary by keyboard.
		K_OEM_102  = 0xE2, //Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard
		K_PROCESSKEY  = 0xE5, //Windows 95/98/Me, Windows NT 4.0, Windows 2000/XP: IME PROCESS key
		K_PACKET  = 0xE7, //Windows 2000/XP: Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
		K_ATTN  = 0xF6, //Attn key
		K_CRSEL  = 0xF7, //CrSel key
		K_EXSEL  = 0xF8, //ExSel key
		K_EREOF  = 0xF9, //Erase EOF key
		K_PLAY  = 0xFA, //Play key
		K_ZOOM  = 0xFB, //Zoom key
		K_PA1  = 0xFD, //PA1 key
		K_OEM_CLEAR  = 0xFE //Clear key
	};

	EventArgs() : handled(false), name(nullptr) {}
	EventArgs(const char* _name) : handled(false), name(_name)
	{
	}

	virtual ~EventArgs() {}

	const char* name;
	bool handled;
};

class  MouseEventArgs : public EventArgs
{
public:
	MouseEventArgs() {}
	virtual ~MouseEventArgs() {}
	
	MouseButtons mbutton;
	ButtonState mstate;
	float mouseX;
	float mouseY;
	int wheelDelta;
};

class  MarkupEventArgs : public MouseEventArgs
{
public:
	MarkupEventArgs() {}
	virtual ~MarkupEventArgs() {}
	
	unsigned int id;
	const char* type;
};

class  MenuEventArgs : public MouseEventArgs
{
public:
	MenuEventArgs() {}
	virtual ~MenuEventArgs() {}
	
	unsigned int id;
};

class  KeyEventArgs : public EventArgs
{
public:
	KeyEventArgs() {}
	virtual ~KeyEventArgs() {}
	
	EventArgs::Keys button;
	ButtonState state;
	const wchar_t* ch;
};

class WindowBase;

class  DragEventArgs : public EventArgs
{
public:
	DragEventArgs() {}
	virtual ~DragEventArgs() {}
	
	point offset;	
	WindowBase*	subj;
};

class  TickEventArgs : public EventArgs
{
public:
	TickEventArgs() {}
	virtual ~TickEventArgs() {}
	
	float delta;
};

class  BinderEventArgs : public EventArgs
{
public:
	BinderEventArgs() : isMouse(false), isKbd(false), isWheel(false), systemkey(0), doubleClick(false) {}
	virtual ~BinderEventArgs() {}
	
	bool isMouse;
	bool isKbd;
	bool isWheel;
	bool doubleClick;
	
	Keys kbutton;
	MouseButtons mbutton;
	size_t systemkey;
	int mwheel;
};

template <typename T> struct event_caster
{
	static T* apply (EventArgs* w)
	{
		return dynamic_cast <T*> (w);
	}
};

}