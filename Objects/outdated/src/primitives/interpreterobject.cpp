
#include "primitives/interpreterobject.h"
#include "primitives/linkobject.h"
#include "primitives/methodobject.h"

using namespace obj;
using namespace tp;

void InterpreterObject::constructor(InterpreterObject* self) {
	new (&self->mInterpreter) Interpreter();

	self->createMember("dict", "globals");
	self->createMember("link", "target method");
}

void InterpreterObject::destructor(InterpreterObject* self) {
	self->mInterpreter.~Interpreter();
}

void InterpreterObject::load(File& file_self, InterpreterObject* self) {
	new (&self->mInterpreter) Interpreter();
}

bool InterpreterObject::running() { return !mInterpreter.finished(); }

void InterpreterObject::exec(obj::ClassObject* self, tp::init_list<Interpreter::GlobalDef> globals) {

	if (running()) {
		return;
	}

	auto link_target = getMember<obj::LinkObject>("target method");
	if (!link_target) {
		return;
	}

	auto target = link_target->getLink();
	if (!target) {
		return;
	}

	NDO_CASTV(obj::MethodObject, target, method);
	if (!method || !method->mScript->mBytecode.mInstructions.length()) {
		return;
	}

	mInterpreter.execAll(method, self, getMember<obj::DictObject>("globals"), globals);
}

void InterpreterObject::debug() {
	if (running()) {
		return;
	}

	auto link_target = getMember<obj::LinkObject>("target method");
	if (!link_target) {
		return;
	}

	auto target = link_target->getLink();
	if (!target) {
		return;
	}

	NDO_CASTV(obj::MethodObject, target, method);
	if (!method || !method->mScript->mBytecode.mInstructions.length()) {
		return;
	}
	
	mInterpreter.exec(method, this, getMember<obj::DictObject>("globals"));
}

struct obj::ObjectType InterpreterObject::TypeData = {
	.base = &ClassObject::TypeData,
	.constructor = (object_constructor)InterpreterObject::constructor,
	.destructor = (object_destructor)InterpreterObject::destructor,
	.size = sizeof(InterpreterObject),
	.name = "interpreter",
	.load = (object_load)InterpreterObject::load,
};