#pragma once

#include "bytecode.h"
#include "core/object.h"

namespace obj {

	// can be other aligned value as well
	typedef obj::Object* Operand;

	class OperandStack {

		enum : tp::alni {
			MAX_STACK_SIZE = 512
		};

	public:

		Operand* mBuff;
		tp::ualni mIdx;

		OperandStack();
		void push(Operand operand);
		void pop();

		Operand getOperand();

		template <typename ObjectType>
		ObjectType* getOperand() {
			auto operand = getOperand();
			auto ret = NDO_CAST(ObjectType, operand);
			RelAssert(ret && "Operand Has Invalid Object Type");
			return ret;
		}

		~OperandStack();
	};

};