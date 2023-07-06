
#pragma once

#include "primitives/stringobject.h"

using namespace obj;
using namespace tp;

void StringObject::constructor(Object* self) {
	new (&NDO_CAST(StringObject, self)->val) string();
}

void StringObject::destructor(StringObject* self) {
	self->val.~string();
}

void StringObject::copy(Object* self, const Object* in) {
	NDO_CAST(StringObject, self)->val = NDO_CAST(StringObject, in)->val;
}

StringObject* StringObject::create(string in) {
	NDO_CASTV(StringObject, NDO->create("str"), out)->val = in;
	return out;
}

void StringObject::from_int(StringObject* self, alni in) {
	self->val = in;
}

void StringObject::from_float(StringObject* self, alnf in) {
	self->val = in;
}

void StringObject::from_string(StringObject* self, string in) {
	self->val = in;
}

string StringObject::to_string(StringObject* self) {
	return self->val;
}

alni StringObject::to_int(StringObject* self) {
	return alni(self->val);
}

alnf StringObject::to_float(StringObject* self) {
	return alnf(self->val);
}

static alni save_size(StringObject* self) {
	return self->val.save_size();
}

static void save(StringObject* self, File& file_self) {
	self->val.save(&file_self);
}

static void load(File& file_self, StringObject* self) {
	new (&self->val) tp::string();
	self->val.load(&file_self);
}

alni allocated_size(StringObject* self) {
	return self->val.sizeAllocatedMem();
}

static bool compare_strings(StringObject* left, StringObject* right) {
	return left->val == right->val;
}

struct ObjectTypeConversions StringObjectTypeConversions = {
	.from_int = (object_from_int)StringObject::from_int,
	.from_float = (object_from_float)StringObject::from_float,
	.from_string = (object_from_string)StringObject::from_string,
	.to_string = (object_to_string)StringObject::to_string,
	.to_int = (object_to_int)StringObject::to_int,
	.to_float = (object_to_float)StringObject::to_float,
};

struct obj::ObjectType StringObject::TypeData = {
	.base = NULL,
	.constructor = StringObject::constructor,
	.destructor = (object_destructor)StringObject::destructor,
	.copy = StringObject::copy,
	.size = sizeof(StringObject),
	.name = "str",
	.convesions = &StringObjectTypeConversions,
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
	.load = (object_load)load,
	.comparison = (object_compare) compare_strings,
	.allocated_size = (object_allocated_size) allocated_size,
};