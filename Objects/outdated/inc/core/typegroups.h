
#pragma once

#include "map.h"
#include "stringt.h"

namespace obj {

	struct ObjectType;
	struct objects_api;

	class TypeGroups {

	public:

		typedef tp::HashMap<obj::TypeGroups*, tp::string> Dict;

		TypeGroups();

		friend struct obj::objects_api;

		TypeGroups(bool is_group);

		void addType(ObjectType* type, tp::init_list<const char*> path, tp::alni cur_arg = 0);
		void setType(ObjectType* type);
		bool isGroup();
		Dict* getChilds();
		const obj::ObjectType* getType();
		~TypeGroups();

	private:

		bool is_group;
		union {
			Dict childs;
			const obj::ObjectType* type;
		};
	};

};