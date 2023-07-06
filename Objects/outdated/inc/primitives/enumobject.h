
#pragma once

#include "core/object.h"

namespace obj {

	struct EnumObject : Object {

		// one entry is 2 * sizeof(alni) in size
		tp::uhalni active;
		tp::uhalni nentries;
		tp::alni* entries;

		static ObjectType TypeData;
		static void constructor(EnumObject* self);
		static void destructor(EnumObject* self);
		static void copy(EnumObject* self, const EnumObject* in);

		void init(tp::init_list<const char*> list);
		const char* getActiveName();
		const char* getItemName(tp::uhalni idx);

		static void from_int(EnumObject* self, tp::alni in);
		static void from_float(EnumObject* self, tp::alnf in);
		static void from_string(EnumObject* self, tp::string in);
		static tp::string to_string(EnumObject* self);
		static tp::alni to_int(EnumObject* self);
		static tp::alnf to_float(EnumObject* self);

		static bool compare(EnumObject* first, EnumObject* second);
		static EnumObject* create(tp::init_list<const char*> list);
	};
};