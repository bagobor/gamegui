#pragma once

#include "rgde/core/factory.h"

namespace game
{
	//базовый объект для всех объектов, которые создает уровень
	//(может быть нужен, если надо будет предьявлять общие требования
	//к таким объектам)
	class ILevelObject
	{
	public:
		virtual ~ILevelObject(){}
	};

	//фабрика обьектов, которые могут создавать уровни
	typedef factory::TFactory<game::ILevelObject> LevelObjFactory;

	template <class T>
	class _registrator
	{
	public:
		_registrator(){LevelObjFactory::Instance().RegisterType<T>();}
	protected:
		static _registrator<T> instance;
	};

	#define REGISTER_LEVEL_TYPE(type) _registrator<type> _registrator<type>::instance;
}