#pragma once
#include <luabind/luabind.hpp>
struct lua_State;

namespace gui
{
	class ScriptSystem;

	struct ScriptObjectBase {
		ScriptObjectBase(ScriptSystem& script_system);

		virtual ~ScriptObjectBase() {}

		virtual void thisset() = 0;

		void thisreset(lua_State* state);

		inline ScriptSystem& getScriptSystem() { return m_script_system; }

	protected:
		ScriptSystem& m_script_system;
		lua_State* m_state;
	};

	template<class T>
	class  ScriptObject : public ScriptObjectBase//, public luabind::wrap_base
	{
	protected:
		ScriptObject(ScriptSystem& script_system)
			: ScriptObjectBase(script_system) {
		}

		~ScriptObject() {}

		void thisset() {
			if (!m_state) return;
			luabind::globals(m_state)["this"] = (T*)this;
		}
		
		friend class ScriptStack;
	};
}