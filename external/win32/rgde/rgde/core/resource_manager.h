#pragma once

#include <rgde/core/errors.h>

#include <boost/call_traits.hpp>
#include <boost/smart_ptr.hpp>

#include <map>

namespace rgde
{
	namespace core
	{
		namespace utils
		{
			template<class ResourceType, typename KeyValue = std::string>
			class resouce_manager
			{
			public:
				typedef ResourceType resource;
				typedef KeyValue key_value;
				typedef typename boost::call_traits<key_value>::param_type key_param_type;
				typedef boost::shared_ptr<resource> resource_ptr;
				typedef boost::function<resource_ptr (const KeyValue& file_name)> resource_creator;

				resouce_manager(key_param_type default_resource_file_name, resource_creator creator)
					: m_creator(creator)
				{
					m_default_resource = m_creator(default_resource_file_name);
				}

				resouce_manager(resource_creator creator)
					: m_creator(creator)
				{

				}

				resource_ptr get(key_param_type file_name)
				{
					resource_ptr out;

					out = find(file_name);

					if (!out)
					{
						out = m_creator(file_name);
						if (!out && m_default_resource)
						{
							out = m_default_resource;
						}

						m_resources[file_name] = out;
					}

					return out;
				}

				resource_ptr find(key_param_type file_name) const 
				{
					const_resource_iter it = m_resources.find(file_name);
					if (it != m_resources.end())
					{
						return it->second;

					}
					return resource_ptr();
				}


			protected:
				resource_ptr	 m_default_resource;
				resource_creator m_creator;

				typedef std::map<key_value, resource_ptr> resource_map;
				//typedef resource_map::iterator resource_iter;
				typedef typename resource_map::const_iterator const_resource_iter;

				resource_map m_resources;
			};
		}
	}
}