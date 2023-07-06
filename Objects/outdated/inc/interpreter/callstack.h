#pragma once

#include "core/object.h"
#include "primitives/classobject.h"

#include "map.h"

namespace obj {
	struct MethodObject;
};

namespace obj {

	struct ByteCode;

	struct CallStack {

		struct CallFrame {
			enum { CALL_DEPTH = 1024 };
			obj::Object* mSelf = NULL;
			obj::MethodObject* mMethod = NULL;
			tp::ualni mIp = 0;
		};

		void enter(const CallFrame& frame);
		void leave();
		ByteCode* getBytecode();
		tp::halni len() const;

		tp::ConstSizeStack<CallFrame, CallFrame::CALL_DEPTH> mStack;
	};
};