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
	ScriptObjectBase::ScriptObjectBase(ScriptSystem& script_system)
		: m_script_system(script_system), m_state(script_system.getLuaState())  {
	}

	

	void ScriptObjectBase::thisreset(lua_State* state) {
		if (!state) return;
		//m_localtable = luabind::globals(state)["this"];
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
void ScriptStack::push(ScriptObjectBase* obj)
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
	reset(externalState);
}

void ScriptSystem::reset(lua_State* externalState) {
	if (!m_ext && m_state)
		lua_close(m_state);

	m_state = externalState;

	if (!m_state)
	{
		m_state = luaL_newstate();
		m_ext = false;
		if (m_state)
		{
			luaL_openlibs(m_state);
		}
	}
	assert(m_state);

	luabind::open(m_state);
	luabind::module(m_state)
		[
			luabind::class_<ScriptObjectBase>("ScriptObject")
		];
}

ScriptSystem::~ScriptSystem()
{
	if(!m_ext)
		lua_close(m_state);
}

lua_State* ScriptSystem::getLuaState()
{
	return m_state;
}

bool ScriptSystem::ExecuteString(const std::string& script, ScriptObjectBase* obj, const std::string& filename)
{
	if (!obj) {
		m_error = "An empty object passed to the script. ";
		return false;
	}
	m_thisStack.push(obj);
	bool retval = ExecuteString(script, filename);
	m_thisStack.pop(m_state);
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

bool ScriptSystem::ExecuteFile(const std::string& filename, ScriptObjectBase* obj) {
	const std::string& script = LoadFile(filename);
	if (script.empty()) return true;

	m_error = "The file '";
	m_error += filename;
	m_error += "' cannot be loaded. ";
		
	return ExecuteString(script, obj, filename);
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