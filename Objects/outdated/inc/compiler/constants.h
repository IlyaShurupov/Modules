
#pragma once

#include "interpreter/bytecode.h"
#include "core/object.h"

namespace obj {
	namespace BCgen {

		struct ConstObject {
			obj::Object* mObj = NULL;
			tp::alni mConstIdx = 0;

			ConstObject();
			ConstObject(obj::Object* mObj);
		};


		struct ConstObjectsPool {
			tp::HashMap<ConstObject*, tp::string> mMethods;
			tp::HashMap<ConstObject*, tp::string> mStrings;
			tp::HashMap<ConstObject*, tp::alni> mIntegers;
			tp::HashMap<ConstObject*, tp::alnf> mFloats;
			ConstObject mBoolTrue;
			ConstObject mBoolFalse;

			bool mDelete = true;
			tp::alni mTotalObjects = 0;

			ConstObject* get(tp::alni val);
			ConstObject* get(tp::string val);
			ConstObject* get(tp::alnf val);
			ConstObject* get(bool val);

			ConstObject* addMethod(tp::string method_id, obj::Object* method);
			ConstObject* registerObject(obj::Object* obj);
			void save(tp::Array<ConstData>& out);

			~ConstObjectsPool();
		};
	};
};