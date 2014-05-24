#include "stdafx.h"
#include "base.h"

named_object::named_object(const std::string& name) 
	: m_strName(name)
{
}

const std::string& named_object::getName() const 
{
	return m_strName;
}

void named_object::setName(const std::string& name)
{
	m_strName = name;
}

//registry::types_t registry::types;