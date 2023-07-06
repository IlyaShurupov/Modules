
#include "compiler/constants.h"
#include "primitives/primitives.h"
#include "primitives/methodobject.h"

using namespace obj;
using namespace tp;
using namespace BCgen;

ConstObject::ConstObject() {}
ConstObject::ConstObject(obj::Object* mObj) : mObj(mObj) {}

ConstObjectsPool::~ConstObjectsPool() {
	if (mDelete) {
		for (auto obj : mStrings) {
			obj::NDO->destroy(obj->val->mObj);
		}
		for (auto obj : mIntegers) {
			obj::NDO->destroy(obj->val->mObj);
		}
		for (auto obj : mFloats) {
			obj::NDO->destroy(obj->val->mObj);
		}
		for (auto obj : mMethods) {
			obj::NDO->destroy(obj->val->mObj);
		}
		if (mBoolFalse.mObj) {
			obj::NDO->destroy(mBoolFalse.mObj);
		}
		if (mBoolTrue.mObj) {
			obj::NDO->destroy(mBoolTrue.mObj);
		}
	}
	for (auto obj : mStrings) {
		delete obj->val;
	}
	for (auto obj : mIntegers) {
		delete obj->val;
	}
	for (auto obj : mFloats) {
		delete obj->val;
	}
}

ConstObject* ConstObjectsPool::registerObject(obj::Object* obj) {
	ConstObject* out = new ConstObject(obj);
	mTotalObjects++;
	return out;
}

ConstObject* ConstObjectsPool::get(tp::alni val) {
	auto idx = mIntegers.presents(val);
	ConstObject* const_obj = NULL;
	if (idx) {
		const_obj = mIntegers.getSlotVal(idx);
	}
	else {
		const_obj = registerObject(IntObject::create(val));
		mIntegers.put(val, const_obj);
	}
	return const_obj;
}

ConstObject* ConstObjectsPool::get(tp::string val) {
	auto idx = mStrings.presents(val);
	ConstObject* const_obj = NULL;
	if (idx) {
		const_obj = mStrings.getSlotVal(idx);
	}
	else {
		const_obj = registerObject(StringObject::create(val));
		mStrings.put(val, const_obj);
	}
	return const_obj;
}

ConstObject* ConstObjectsPool::get(tp::alnf val) {
	auto idx = mFloats.presents(val);
	ConstObject* const_obj = NULL;
	if (idx) {
		const_obj = mFloats.getSlotVal(idx);
	}
	else {
		const_obj = registerObject(FloatObject::create(val));
		mFloats.put(val, const_obj);
	}
	return const_obj;
}

ConstObject* ConstObjectsPool::get(bool val) {
	if (val) {
		if (!mBoolTrue.mObj) {
			mBoolTrue.mObj = BoolObject::create(val);
			mTotalObjects++;
		}
		return &mBoolTrue;
	}
	else {
		if (!mBoolFalse.mObj) {
			mBoolFalse.mObj = BoolObject::create(val);
			mTotalObjects++;
		}
		return &mBoolFalse;
	}
}

ConstObject* ConstObjectsPool::addMethod(tp::string method_id, obj::Object* method) {
	RelAssert(NDO_CAST(MethodObject, method) && "Object is not a method object");
	RelAssert(!mMethods.presents(method_id) && "Method Redefinition");
	auto out = registerObject(method);
	mMethods.put(method_id, out);
	return out;
}

void ConstObjectsPool::save(tp::Array<ConstData>& out) {
	out.reserve(mTotalObjects);
	tp::alni data_idx = 0;
	for (auto obj : mMethods) {
		out[data_idx] = obj->val->mObj;
		obj->val->mConstIdx = data_idx;
		data_idx++;
	}
	for (auto obj : mStrings) {
		out[data_idx] = obj->val->mObj;
		obj->val->mConstIdx = data_idx;
		data_idx++;
	}
	for (auto obj : mIntegers) {
		out[data_idx] = obj->val->mObj;
		obj->val->mConstIdx = data_idx;
		data_idx++;
	}
	for (auto obj : mFloats) {
		out[data_idx] = obj->val->mObj;
		obj->val->mConstIdx = data_idx;
		data_idx++;
	}
	if (mBoolFalse.mObj) {
		out[data_idx] = mBoolFalse.mObj;
		mBoolFalse.mConstIdx = data_idx;
		data_idx++;
	}
	if (mBoolTrue.mObj) {
		out[data_idx] = mBoolTrue.mObj;
		mBoolTrue.mConstIdx = data_idx;
	}
	mDelete = false;
}