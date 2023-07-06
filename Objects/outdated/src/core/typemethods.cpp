
#include "core/typemethods.h"

#include "primitives/nullobject.h"
#include "primitives/typeobject.h"
#include "primitives/floatobject.h"

#include "interpreter/interpreter.h"


using namespace obj;

TypeMethod obj::gDefaultTypeMethods[] = {
	{
		.nameid = "type",
		.descr = "retrieves typeobject",
		.exec = [](const TypeMethod* tm) {
			tm->ret.obj = TypeObject::create(tm->self->type);
		},
		.ret = { "typeobject", NULL },
	},
	{
		.nameid = "to_str",
		.descr = "converts to string",
		.exec = [](const TypeMethod* tm) {
			if (tm->self->type->convesions && tm->self->type->convesions->to_string) {
				tm->ret.obj = StringObject::create(NDO->toString(tm->self));
			}
		},
		.ret = { "string object", NULL },
	},
	{
		.nameid = "to_float",
		.descr = "converts to float",
		.exec = [](const TypeMethod* tm) {
			if (tm->self->type->convesions && tm->self->type->convesions->to_float) {
				tm->ret.obj = FloatObject::create(NDO->toFloat(tm->self));
			}
		},
		.ret = { "string object", NULL },
	},
};

tp::int2 TypeMethods::presents(tp::string id) const {
	for (tp::int2 idx = 0; idx < mNMethods; idx++) {
		if (id == methods[idx]->nameid) {
			return idx;
		}
	}

	tp::int2 idx = 0;
	for (auto& tm : gDefaultTypeMethods) {
		if (id == tm.nameid) {
			return { tp::int2(idx + MAX_TYPE_METHODS) };
		}
		idx++;
	}

	return -1;
}

TypeMethods::LookupKey TypeMethods::presents(const ObjectType* type, tp::string id) {
	
	tp::int2 depth = 0;
	tp::int2 idx = 0;

	for (auto iter_type = type; iter_type; iter_type = iter_type->base) {
		idx = iter_type->type_methods.presents(id);
		if (idx != -1) {
			break;
		}
		depth++;
	}

	return { idx, depth};
}

const TypeMethod* TypeMethods::getMethod(tp::int2 key) const {
	if (key < MAX_TYPE_METHODS) {
		return methods[key];
	}
	return &gDefaultTypeMethods[key - MAX_TYPE_METHODS];
}

const TypeMethod* TypeMethods::getMethod(const ObjectType* type, LookupKey key) {
	auto type_iter = type;
	for (auto idx = 0; idx < key.type_depth; idx++) {
		type_iter = type_iter->base;
	}
	return type_iter->type_methods.getMethod(key.key);
}

void TypeMethods::init() {
	while (methods[mNMethods]) {
		mNMethods++;
	}

	for (tp::int1 i = 0; i < mNMethods; i++) {
		methods[i]->init();
	}

	// initialize and use finate state automata to lookup methods
}

tp::halni TypeMethods::nMethods() const {
	return mNMethods;
}

void TypeMethod::operator()(Interpreter* interp) const{
	for (auto i = 1; i <= mNargs; i++) {
		args[mNargs - i].obj = interp->mOperandsStack.getOperand();
		//NDO->refinc(args[i].obj);
		RelAssert(args[mNargs - i].obj && "expected an argument");
	}

	RelAssert(!interp->mOperandsStack.getOperand() && "args remained");
	interp->mOperandsStack.getOperand();

	self = interp->mLastParent;

	exec(this);

	if (ret.obj) {
		interp->mOperandsStack.push(ret.obj);
		interp->mScopeStack.addTempReturn(ret.obj);
	}
	else {
		interp->mOperandsStack.push(NDO_NULL_REF);
	}
}

void TypeMethod::init() {
	while (args[mNargs].descr) {
		mNargs++;
	}
}