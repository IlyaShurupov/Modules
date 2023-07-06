
#pragma once

#include "opcodes.h"

#include "primitives/intobject.h"
#include "primitives/stringobject.h"

#include "array.h"
#include "stringt.h"

namespace obj {

	typedef Object* ConstData;

	struct ByteCode {
		tp::Array<ConstData> mConstants;
		tp::Array<OpCode> mInstructions;
		tp::ualni mInstructionIdx = 0;
		tp::ualni mArgumentsLoaded = 0;

		~ByteCode() {
			for (auto const_obj : mConstants) {
				NDO->destroy(const_obj.data());
			}
		}
	};
};