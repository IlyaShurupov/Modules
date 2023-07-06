
#pragma once

#include "core/object.h"
#include "color.h"

namespace obj {

	struct ColorObject : Object {
		tp::rgba mCol;

		static ObjectType TypeData;
		static ObjectTypeAriphmetics TypeAriphm;

		static void constructor(Object* self);
		static void copy(ColorObject* self, const ColorObject* in);

		static void from_int(ColorObject* self, tp::alni in);
		static void from_float(ColorObject* self, tp::alnf in);
		static tp::string to_string(ColorObject* self);

		static ColorObject* create(tp::rgba in);
	};
};