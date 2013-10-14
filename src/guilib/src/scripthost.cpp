#include "stdafx.h"

#include "scripthost.h"
#include "scriptobject.h"
#include "system.h"

extern "C"
{
	#include <lstate.h>
	#include <lauxlib.h>
	#include <lua.h>	
	#include <lualib.h>
}


namespace gui
{

void ScriptObject::thisreset(lua_State* state)
{
	if(state)
		luabind::globals(state)["this"] = 0;
}

ScriptStack::ScriptStack()
{
	m_stack.reserve(16);
}
void ScriptStack::clear()
{
	m_stack.clear();
}
void ScriptStack::push(ScriptObject* obj)
{
	assert(obj);
	obj->thisset();
	m_stack.push_back(obj);
}
void ScriptStack::pop(lua_State* state)
{
	if(m_stack.size())
	{
		size_t size = m_stack.size();
		if(size == 1)
			m_stack[size - 1]->thisreset(state);

		m_stack.pop_back();
		size = m_stack.size();
		if(size)
			m_stack[size - 1]->thisset();
	}
}

ScriptSystem::ScriptSystem(filesystem_ptr fs, lua_State* externalState)
: m_state(externalState)
, m_ext(true)
, m_filesystem(fs)
{
	if(!m_state)
	{
		m_state = luaL_newstate();
		m_ext = false;
		if (m_state)
		{
			luaL_openlibs(m_state);
		}
	}
	assert(m_state);
	using namespace luabind;
	open(m_state);
	module(m_state)
		[
			class_<ScriptObject>("ScriptObject")
		];
}
ScriptSystem::~ScriptSystem()
{
	if(!m_ext)
		lua_close(m_state);
}

lua_State* ScriptSystem::LuaState()
{
	return m_state;
}

bool ScriptSystem::ExecuteString(const std::string& script, ScriptObject* obj, const std::string& filename)
{
	bool retval = false;
	if(obj)
	{
		m_thisStack.push(obj);
		retval = ExecuteString(script, filename);
		m_thisStack.pop(m_state);
	}
	else
	{
		m_error = "An empty object passed to the script. ";
	}
	return retval;
}

bool ScriptSystem::ExecuteString(const std::string& script, const std::string& filename)
{
	bool success = false;
	if(m_state)
	{
		if(int result = luaL_loadbuffer(m_state, script.c_str(), script.length(), filename.c_str()))
		{
			m_error = "Can't parse LUA string: ";
			m_error += GetLuaError();
			return false;
		}

		try
		{
			success = Execute(script, filename);
		}
		catch(std::exception& e)
		{
			m_error = "LUA runtime error: ";
			m_error += e.what();
		}
	}
	else
	{
		m_error = "LUA state is uninitialized. ScriptSystem isn't available. ";
	}
	return success;
}

bool ScriptSystem::Execute(const std::string& script, const std::string& filename)
{
	if(int result = lua_pcall(m_state, 0, 0, 0))
	{
		m_error = "Can't execute LUA string: ";
		m_error += GetLuaError();
		return false;
	}
	return true;
}

bool ScriptSystem::ExecuteFile(const std::string& filename)
{
	const std::string& script = LoadFile(filename);
	if(!script.empty())
	{
		m_error = "The file '";
		m_error += filename;
		m_error += "' cannot be loaded. ";
		return ExecuteString(script, filename);
	}
	
	return true; // just empty file
}

std::string ScriptSystem::LoadFile(const std::string& filename)
{
	std::string str = m_filesystem->load_text(filename);
	return str;
}

std::string ScriptSystem::GetLuaError()
{
	std::string ret = "<unknown>";
	if(m_state && lua_isstring(m_state, -1))
	{
		ret = lua_tostring(m_state, -1);
		lua_pop(m_state, 1);
	}
	return ret;
}

}