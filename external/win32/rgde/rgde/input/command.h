#pragma once

namespace rgde
{
namespace input
{
	class input_impl;
	class helper;

    //команда системы ввода
    class command
    {
    public:
        command(const std::wstring &name, input_impl &input);

        const std::wstring& get_name() const;
        void lock();
        void unlock();
        bool is_locked() const;

    protected:
        friend class control;
		// notify all observers
        void notify_all (const control &control);

    protected:
		friend class helper;
        void attach (helper *helper);
        void detach (helper *helper);

    private:
        bool          m_locked;
        input_impl&	  m_input;
        std::wstring  m_name;

		typedef std::list<helper*> helpers_list;
		typedef helpers_list::iterator helpers_iter;

        helpers_list m_helpers;
    };

	inline const std::wstring& command::get_name() const 
	{
		return m_name;
	}

	inline void command::lock() 
	{
		m_locked = true;
	}

	inline void command::unlock() 
	{
		m_locked = false;
	}

	inline bool command::is_locked() const 
	{
		return m_locked;
	}
}
}