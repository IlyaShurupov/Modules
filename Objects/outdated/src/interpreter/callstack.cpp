
#include "interpreter/callstack.h"

#include "interpreter/bytecode.h"
#include "primitives/methodobject.h"

using namespace obj;

void CallStack::enter(const CallStack::CallFrame& frame) {
	if (mStack.loadLen()) {
		auto& last_frame = mStack.last();
		last_frame.mIp = last_frame.mMethod->mScript->mBytecode.mInstructionIdx;
	}

	frame.mMethod->mScript->mBytecode.mArgumentsLoaded = 0;
	frame.mMethod->mScript->mBytecode.mInstructionIdx = 0;
	obj::NDO->refinc(frame.mMethod);
	mStack.push(frame);
}

void CallStack::leave() {
	auto frame = mStack.last();
	obj::NDO->destroy(frame.mMethod);
	mStack.pop();

	if (mStack.loadLen()) {
		auto& last_frame = mStack.last();
		last_frame.mMethod->mScript->mBytecode.mInstructionIdx = last_frame.mIp;
	}
}

ByteCode* CallStack::getBytecode() {
	return &mStack.last().mMethod->mScript->mBytecode;
}

tp::halni CallStack::len() const {
	return mStack.loadLen();
}