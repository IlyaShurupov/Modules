#pragma once

#include "core/object.h"
#include "map.h"

namespace obj {

	struct Scope {
		typedef tp::HashMap<obj::Object*, tp::string> ObjDict;
		typedef tp::List<obj::Object*> ObjList;

		ObjDict mLocals;
		ObjList mTemps;
		bool mChildReachable = true;

		~Scope();
	};

	class ScopeStack {

		enum : tp::alni {
			MAX_STACK_SIZE = 1024 * 4
		};

	public:

		Scope* mBuff;
		tp::ualni mIdx;
		tp::ualni mIterIdx;

		ScopeStack();
		void enterScope(bool aChildReachable);
		void leaveScope();
		void addLocal(obj::Object* local, tp::string id);
		void addTemp(obj::Object* tmp);
		void popTemp();
		void addTempReturn(obj::Object* ret);
		obj::Object* getLocal(tp::string& str);
		Scope* getCurrentScope();
		~ScopeStack();

	private:
		obj::Object* getLocalUtil(Scope& scope, tp::string* id);
	};

};