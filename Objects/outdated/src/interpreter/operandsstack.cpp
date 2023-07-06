
#include "interpreter/operandsstack.h"

using namespace obj;

OperandStack::OperandStack() {
	mBuff = new Operand[MAX_STACK_SIZE];
	mIdx = 0;
}

void OperandStack::push(Operand operand) {
	RelAssert(MAX_STACK_SIZE - 1 > mIdx && "stack overflow");
	mBuff[mIdx] = operand;
	mIdx++;
}

void OperandStack::pop() {
	RelAssert(mIdx != NULL && "stack overflow");
	mIdx--;
}

Operand OperandStack::getOperand() {
	auto ret = mBuff[mIdx - 1];
	mIdx--;
	return ret;
}

OperandStack::~OperandStack() {
	delete[] mBuff;
}
