
#pragma once

#include "stringt.h"
#include "list.h"
#include "map.h"

#include "instruction.h"
#include "statement.h"
#include "constants.h"

namespace obj {
	struct MethodObject;

	namespace BCgen {

		struct FunctionDefinition {
			
			FunctionDefinition* mPrnt = NULL;

			// signature
			tp::string mFunctionId;
			tp::List<ConstObject*> mArgsOrder;

			ConstObjectsPool mConstants;
			tp::HashMap<ConstObject*, tp::string> mLocals;
			tp::List<Instruction> mInstructions;

			FunctionDefinition(tp::string function_id, tp::Array<tp::string> args, FunctionDefinition* prnt);
			FunctionDefinition() {}

			void generateByteCode(ByteCode& out);

			tp::ListNode<Instruction>* inst(Instruction inst);
			
			void EvalExpr(Expression* expr);
			void EvalStatement(Statement* expr);

			ConstObject* defineLocal(tp::string id);
		};

		void init();
		void deinit();
		void Genereate(ByteCode& out, StatementScope* body);
		bool Compile(MethodObject* obj);
	};
};