
#pragma once

#include "primitives/primitives.h"

#include "compressor.h"
#include "tokenizer.h"

void obj::primitives_uninitialize() {
	obj::NullObject::uninit();
	obj::MethodObject::UnInitialize();
}

void obj::primitives_define_types() {
	
	using namespace obj;
	using namespace tp;

	static bool initialized = false;
	if (initialized) {
		return;
	}

	assert(NDO && "Objects library is not initialized");

	NDO->define(&DictObject::TypeData);
	NDO->define(&IntObject::TypeData);
	NDO->define(&LinkObject::TypeData);
	NDO->define(&ListObject::TypeData);
	NDO->define(&NullObject::TypeData);
	NDO->define(&StringObject::TypeData);
	NDO->define(&BoolObject::TypeData);
	NDO->define(&FloatObject::TypeData);
	NDO->define(&EnumObject::TypeData);
	NDO->define(&ClassObject::TypeData);
	NDO->define(&ColorObject::TypeData);
	NDO->define(&InterpreterObject::TypeData);
	NDO->define(&TypeObject::TypeData);

	NDO->type_groups.addType(&DictObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&IntObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&LinkObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&ListObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&NullObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&StringObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&BoolObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&FloatObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&EnumObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&ClassObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&ColorObject::TypeData, {"Primitives"});
	NDO->type_groups.addType(&InterpreterObject::TypeData, { "scripting" });

	obj::MethodObject::Initialize();

	initialized = true;
}

namespace obj {
	objects_api* objects_init();
	void objects_finalize();
};

static bool objInit() {
	obj::objects_init();
	obj::primitives_define_types();
	return true;
}

static void objDeinit() {
	obj::primitives_uninitialize();
	obj::objects_finalize();
}

static tp::ModuleManifest* sModuleDependencies[] = {
	&tp::gModuleCompressor, 
	&tp::gModuleMath, 
	&tp::gModuleStringt, 
	&tp::gModuleTokenizer,
	NULL
};

tp::ModuleManifest obj::gModuleObjects = tp::ModuleManifest("Objects", objInit, objDeinit, sModuleDependencies);