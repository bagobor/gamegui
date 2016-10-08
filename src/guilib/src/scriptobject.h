#pragma once
#include <luabind/luabind.hpp>
#include <luabind/weak_ref.hpp>
#include <luabind/get_main_thread.hpp>

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

		luabind::object getScriptRepl() { return m_localtable; }

	protected:
		ScriptSystem& m_script_system;
		lua_State* m_state;
		luabind::object m_localtable;
	};

	template<class T>
	class  ScriptObject : public ScriptObjectBase, public luabind::wrap_base
	{
	protected:
		ScriptObject(ScriptSystem& script_system)
			: ScriptObjectBase(script_system) {
			// hack
			static char buff[512];
			sprintf(buff, "{ScriptObject_%x}", (intptr_t)(void*)this);
			luabind::globals(m_state)[buff] = (T*)this;
			m_localtable = luabind::globals(m_state)[buff];
		}

		~ScriptObject() {
			static char buff[512];
			sprintf(buff, "{ScriptObject_%x}", (intptr_t)(void*)this);
			luabind::globals(m_state)[buff] = (T*)NULL;
		}

		void thisset() {
			if (!m_state) return;
			luabind::globals(m_state)["this"] = m_localtable;
		}
		
		friend class ScriptStack;
	};
}