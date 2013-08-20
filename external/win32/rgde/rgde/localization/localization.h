#pragma once

#include <rgde/core/errors.h>

namespace rgde
{
	namespace localization		
	{
		struct base_localization_table
		{
			virtual ~base_localization_table() {}
			virtual const std::wstring& get_text(const std::string& id_string) const = 0;
		};

		typedef boost::shared_ptr<base_localization_table> localization_table_ptr;

		// любая подсистема движка, которой нужно иметь дело с языковой информацией
		// должна содержать в себе такую таблицу
		class localization_table : public base_localization_table
		{
			typedef std::map<std::string, std::wstring> string_table;
			typedef string_table::const_iterator string_table_it;
		public:
			localization_table();

			// вернет пустую строку если id не найден, альтернатива - вернуть text_id_string
			const std::wstring& get_text(const std::string& id_string) const;
			
			void add_text(const std::string& id_string, const std::wstring& text);

			void clear();

		protected:
			string_table m_table;
			const std::wstring m_empty_string;
		};

		class localization_system
		{
		public:
			// здесь грузятся таблицы локализаций все какие есть в файле локали.
			localization_system(const std::string& default_locale = "en");

			const std::string& get_locale() const {return m_current_locale;}
			void set_locale(const std::string& new_locale);

			localization_table_ptr get_table(const std::string& locale, const std::string& table_id);
			localization_table_ptr add_table(const localization_table_ptr& table, const std::string& locale,
											 const std::string& table_id);

			const std::wstring& get_text(const std::string& table_id, const std::string& id_string) const;

		protected:
			typedef std::map<std::string, localization_table_ptr> map_tables;
			typedef map_tables::const_iterator map_tables_citer;
			typedef std::map<std::string, typename map_tables> loc_tables;
			typedef loc_tables::iterator loc_tables_iter;

			localization_table_ptr m_empty_table;
			loc_tables m_tables;

			std::string m_current_locale;
			map_tables m_current_tables;
		};
	}
}