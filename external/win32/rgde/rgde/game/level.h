#pragma once

#include "rgde/core/property.h"
#include "rgde/game/level_object.h"

namespace game
{
	//базовый класс уровня
	class Level : public core::FunctionsOwner
	{
	public:
		Level(const std::string& name, const std::string& next_level);
		~Level();

		void addTypeToCreate(const std::string& type_name);	//добавление типа для создания
		void enter();											//инициализация уровня
		void leave();											//деинициализация уровня

		//узнать имя уровня
		const std::string& getName() const		{return m_name;}
		//узнать имя следущего уровня
		const std::string& get_next_level() const {return m_next_level;}

	private:
		std::list<ILevelObject*> m_listLevelObjs; //список объектов, которые созданы уровнем
		std::list<std::string>   m_listTypes;     //типы объектов которые надо создать
		std::string              m_name;       //имя уровня
		std::string              m_next_level;  //имя уровня, который должен быть следующим
	};
}