#include "StdAfx.h"
#include "window.h"
#include "factory.h"

#include "system.h"

namespace gui
{


WindowFactory::WindowFactory(System& sys) :
	m_system(sys)
{
}

WindowFactory::~WindowFactory(void)
{
}

void WindowFactory::RegisterCreator(const std::string& type, CreatorPtr creator)
{
	m_system.logEvent(log::system, std::string("Factory: adding ") + type);
	m_creators[type] = creator;
}

window_ptr WindowFactory::Create(const std::string& type, const std::string& name)
{
	try
	{
		if(type.empty())
			return window_ptr();

		typedef CreatorsMap::iterator CreatorsIt;

		CreatorsIt it = m_creators.find(type);

		if (it == m_creators.end())
		{
			m_system.logEvent(log::warning, std::string("Factory: couldn't create window ") + type);
			return window_ptr();
		}

		return (it->second)->Create(name);
	}
	catch (...)
	{
		m_system.logEvent(log::warning, std::string("Factory: exception occerud while creating window ") + type);
		return window_ptr();
	}
}

WindowFactory::TypesList WindowFactory::GetTypesList()
{
	WindowFactory::TypesList types_list;

	for (CreatorsConstIt it = m_creators.begin(); it != m_creators.end(); ++it)
		types_list.push_back(it->first);

	return types_list;
}

window_ptr RootCreator::Create(const std::string& name)
{
	return m_system.getRootPtr();
}

}
