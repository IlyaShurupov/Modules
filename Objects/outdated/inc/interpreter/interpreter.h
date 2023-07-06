#pragma once

#include "operandsstack.h"
#include "scopestack.h"
#include "callstack.h"

namespace obj {
	struct Interpreter {
		OperandStack mOperandsStack;
		ScopeStack mScopeStack;
		CallStack mCallStack;

		obj::Object* mLastParent = NULL;
		bool mIsTypeMethod = false;
		const TypeMethod* mTypeMethod = NULL;

		typedef struct { obj::Object* obj; tp::string id; } GlobalDef;

		void exec(obj::MethodObject* method, obj::ClassObject* self = NULL, obj::DictObject* globals = NULL, tp::init_list<GlobalDef> globals2 = {});

		void stepBytecode();
		void stepBytecodeIn();
		void stepBytecodeOut();

		bool finished();

		void execAll(obj::MethodObject* method, obj::ClassObject* self = NULL, obj::DictObject* globals = NULL, tp::init_list<GlobalDef> globals2 = {});
	};
};