#pragma once

namespace rgde
{
namespace input
{
    class input_impl;
    class device;
    class control;
    class command;

    typedef boost::shared_ptr<command> command_ptr;

	class helper;       //базовый класс обьекта-посредника для получения информации о событиях ввода
	class button;       //кнопка (нажатие и отжатие клавиш)
	class trigger;      //триггер (переключение состояния при каждом НАжатии клавиш)
	class key_up;        //ОТжатие клавиши
	class key_down;      //НАжатие на клавишу
	class relative_axis; //относительная ось (сдвиг оси)
	class absolute_axis; //абсолютная ось (координата оси)
	class char_stream;

	namespace types
	{
		//перечисление существующих устройств ввода
		enum EDevice
		{
			Keyboard,
			Mouse,
			Joystick,

			InvalidDevice,
		};

		//перечисление существующих элементов ввода
		enum EControl
		{
			KeyEscape,
			Key1,
			Key2,
			Key3,
			Key4,
			Key5,
			Key6,
			Key7,
			Key8,
			Key9,
			Key0,
			KeyMinus,
			KeyEquals,
			KeyBack,
			KeyTab,
			KeyQ,
			KeyW,
			KeyE,
			KeyR,
			KeyT,
			KeyY,
			KeyU,
			KeyI,
			KeyO,
			KeyP,
			KeyLBracket,
			KeyRBracket,
			KeyReturn,
			KeyLControl,
			KeyA,
			KeyS,
			KeyD,
			KeyF,
			KeyG,
			KeyH,
			KeyJ,
			KeyK,
			KeyL,
			KeySemicolon,
			KeyApostrophe,
			KeyGrave,
			KeyLShift,
			KeyBackSlash,
			KeyZ,
			KeyX,
			KeyC,
			KeyV,
			KeyB,
			KeyN,
			KeyM,
			KeyComma,
			KeyPeriod,
			KeySlash,
			KeyRShift,
			KeyMultiply,
			KeyLMenu,
			KeySpace,
			KeyCapital,
			KeyF1,
			KeyF2,
			KeyF3,
			KeyF4,
			KeyF5,
			KeyF6,
			KeyF7,
			KeyF8,
			KeyF9,
			KeyF10,
			KeyNumLock,
			KeyScroll,
			KeyNumPad7,
			KeyNumPad8,
			KeyNumPad9,
			KeySubtract,
			KeyNumPad4,
			KeyNumPad5,
			KeyNumPad6,
			KeyAdd,
			KeyNumPad1,
			KeyNumPad2,
			KeyNumPad3,
			KeyNumPad0,
			KeyDecimal,
			KeyOEM_102,
			KeyF11,
			KeyF12,
			KeyF13,
			KeyF14,
			KeyF15,
			KeyKana,
			KeyABNT_C1,
			KeyConvert,
			KeyNoConvert,
			KeyYen,
			KeyABNT_C2,
			KeyNumPadEquals,
			KeyPrevTrack,
			KeyAt,
			KeyColon,
			KeyUnderLine,
			KeyKANJI,
			KeyStop,
			KeyAx,
			KeyUnlabeled,
			KeyNextTrack,
			KeyNumPadEnter,
			KeyRControl,
			KeyMute,
			KeyCalculator,
			KeyPlayPause,
			KeyMediaStop,
			KeyVolumeDown,
			KeyVolumeUp,
			KeyWebHome,
			KeyNumPadComma,
			KeyDivide,
			KeySysRQ,
			KeyRMenu,
			KeyPause,
			KeyHome,
			KeyUp,
			KeyPrior,
			KeyLeft,
			KeyRight,
			KeyEnd,
			KeyDown,
			KeyNext,
			KeyInsert,
			KeyDelete,
			KeyLWin,
			KeyRWin,
			KeyApps,

			ButtonLeft,
			ButtonMid,
			ButtonRight,

			AxisX,
			AxisY,
			AxisWheel,

			InvalidControl
		};
	}

	struct type_to_string
	{
		static std::wstring device(types::EDevice type);
		static std::wstring control(types::EControl type);
	};

	struct string_to_type
	{
		static types::EDevice device(const std::wstring& type_name);
		static types::EControl control(const std::wstring& type_name);
	};
}
}