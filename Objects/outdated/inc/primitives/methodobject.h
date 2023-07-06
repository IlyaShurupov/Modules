#pragma once

#include "primitives/stringobject.h"
#include "interpreter/script.h"

namespace obj {

	struct MethodObject : Object {

		static ObjectType TypeData;

		Script* mScript;

		static void constructor(MethodObject* self);
		static void copy(MethodObject* self, MethodObject* in);
		static void destructor(MethodObject* self);
		static tp::alni save_size(MethodObject* self);
		static void save(MethodObject* self, tp::File& file_self);
		static void load(tp::File& file_self, obj::MethodObject* self);

		static void Initialize();
		static void UnInitialize();

		void compile();

		static MethodObject* create(tp::string script);
	};
};