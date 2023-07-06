
#pragma once

#include "core/object.h"

namespace obj {

	struct LinkObject : Object {
		static ObjectType TypeData;
		static void constructor(Object* self);
		static void destructor(LinkObject* self);
		static void copy(Object* self, const Object* in);
		static LinkObject* create(Object* in);

		static tp::alni save_size(LinkObject* self);
		static void save(LinkObject* self, tp::File& file_self);
		static void load(tp::File& file_self, LinkObject* self);
		static tp::alni allocated_size(LinkObject* self);
		static tp::alni allocated_size_recursive(LinkObject* self);
		static tp::Array<Object*> childs_retrival(LinkObject* self);

		Object* getLink();
		void setLink(Object* obj);

	private:
		Object* link;
	};

};