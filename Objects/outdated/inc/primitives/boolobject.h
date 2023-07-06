
#pragma once

#include "core/object.h"

namespace obj {

	struct BoolObject : Object {
		tp::alni val;

		static ObjectType TypeData;
		static void constructor(BoolObject* self);
		static void copy(BoolObject* self, const BoolObject* in);
		
		static BoolObject* create(bool in);

		static void from_int(BoolObject* self, tp::alni in);
		static void from_float(BoolObject* self, tp::alnf in);
		static void from_string(BoolObject* self, tp::string in);
		static tp::string to_string(BoolObject* self);
		static tp::alni to_int(BoolObject* self);
		static tp::alnf to_float(BoolObject* self);
	};
};