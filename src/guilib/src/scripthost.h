#pragma once

#include <memory>

struct lua_State;

namespace gui
{
	struct  ScriptObjectBase;
	struct filesystem;
	typedef std::shared_ptr<filesystem> filesystem_ptr;

	class ScriptStack
	{
	public:
		ScriptStack();
		void clear();
		void push(ScriptObjectBase* obj);
		void pop(lua_State* state);
	private:
		std::vector<ScriptObjectBase*> m_stack;
	};

	class  ScriptSystem
	{
	public:
		/// @brief - ctor
		/// @param externalState - to provide external luaState, 0 - to use own one.
		ScriptSystem(filesystem_ptr fs, lua_State* externalState = NULL);
		~ScriptSystem();

		lua_State* getLuaState();

		bool ExecuteString(const std::string& script, ScriptObjectBase* obj, const std::string& filename = "unknown event");
		bool ExecuteString(const std::string& script, const std::string& filename = "mem buffer");
		bool ExecuteFile(const std::string& filename);
		bool ExecuteFile(const std::string& filename, ScriptObjectBase* obj);

		const std::string& GetLastError() { return m_error; }

		filesystem_ptr filesystem() { return m_filesystem; }

	protected:
		std::string LoadFile(const std::string& filename);
		std::string GetLuaError();
		bool Execute(const std::string& script, const std::string& filename);
	
	private:
		lua_State* m_state;
		std::string m_error;

		ScriptStack m_thisStack;
		bool m_ext;

		filesystem_ptr m_filesystem;
	};
}