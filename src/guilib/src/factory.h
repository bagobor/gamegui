#pragma once

namespace xml {
class node;
};

namespace gui
{

class System;
class WindowBase;
typedef std::shared_ptr<WindowBase> window_ptr;

struct base_creator
{
	base_creator(System& sys, const char* type)
		: m_system(sys), m_typename(type)
	{
	}
	virtual ~base_creator(){}
	virtual window_ptr Create(const std::string& name) = 0;

	System& m_system;
	const char* m_typename;
};

template<class T>
struct Creator : public base_creator
{
	explicit Creator(System& sys) : base_creator(sys, T::GetType()) {}
	virtual window_ptr Create(const std::string& name)
	{
		return window_ptr(new T(m_system, name));
	}
};

struct RootCreator : public base_creator
{
	RootCreator(System& sys, const char* type_name) : base_creator(sys, type_name) {}
	virtual window_ptr Create(const std::string& name);	
};

typedef std::shared_ptr<base_creator> CreatorPtr;
class WindowFactory
{
public:
	typedef std::map<std::string, CreatorPtr> CreatorsMap;
	typedef CreatorsMap::const_iterator CreatorsConstIt;
	typedef std::list<std::string> TypesList;

	explicit WindowFactory(System& sys);
	virtual ~WindowFactory();

	void registerCreator(const std::string& type, CreatorPtr creator);

	template<typename T>
	void registerCreator()
	{
		registerCreator(T::GetType(), CreatorPtr(new Creator<T>(m_system)));
	}

	window_ptr Create(const std::string& type, const std::string& name);
	
	TypesList getTypesList();
	
protected:
	CreatorsMap m_creators;
	System& m_system;
};

}