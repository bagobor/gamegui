#pragma once

#include <string>
#include <exception>

namespace rgde
{
	namespace core
	{
		namespace errors
		{
			class core_exception : public std::exception
			{
			public:
				explicit core_exception(const std::string& error_message, const std::string& module_name = "core");

				virtual const char * what() const;
				virtual const char * module() const;
				virtual const char * error_message() const;

			protected:
				std::string m_full_message;
				std::string m_error_message;
				std::string m_module_name;
			};

			class not_implemented : public core_exception
			{
			public:
				explicit not_implemented(const std::string& method);

			protected:
			};
		}
	}
}