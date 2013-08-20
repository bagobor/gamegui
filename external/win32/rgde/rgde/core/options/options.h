#pragma once

//TODO: 

namespace rgde
{
	class options
	{
	public:
		struct param
		{
			std::string m_name;
			std::string m_value;

			template<class T>
			T cast() const 
			{
				return boost::lexical_cast<T>(m_value);
			}

			template<class T>
			T try_cast(T& def_value = T()) const 
			{
				try
				{
					return boost::lexical_cast<T>(m_value);
				}
				catch (boost::bad_lexical_cast&)
				{
					return def_value;
				}	
			}
		};

		struct section
		{
			struct section_finder
			{
				//TODO:
			};

			struct section_sorter
			{
				bool operator()(const section& l, const section& r)
				{
					return l.m_name < r.m_name;
				}
			};

			std::string m_name;
			std::vector<section> m_sub_sections;
			std::vector<param> m_params;

			void sort()
			{
				std::sort(m_sub_sections.begin(), m_sub_sections.end(), section_sorter());
			}

			section& operator[](const std::string& section_name)
			{
				return *m_sub_sections.begin();
			}
		};

		void load(const std::string& filename);
		void save(const std::string& filename);

	private:
		section m_root_section;
		section m_empty_section;
	};
}