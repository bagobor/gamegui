#pragma once

#include <rgde/input/base.h>

namespace rgde
{
namespace input
{
    class control
    {
    public:        
        enum EType
        {
            button,
            axis
        };

        control(types::EControl name, EType type, device &device);

        types::EControl get_name () const {return m_name;}
		EType			get_type () const {return m_type;}
        
        void bind (command_ptr command);
        void bind (const std::wstring &command_name);

        void unbind (command_ptr command);
        void unbind (const std::wstring &command_name);

        bool is_bind (command_ptr command);
        bool is_bind (const std::wstring &command_name);

        void notify_all (); 

		bool is_pressed() const {return m_press;}

        int  m_time;
        bool m_press;
        int  m_delta;

    private:
        device          &m_device; //устройство, которому принадлежит контрол
        EType            m_type;   //тип контрола
        types::EControl  m_name;   //'имя' контрола

		typedef std::list<command_ptr> commands_list;
		typedef commands_list::iterator commands_iter;

		//команды, к которой привязан контрол
        commands_list m_commands;
    };
}
}