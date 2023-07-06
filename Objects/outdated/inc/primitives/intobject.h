
#pragma once

#include "core/object.h"

namespace obj {

	struct IntObject : Object {
		tp::alni val;

		static ObjectType TypeData;
		static ObjectTypeAriphmetics TypeAriphm;

		static void constructor(Object* self);
		static void copy(IntObject* self, const IntObject* in);
		static IntObject* create(tp::alni in);

		static void from_int(Object* self, tp::alni in);
		static void from_float(Object* self, tp::alnf in);
		static void from_string(Object* self, tp::string in);
		static tp::string to_string(Object* self);
		static tp::alni to_int(Object* self);
		static tp::alnf to_float(Object* self);
	};
};