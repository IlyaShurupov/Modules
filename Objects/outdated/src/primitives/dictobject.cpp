#pragma once

#include "primitives/dictobject.h"
#include "primitives/stringobject.h"

using namespace obj;
using namespace tp;

void DictObject::constructor(Object* self) {
	NDO_CASTV(DictObject, self, dict);

	new (&dict->items) HashMap<Object*, string>();
}

void DictObject::copy(Object* in, const Object* target) {
	NDO_CASTV(DictObject, in, self);
	NDO_CASTV(DictObject, target, src);

	destructor(self);
	constructor(self);

	for (auto item : src->items) {
		auto instance = NDO->instatiate(item->val);
		self->items.put(item->key, instance);
	}
}

void DictObject::destructor(Object* self) {
	NDO_CASTV(DictObject, self, dict);
	for (auto item : dict->items) {
		NDO->destroy(item->val);
	}
	dict->items.~HashMap();
}

alni DictObject::save_size(DictObject* self) {
	// calculate size needed
	alni save_size = 0;

	// number on entries
	save_size += sizeof(alni);

	for (auto item : self->items) {
		// string length
		save_size += (item->key.size() + 1) * sizeof(*item->key.cstr());
		// object file adress
		save_size += sizeof(alni);
	}

	return save_size;
}

void DictObject::save(DictObject* self, File& file_self) {

	// write size
	alni len = self->items.size();
	file_self.write<alni>(&len);

	// save hashmap pairs
	for (auto item : self->items) {
		// item val
		alni ndo_object_adress = NDO->save(file_self, item->val);
		file_self.write<alni>(&ndo_object_adress);

		// item key
		item->key.save(&file_self);
	}
}

void DictObject::load(File& file_self, DictObject* self) {

	new (&self->items) tp::HashMap<Object*, tp::string>();

	alni len;
	file_self.read<alni>(&len);

	for (alni i = 0; i < len; i++) {

		// read val
		alni ndo_object_adress;
		file_self.read<alni>(&ndo_object_adress);
		Object* val = NDO->load(file_self, ndo_object_adress);

		// read key value
		string key;
		key.load(&file_self);

		// add to dictinary
		self->items.put(key, val);
	}
}

tp::Array<Object*> DictObject::childs_retrival(DictObject* self) {
	tp::Array<Object*> out;
	out.reserve(self->items.size());
	for (auto item : self->items) {
		out[item.entry_idx] = item->val;
	}
	return out;
}

alni DictObject::allocated_size(DictObject* self) {
	alni out = self->items.sizeAllocatedMem();
	for (auto item : self->items) {
		out += item->key.sizeAllocatedMem();
	}
	return out;
}

alni DictObject::allocated_size_recursive(DictObject* self) {
	alni out = allocated_size(self);
	for (auto item : self->items) {
		out += NDO->objsize_ram_recursive_util(item->val, item->val->type);
	}
	return out;
}

void DictObject::put(tp::string str, Object* obj) {
	assert(obj);
	NDO->refinc(obj);
	items.put(str, obj);
}

void DictObject::remove(tp::string str) {
	auto idx = items.presents(str);
	if (idx) {
		NDO->destroy(items.getSlotVal(idx));
		items.remove(str);
	}
}

Object* DictObject::get(tp::string str) {
	return items.get(str);
}

tp::HashMap<Object*, tp::string>::MapIdx DictObject::presents(tp::string str) {
	return items.presents(str);
}

Object* DictObject::getSlotVal(tp::alni idx) {
	return items.getSlotVal(idx);
}

const tp::HashMap<Object*, tp::string>& DictObject::getItems() const {
	return items;
}

static auto tm_get = TypeMethod{
	.nameid = "get",
	.descr = "gets the object",
	.args = { 
		{ "str key", NULL }
	},
	.exec = [](const TypeMethod* tm) {
		auto const self = (DictObject*)tm->self;
		auto str_key = tm->args[0].obj;

		NDO_CASTV(StringObject, str_key, key);
		RelAssert(key);

		auto idx = self->presents(key->val);
		if (idx) {
			tm->ret.obj = self->getSlotVal(idx);
		}
	},
	.ret = { "object", NULL }
};

static auto tm_put = TypeMethod{
	.nameid = "put",
	.descr = "puts the object into the dictinary",
	.args = {
		{ "key", NULL },
		{ "object", NULL }
	},
	.exec = [](const TypeMethod* tm) {
		auto const self = (DictObject*)tm->self;

		auto str_key = tm->args[0].obj;
		auto obj = tm->args[1].obj;

		NDO_CASTV(StringObject, str_key, key);
		RelAssert(key);

		self->put(key->val, obj);
	},
};

static auto tm_remove = TypeMethod{
	.nameid = "remove",
	.descr = "remove the object from the dictinary",
	.args = {
		{ "key", NULL },
	},
	.exec = [](const TypeMethod* tm) {
		auto const self = (DictObject*)tm->self;

		auto str_key = tm->args[0].obj;
		
		NDO_CASTV(StringObject, str_key, key);
		RelAssert(key);

		self->remove(key->val);
	},
};

struct obj::ObjectType DictObject::TypeData = {
	.base = NULL,
	.constructor = DictObject::constructor,
	.destructor = DictObject::destructor,
	.copy = DictObject::copy,
	.size = sizeof(DictObject),
	.name = "dict",
	.save_size = (object_save_size)DictObject::save_size,
	.save = (object_save)DictObject::save,
	.load = (object_load)DictObject::load,
	.childs_retrival = (object_debug_all_childs_retrival)DictObject::childs_retrival,
	.allocated_size = (object_allocated_size)DictObject::allocated_size,
	.allocated_size_recursive = (object_allocated_size_recursive)DictObject::allocated_size_recursive,

	.type_methods = {
	.methods = {
			&tm_put,
			&tm_remove,
			&tm_get,
		}
	}
};
