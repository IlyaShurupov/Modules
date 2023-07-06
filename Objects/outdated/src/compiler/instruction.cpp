
#include "compiler/instruction.h"

using namespace obj;
using namespace tp;
using namespace BCgen;

Instruction::Instruction(OpCode op) : mOp(op), mArgType(ArgType::NO_ARG), mInstType(InstType::EXEC) {}

Instruction::Instruction() : mInstType(InstType::NONE) {}

Instruction::Instruction(ConstObject* constData)
	: mConstData(constData), mInstType(InstType::PURE_CONST) {}

Instruction::Instruction(OpCode op, ConstObject* constData)
	: mOp(op), mConstData(constData), mArgType(ArgType::CONST), mInstType(InstType::EXEC) {
}

Instruction::Instruction(OpCode op, ConstObject* constData, ConstObject* constData2)
	: mOp(op), mConstData(constData), mConstData2(constData2), mArgType(ArgType::CONST), mInstType(InstType::EXEC) {
}

Instruction::Instruction(OpCode op, tp::alni param, tp::alni nBytes)
	: mOp(op), mParam(param), mArgType(ArgType::PARAM), mParamBytes(nBytes), mInstType(InstType::EXEC) {
}

Instruction::Instruction(tp::ListNode<Instruction>* inst, InstType jump_type): mInstTarget(inst) {
	mInstType = jump_type;
}