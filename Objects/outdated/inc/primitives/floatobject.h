
#pragma once

#include "core/object.h"

namespace obj {

	struct FloatObject : Object {
		tp::alnf val;

		static ObjectType TypeData;
		static ObjectTypeAriphmetics TypeAriphm;


		static void constructor(FloatObject* self);
		static void copy(FloatObject* self, const FloatObject* in);
		static FloatObject* create(tp::alnf in);

		static void from_int(FloatObject* self, tp::alni in);
		static void from_float(FloatObject* self, tp::alnf in);
		static void from_string(FloatObject* self, tp::string in);
		static tp::string to_string(FloatObject* self);
		static tp::alni to_int(FloatObject* self);
		static tp::alnf to_float(FloatObject* self);
	};
};