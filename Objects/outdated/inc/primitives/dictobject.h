#pragma once

#include "core/object.h"

namespace obj {

	struct DictObject : Object {
		static ObjectType TypeData;
		static void copy(Object* self, const Object* in);
		static void destructor(Object* self);
		static void constructor(Object* self);

		static tp::alni save_size(DictObject* self);
		static void save(DictObject* self, tp::File& file_self);
		static void load(tp::File& file_self, DictObject* self);
		static tp::Array<Object*> childs_retrival(DictObject* self);
		static tp::alni allocated_size(DictObject* self);
		static tp::alni allocated_size_recursive(DictObject* self);

		void put(tp::string, Object*);
		void remove(tp::string);
		Object* get(tp::string);
		tp::HashMap<Object*, tp::string>::MapIdx presents(tp::string);
		Object* getSlotVal(tp::alni);

		const tp::HashMap<Object*, tp::string>& getItems() const;

	private:
		tp::HashMap<Object*, tp::string> items;
	};
};