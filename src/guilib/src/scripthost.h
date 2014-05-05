#pragma once

#include <memory>

struct lua_State;

namespace gui
{
	class  ScriptObject;
	struct filesystem;
	typedef std::shared_ptr<filesystem> filesystem_ptr;

	class ScriptStack
	{
	public:
		ScriptStack();
		void clear();
		void push(ScriptObject* obj);
		void pop(lua_State* state);
	private:
		std::vector<ScriptObject*> m_stack;
	};

	class  ScriptSystem
	{
	public:
		/// @brief - ctor
		/// @param externalState - to provide external luaState, 0 - to use own one.
		ScriptSystem(filesystem_ptr fs, lua_State* externalState = NULL);
		~ScriptSystem();

		lua_State* LuaState();

		bool ExecuteString(const std::string& script, ScriptObject* obj, const std::string& filename = "unknown event");
		bool ExecuteString(const std::string& script, const std::string& filename = "mem buffer");
		bool ExecuteFile(const std::string& filename);

		const std::string& GetLastError() { return m_error; }

		filesystem_ptr get_filesystem() {return m_filesystem;}

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