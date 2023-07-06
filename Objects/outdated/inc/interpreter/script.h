#pragma once

#include "interpreter/bytecode.h"

namespace obj {
	struct Script {
		obj::StringObject* mReadable;
		ByteCode mBytecode;

		void compile();
	};
};