
#pragma once

#include "array.h"
#include "stringt.h"

namespace obj {

	struct Interpreter;
	struct Object;
	struct ObjectType;

	struct TypeMethod {
		enum { MAX_ARGS = 8 };
		struct Arg {
			const char* descr = NULL;
			mutable Object* obj = NULL;
		};

		const char* nameid = NULL;
		const char* descr = NULL;

		mutable Object* self = NULL;
		Arg args[MAX_ARGS];
		
		tp::int1 mNargs = NULL;

		void (*exec)(const TypeMethod* tm) = NULL;

		mutable Arg ret;

		void operator()(Interpreter* interp) const;

		void init();
	};

	extern TypeMethod gDefaultTypeMethods[3];

	struct TypeMethods {
		enum : tp::int2 { MAX_TYPE_METHODS = 128 };

		TypeMethod* methods[MAX_TYPE_METHODS];
		tp::halni mNMethods = NULL;

		struct LookupKey {
			tp::int2 key = -1;
			tp::int2 type_depth = -1;
			operator bool() { return key != -1; }
		};

		static LookupKey presents(const ObjectType*, tp::string);
		static const TypeMethod* getMethod(const ObjectType*, LookupKey);

		void init();

		tp::halni nMethods() const;

	private:
		tp::int2 presents(tp::string) const;
		const TypeMethod* getMethod(tp::int2) const;
	};
	
};