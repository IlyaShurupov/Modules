
#pragma once

#include "primitives/boolobject.h"

using namespace obj;
using namespace tp;

void BoolObject::constructor(BoolObject* self) {
	self->val = false;
}

void BoolObject::copy(BoolObject* self, const BoolObject* in) {
	self->val = in->val;
}

BoolObject* BoolObject::create(bool in) {
	NDO_CASTV(BoolObject, NDO->create("bool"), out)->val = alni(in);
	return out;
}

void BoolObject::from_int(BoolObject* self, alni in) {
	self->val = in;
}

void BoolObject::from_float(BoolObject* self, alnf in) {
	self->val = alni(bool(in));
}

void BoolObject::from_string(BoolObject* self, string in) {
	self->val = alni(bool(in));
}

string BoolObject::to_string(BoolObject* self) {
	return string(bool(self->val));
}

alni BoolObject::to_int(BoolObject* self) {
	return self->val;
}

alnf BoolObject::to_float(BoolObject* self) {
	return alnf(bool(self->val));
}

static alni save_size(BoolObject* self) {
	return sizeof(alni);
}

static void save(BoolObject* self, File& file_self) {
	file_self.write<alni>(&self->val);
}

static void load(File& file_self, BoolObject* self) {
	file_self.read<alni>(&self->val);
}

struct ObjectTypeConversions BoolObjectTypeConversions = {
	.from_int = (object_from_int) BoolObject::from_int,
	.from_float = (object_from_float) BoolObject::from_float,
	.from_string = (object_from_string) BoolObject::from_string,
	.to_string = (object_to_string) BoolObject::to_string,
	.to_int = (object_to_int) BoolObject::to_int,
	.to_float = (object_to_float) BoolObject::to_float,
};

struct obj::ObjectType obj::BoolObject::TypeData = {
	.base = NULL,
	.constructor = (object_constructor) BoolObject::constructor,
	.destructor = NULL,
	.copy = (object_copy) BoolObject::copy,
	.size = sizeof(BoolObject),
	.name = "bool",
	.convesions = &BoolObjectTypeConversions,
	.save_size = (object_save_size)save_size,
	.save = (object_save)save,
	.load = (object_load)load,
};