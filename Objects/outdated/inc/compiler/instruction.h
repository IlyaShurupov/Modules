
#pragma once

#include "interpreter/opcodes.h"

#include "core/object.h"

#include "list.h"

namespace obj {
	namespace BCgen {

		struct ConstObject;

		struct Instruction {

			OpCode mOp = OpCode::NONE;

			enum class ArgType {
				NO_ARG,
				PARAM,
				CONST,
			} mArgType = ArgType::NO_ARG;

			enum class InstType {
				NONE,
				JUMP,
				JUMP_IF,
				JUMP_IF_NOT,
				EXEC,
				PURE_CONST,
			} mInstType = InstType::NONE;

			tp::alni mParam = 0;
			tp::alni mParamBytes = 1;

			ConstObject* mConstData = NULL;
			ConstObject* mConstData2 = NULL;

			tp::alni mInstIdx = 0;
			tp::ListNode<Instruction>* mInstTarget = NULL;

			Instruction();
			Instruction(ConstObject* constData);
			Instruction(OpCode op);
			Instruction(OpCode op, ConstObject* constData);
			Instruction(OpCode op, ConstObject* constData, ConstObject* constData2);
			Instruction(OpCode op, tp::alni param, tp::alni nBytes);
			Instruction(tp::ListNode<Instruction>* inst, InstType jump_type);
		};
	};
};