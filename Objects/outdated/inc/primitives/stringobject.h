
#pragma once

#include "core/object.h"

namespace obj {

	struct StringObject : Object {
		tp::string val;

		static ObjectType TypeData;
		static void constructor(Object* self);
		static void destructor(StringObject* self);
		static void copy(Object* self, const Object* in);
		static StringObject* create(tp::string in);

		static void from_int(StringObject* self, tp::alni in);
		static void from_float(StringObject* self, tp::alnf in);
		static void from_string(StringObject* self, tp::string in);
		static tp::string to_string(StringObject* self);
		static tp::alni to_int(StringObject* self);
		static tp::alnf to_float(StringObject* self);
	};
};