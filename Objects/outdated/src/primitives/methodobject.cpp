#pragma once

#include "primitives/methodobject.h"

#include "core/scriptsection.h"
#include "compiler/function.h"

using namespace obj;

struct ObjectType MethodObject::TypeData = {
	.base = NULL,
	.constructor = (object_constructor)MethodObject::constructor,
	.destructor = (object_destructor)MethodObject::destructor,
	.copy = (object_copy)MethodObject::copy,
	.size = sizeof(MethodObject),
	.name = "method",
	.convesions = NULL,
	.save_size = (object_save_size)MethodObject::save_size,
	.save = (object_save)MethodObject::save,
	.load = (object_load)MethodObject::load,
};

void MethodObject::constructor(MethodObject* self) {
	self->mScript = obj::ScriptSection::globalHandle()->createScript();
}

void MethodObject::copy(MethodObject* self, MethodObject* in) {
	obj::ScriptSection::globalHandle()->changeScript(&self->mScript, &in->mScript);
}

void MethodObject::destructor(MethodObject* self) {
	obj::ScriptSection::globalHandle()->abandonScript(self->mScript);
}

tp::alni MethodObject::save_size(MethodObject* self) {
	// script_table_file_address & script string object address
	return sizeof(tp::alni);
}

void MethodObject::save(MethodObject* self, tp::File& file_self) {
	auto script_section = obj::ScriptSection::globalHandle();

	// script_table_file_address 
	tp::alni script_table_file_address = script_section->get_script_file_adress(self->mScript);
	file_self.write<tp::alni>(&script_table_file_address);
}

void MethodObject::load(tp::File& file_self, obj::MethodObject* self) {
	auto script_section = obj::ScriptSection::globalHandle();

	// script_table_file_address 
	tp::alni script_table_file_address;
	file_self.read<tp::alni>(&script_table_file_address);
	self->mScript = script_section->get_scritp_from_file_adress(script_table_file_address);
}


void MethodObject::Initialize() {
	obj::ScriptSection::initialize();
	NDO->define(&MethodObject::TypeData);
	obj::NDO->type_groups.addType(&MethodObject::TypeData, { "Primitives" });
}

void MethodObject::UnInitialize() {
	obj::ScriptSection::uninitialize();
}

void MethodObject::compile() {
	BCgen::Compile(this);
}

MethodObject* MethodObject::create(tp::string script) {
	auto out = (MethodObject*)NDO->create(MethodObject::TypeData.name);
	out->mScript->mReadable->val = script;
	out->compile();
	return out;
}