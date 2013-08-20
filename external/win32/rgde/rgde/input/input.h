#pragma once

#include <rgde/input/base.h>
#include <rgde/core/windows_forward.h>

namespace rgde
{
namespace input
{
	class input_impl;

	// основной класс системы ввода
	class system : boost::noncopyable
	{
	public:
		system(core::windows::window_ptr window, bool exclusive=false, bool foreground=true);
		~system();

        // функции самой системы ввода
        bool set_mode (bool exclusive=false, bool foreground=true); //изменить режим работы устройств ввода
        void load_from_string (const std::string &xml_str);      //загрузить раскладку
        void load_from_file   (const std::string &file_name); //загрузить раскладку
        void update();                        //считать из буфера все события от устройств ввода
        void save(std::string &xml_str);       //сохранить раскладку

        // доступ к устройствам ввода
        //получить устройство
        device* get_device (types::EDevice device_type, int indx=0);
        device* get_device (const std::wstring &device_name, int indx=0);

        //есть ли такое устройство
        bool is_device_present (types::EDevice device_type, int indx=0);
        bool is_device_present (const std::wstring &device_name, int indx=0);

		control* get_control(types::EDevice device, int dev_index, types::EControl control);
		control* get_control(types::EDevice device, types::EControl control)
		{
			return get_control(device, 0, control);
		}

        // доступ к командам системы ввода
        void add_command       (const std::wstring &command_name); //добавить команду
        command_ptr  get_command       (const std::wstring &command_name); //получить команду
        bool is_command_present (const std::wstring &command_name); //есть ли такая команда
        void detach_command    (command_ptr command);                //отвязать команду ото всех контролов

	private:
		std::auto_ptr<input_impl> m_impl;      //конкретная реализация системы ввода
	};
}
}