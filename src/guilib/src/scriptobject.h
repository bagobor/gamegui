#pragma once

struct lua_State;
namespace gui
{
	class  ScriptObject
	{
	protected:
		~ScriptObject() {}

	protected:
		virtual void thisset() = 0;
		void thisreset(lua_State* state);
		friend class ScriptStack;
	};
}