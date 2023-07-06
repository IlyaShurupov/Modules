
#include "interpreter/scopestack.h"

using namespace obj;

Scope::~Scope() {
	for (auto local : mLocals) {
		obj::NDO->destroy(local->val);
	}
	for (auto tmp : mTemps) {
		obj::NDO->destroy(tmp.data());
	}
}

obj::Object* ScopeStack::getLocalUtil(Scope& scope, tp::string* id) {

	auto idx = scope.mLocals.presents(*id);

	if (idx) {
		return scope.mLocals.getSlotVal(idx);
	}
	else {
		mIterIdx--;
		Scope& parent_scope = mBuff[mIterIdx];
		RelAssert(parent_scope.mChildReachable && "Undefined Local Reference");
		return getLocalUtil(parent_scope, id);
	}
}

ScopeStack::ScopeStack() {
	mBuff = (Scope*) tp::heapalloc->Alloc(sizeof(Scope) * MAX_STACK_SIZE);
	mIdx = 0;
	mIterIdx = 0;
}

void ScopeStack::enterScope(bool aChildReachable) {
	RelAssert(MAX_STACK_SIZE - 1 > mIdx && "stack overflow");
	new (&mBuff[mIdx]) Scope();
	mBuff[mIdx].mChildReachable = aChildReachable;
	mIdx++;
}

void ScopeStack::leaveScope() {
	RelAssert(mIdx != NULL && "stack overflow");
	mBuff[mIdx - 1].~Scope();
	mIdx--;
}

void ScopeStack::addTemp(obj::Object* tmp) {
	obj::NDO->refinc(tmp);
	mBuff[mIdx - 1].mTemps.pushBack(tmp);
}

void ScopeStack::popTemp() {
	obj::NDO->destroy(mBuff[mIdx - 1].mTemps.last()->data);
	mBuff[mIdx - 1].mTemps.popBack();
}

void ScopeStack::addTempReturn(obj::Object* ret) {
	if (mIdx >= 2) {
		obj::NDO->refinc(ret);
		mBuff[mIdx - 2].mTemps.pushBack(ret);
	}
}

void ScopeStack::addLocal(obj::Object* local, tp::string id) {
	assert(mIdx != 0 && "No scope given");
	Scope::ObjDict& locals = mBuff[mIdx - 1].mLocals;
	auto idx = locals.presents(id);
	if (idx) {
		obj::NDO->destroy(locals.getSlotVal(idx));
	}
	obj::NDO->refinc(local);
	locals.put(id, local);
}

obj::Object* ScopeStack::getLocal(tp::string& str) {
	mIterIdx = mIdx - 1;
	return getLocalUtil(mBuff[mIdx - 1], &str);
}

Scope* ScopeStack::getCurrentScope() {
	return &mBuff[mIdx - 1];
}

ScopeStack::~ScopeStack() {
	tp::pickalloc->Free(mBuff);
}
