
#pragma once

#include "primitives/linkobject.h"

using namespace obj;
using namespace tp;

void LinkObject::constructor(Object* self) {
	NDO_CAST(LinkObject, self)->link = 0;
}

void LinkObject::destructor(LinkObject* self) {
	if (self->link) NDO->destroy(self->link);
}

void LinkObject::copy(Object* self, const Object* in) {
	NDO_CAST(LinkObject, self)->setLink(NDO_CAST(LinkObject, in)->link);
}

LinkObject* LinkObject::create(Object* in) {
	NDO_CASTV(LinkObject, NDO->create("link"), out)->link = in;
	return out;
}

alni LinkObject::save_size(LinkObject* self) {
	return sizeof(alni);
}

void LinkObject::save(LinkObject* self, File& file_self) {
	if (self->link != NULL) {
		alni link_object_save_adress = NDO->save(file_self, self->link);
		file_self.write<alni>(&link_object_save_adress);
	}
	else {
		alni null = -1;
		file_self.write<alni>(&null);
	}
}

void LinkObject::load(File& file_self, LinkObject* self) {

	alni saved_object_adress;
	file_self.read<alni>(&saved_object_adress);

	if (saved_object_adress == -1) {
		self->link = NULL;
	}
	else {
		self->link = NDO->load(file_self, saved_object_adress);
	}
}

tp::Array<Object*> LinkObject::childs_retrival(LinkObject* self) {
	tp::Array<Object*> out;
	if (self->link) out.pushBack(self->link);
	return out;
}

alni LinkObject::allocated_size(LinkObject* self) {
	return sizeof(Object*);
}

alni LinkObject::allocated_size_recursive(LinkObject* self) {
	alni out = sizeof(Object*);
	if (self->link) {
		out += NDO->objsize_ram_recursive_util(self->link, self->link->type);
	}
	return out;
}

Object* LinkObject::getLink() {
	return link;
}

void LinkObject::setLink(Object* obj) {
#ifdef OBJECT_REF_COUNT
	if (link) NDO->destroy(link);
	if (obj) NDO->refinc(obj);
#endif // OBJECT_REF_COUNT
	link = obj;
}

static auto tm_set = TypeMethod{
	.nameid = "set",
	.descr = "sets the link",
	.args = {
		{ "target", NULL }
	},
	.exec = [](const TypeMethod* tm) {
		auto const self = (LinkObject*)tm->self;
		auto const target = tm->args[0].obj;
		self->setLink(target);
	}
};

static auto tm_get = TypeMethod{
	.nameid = "get",
	.descr = "gets the link",
	.exec = [](const TypeMethod* tm) {
		auto const self = (LinkObject*)tm->self;
		auto link = self->getLink();
		if (link) {
			tm->ret.obj = link;
		}
	},
	.ret = { "the link", NULL }
};

struct obj::ObjectType LinkObject::TypeData = {
	.base = NULL,
	.constructor = LinkObject::constructor,
	.destructor = (object_destructor)LinkObject::destructor,
	.copy = LinkObject::copy,
	.size = sizeof(LinkObject),
	.name = "link",
	.convesions = NULL,
	.save_size = (object_save_size)LinkObject::save_size,
	.save = (object_save)LinkObject::save,
	.load = (object_load)LinkObject::load,
	.childs_retrival = (object_debug_all_childs_retrival)LinkObject::childs_retrival,
	.allocated_size = (object_allocated_size)LinkObject::allocated_size,
	.allocated_size_recursive = (object_allocated_size_recursive)LinkObject::allocated_size_recursive,

	.type_methods = {
		.methods = {
			&tm_set,
			&tm_get,
		}
	}
};