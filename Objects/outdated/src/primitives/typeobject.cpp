
#pragma once

#include "primitives/typeobject.h"
#include "primitives/nullobject.h"

using namespace obj;
using namespace tp;

TypeObject* TypeObject::create(const ObjectType* type) {
	NDO_CASTV(TypeObject, NDO->create("typeobject"), out);
	out->mTypeRef = type;
	return out;
}

static alni save_size(TypeObject* self) {
	tp::string const nameid(self->mTypeRef->name);
	return nameid.save_size();
}

static void save(TypeObject* self, File& file_self) {
	tp::string const nameid(self->mTypeRef->name);
	nameid.save(&file_self);
}

static void load(File& file_self, TypeObject* self) {
	tp::string nameid;
	nameid.load(&file_self);

	auto idx = NDO->types.presents(nameid);

	if (idx) {
		self->mTypeRef = NDO->types.getSlotVal(idx);
	}
	else {
		self->mTypeRef = &NullObject::TypeData;
	}
}

static alni allocated_size(TypeObject* self) {
	return sizeof(alni);
}

static void from_string(TypeObject* self, tp::string in) {
	auto idx = NDO->types.presents(in);

	if (idx) {
		self->mTypeRef = NDO->types.getSlotVal(idx);
	}
	else {
		self->mTypeRef = &NullObject::TypeData;
	}
}

static string to_string(TypeObject* self) {
	return self->mTypeRef->name;
}

bool comparator(TypeObject* left, TypeObject* right) {
	return left->mTypeRef == right->mTypeRef;
}

static struct ObjectTypeConversions conversions = {
	.from_string = (object_from_string) from_string,
	.to_string = (object_to_string) to_string,
};

struct obj::ObjectType TypeObject::TypeData = {
	.base = NULL,
	//.constructor = (object_constructor) TypeObject::constructor,
	.size = sizeof(TypeObject),
	.name = "typeobject",
	.convesions = &conversions,
	.save_size = (object_save_size) save_size,
	.save = (object_save) save,
	.load = (object_load) load,
	.comparison = (object_compare) comparator,
	.allocated_size = (object_allocated_size) allocated_size,
};