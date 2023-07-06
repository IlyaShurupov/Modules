
#pragma once

#include "core/object.h"
//#include "array.h"

namespace obj {

	enum ListMethods {
		LISTOBJECT_PUSH_BACK,
		LISTOBJECT_GET_LENGTH,
	};

	struct ListObject : Object {
		static ObjectType TypeData;
		static void constructor(Object* self);
		static void copy(Object* self, const Object* in);
		static void destructor(Object* self);

		static tp::alni allocated_size_recursive(ListObject* self);
		static tp::alni allocated_size(ListObject* self);
		static tp::Array<Object*> childs_retrival(ListObject* self);
		static void load(tp::File& file_self, ListObject* self);
		static void save(ListObject* self, tp::File& file_self);
		static tp::alni save_size(ListObject* self);

		const tp::List<Object*>& getItems() const;

		void pushBack(Object* obj);
		void pushFront(Object* obj);
		void popBack();
		void delNode(tp::ListNode<Object*>*);

	private:
		tp::List<Object*> items;
	};
};