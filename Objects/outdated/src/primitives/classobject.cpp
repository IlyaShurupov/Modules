#pragma once

#include "primitives/classobject.h"

#include "primitives/dictobject.h"

#include "primitives/nullobject.h"

using namespace obj;
using namespace tp;

void ClassObject::constructor(ClassObject* self) {
	self->members = NDO_CAST(DictObject, NDO->create("dict"));

	self->addMember(NDO_NULL, "__init__");
	self->addMember(NDO_NULL, "__del__");
}

void ClassObject::copy(ClassObject* self, const ClassObject* blueprint) {
	NDO->copy(self->members, blueprint->members);
}

void ClassObject::destructor(ClassObject* self) {
	NDO->destroy(self->members);
}

void ClassObject::addMember(Object* obj, tp::string id) {
	members->put(id, obj);
}

void ClassObject::createMember(tp::string type, tp::string id) {
	auto newo = NDO->create(type);
	members->put(id, newo);
}

alni ClassObject::save_size(ClassObject* self) {
	return sizeof(alni); // dict object adress
}

void ClassObject::save(ClassObject* self, File& file_self) {
	// save dictobject
	alni ndo_object_adress = NDO->save(file_self, self->members);
	file_self.write<alni>(&ndo_object_adress);
}

void ClassObject::load(File& file_self, ClassObject* self) {
	alni ndo_object_adress;
	file_self.read<alni>(&ndo_object_adress);
	self->members = NDO_CAST(DictObject, NDO->load(file_self, ndo_object_adress));
}

tp::Array<Object*> childs_retrival(ClassObject* self) {
	tp::Array<Object*> out;
	out.pushBack(self->members);
	return out;
}

alni allocated_size(ClassObject* self) {
	return sizeof(DictObject*);
}

alni allocated_size_recursive(ClassObject* self) {
	alni out = sizeof(DictObject*);
	out += NDO->objsize_ram_recursive_util(self->members, self->members->type);
	return out;
}

struct ObjectType ClassObject::TypeData = {
	.base = NULL,
	.constructor = (object_constructor) ClassObject::constructor,
	.destructor = (object_destructor) ClassObject::destructor,
	.copy = (object_copy) ClassObject::copy,
	.size = sizeof(ClassObject),
	.name = "class",
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
  .load = (object_load)load,
	.childs_retrival = (object_debug_all_childs_retrival) childs_retrival,
	.allocated_size = (object_allocated_size) allocated_size,
	.allocated_size_recursive = (object_allocated_size_recursive) allocated_size_recursive
};
