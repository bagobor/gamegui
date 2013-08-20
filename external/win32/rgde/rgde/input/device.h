#pragma once

#include <rgde/input/base.h>

namespace rgde
{
namespace input
{
    //устройство ввода
    class device
    {
    public:		
        virtual ~device();

		//получить 'имя' устройства
        types::EDevice get_name () const {return m_device_type;}
		//получить порядковый номер устройства
        int		get_index () const {return m_index;}
		//получить устройство ввода
        input_impl& get_input()     {return m_input;}
		const input_impl& get_input() const {return m_input;}

		//получить контрол
        control* get_control       (types::EControl	contol_type);
        control* get_control       (const std::wstring &control_name);

		//есть ли такой контрол
        bool is_control_present (types::EControl	contol_type) const;
        bool is_control_present (const std::wstring &control_name) const;

    protected:
		device(types::EDevice name, int index, input_impl &input_system);

        friend class input_impl;
        void add_button (types::EControl contol_type); //добавить кнопку
        void add_axis   (types::EControl contol_type); //добавить ось
        void detach_command (command_ptr command); //отвязать команду ото всех контролов

    private:
        int         m_index;
        types::EDevice m_device_type;
        input_impl &m_input;

        std::map<types::EControl, control*> m_controls; //контролы, которые есть у устройства
    };
}
}