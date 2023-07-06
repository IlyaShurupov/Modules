
#pragma once

#include "primitives/listobject.h"

#include "primitives/intobject.h"

using namespace obj;
using namespace tp;

void ListObject::constructor(Object* in) {
	NDO_CASTV(ListObject, in, self);
	new (&self->items) List<Object*>();
}

void ListObject::copy(Object* in, const Object* target) {
	NDO_CASTV(ListObject, in, self);
	NDO_CASTV(ListObject, target, src);

	destructor(in);

	for (auto item : src->items) {
		self->pushBack(NDO->instatiate(item.data()));
	}
}

void ListObject::destructor(Object* in) {
	NDO_CASTV(ListObject, in, self);
	for (auto item : self->items) {
		NDO->destroy(item.data());
	}
	self->items.free();
}

alni ListObject::save_size(ListObject* self) {
	alni len = self->items.length();
	return (len + 1) * sizeof(alni);
}

void ListObject::save(ListObject* self, File& file_self) {

	alni len = self->items.length();
	file_self.write<alni>(&len);

	for (auto item : self->items) {
		alni ndo_object_adress = NDO->save(file_self, item.data());
		file_self.write<alni>(&ndo_object_adress);
	}
}

void ListObject::load(File& file_self, ListObject* self) {

	new (&self->items) tp::List<Object*>();

	alni len; 
	file_self.read<alni>(&len);
	
	for (alni i = 0; i < len; i++) {
		alni ndo_object_adress;
		file_self.read<alni>(&ndo_object_adress);
		self->items.pushBack(NDO->load(file_self, ndo_object_adress));
	}
}

tp::Array<Object*> ListObject::childs_retrival(ListObject* self) {
	tp::Array<Object*> out;
	out.reserve(self->items.length());
	for (auto item : self->items) {
		out[item.idx()] = item.data();
	}
	return out;
}

alni ListObject::allocated_size(ListObject* self) {
	return self->items.sizeAllocatedMem();
}

alni ListObject::allocated_size_recursive(ListObject* self) {
	alni out = self->items.sizeAllocatedMem();
	for (auto item : self->items) {
		out += NDO->objsize_ram_recursive_util(item.data(), item->type);
	}
	return out;
}

void ListObject::pushBack(Object* obj) {
#ifdef OBJECT_REF_COUNT
	obj::NDO->refinc(obj);
#endif // OBJECT_REF_COUNT

	items.pushBack(obj);
}

void ListObject::pushFront(Object* obj) {
#ifdef OBJECT_REF_COUNT
	obj::NDO->refinc(obj);
#endif // OBJECT_REF_COUNT

	items.pushFront(obj);
}

void ListObject::delNode(tp::ListNode<Object*>* node) {
#ifdef OBJECT_REF_COUNT
	obj::NDO->destroy(node->data);
#endif // OBJECT_REF_COUNT
	items.delNode(node);
}

void ListObject::popBack() {
#ifdef OBJECT_REF_COUNT
	auto obj = items.last();
	if (obj) obj::NDO->destroy(obj->data);
#endif // OBJECT_REF_COUNT

	items.popBack();
}

const tp::List<Object*>& ListObject::getItems() const {
	return items;
}

struct obj::ObjectType obj::ListObject::TypeData = {
	.base = NULL,
	.constructor = ListObject::constructor,
	.destructor = ListObject::destructor,
	.copy = ListObject::copy,
	.size = sizeof(ListObject),
	.name = "list",
	.convesions = NULL,
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
	.load = (object_load)load,
	.childs_retrival = (object_debug_all_childs_retrival) childs_retrival,
	.allocated_size = (object_allocated_size) allocated_size,
	.allocated_size_recursive = (object_allocated_size_recursive) allocated_size_recursive
};