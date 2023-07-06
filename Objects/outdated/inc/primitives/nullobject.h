
#pragma once

#include "core/object.h"

namespace obj {

	extern struct NullObject* NdoNull_globalInstance;

	struct NullObject : Object {

		static void destructor(Object* self);
		static ObjectType TypeData;
		static void uninit();
		static Object* null_return() {
			if (!NdoNull_globalInstance) {
				NdoNull_globalInstance = (NullObject*) NDO->create("null");
				obj::NDO->refinc(NdoNull_globalInstance);
			}
			return (Object*) NdoNull_globalInstance;
		}

		static Object* null_return_ref() {
			obj::NDO->refinc(null_return());
			return NdoNull_globalInstance;
		}
	};

	#define NDO_NULL obj::NullObject::null_return()
	#define NDO_NULL_REF obj::NullObject::null_return_ref()

};