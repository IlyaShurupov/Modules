
#pragma once

#include "stringt.h"
#include "array.h"

#include "interpreter/opcodes.h"

namespace obj {
	namespace BCgen {

		struct ExpressionChild;
		struct ExpressionCall;

		struct Expression {
			
			enum class Type {
				NONE,
				NEW,
				LOCAL,
				CONST,
				CHILD,
				CALL,
				ARIPHM,
				FUNC,
				BOOLEAN,
				SELF,
			} mType = Type::NONE;

			bool mValueUsed = false;

			Expression();
			Expression(Type type);

			ExpressionChild* ExprChild(tp::string id);
			ExpressionCall* ExprCall(tp::init_list<Expression*> args);
		};

		struct ExpressionNew : public Expression {
			tp::string mNewType;
			ExpressionNew(tp::string type);
		};

		struct ExpressionLocal : public Expression {
			tp::string mLocalId;
			ExpressionLocal(tp::string id);
		};

		struct ExpressionFunc : public Expression {
			tp::string mFuncId;
			ExpressionFunc(tp::string id);
		};

		struct ExpressionChild : public Expression {
			Expression* mParent = NULL;
			tp::string mLocalId;
			bool mMethod = false;
			ExpressionChild(Expression* mParent, tp::string id);
		};

		struct ExpressionCall : public Expression {
			Expression* mParent = NULL;
			tp::Array<Expression*> mArgs;
			ExpressionCall(Expression* mParent, tp::init_list<Expression*> args);
		};

		struct ExpressionAriphm : public Expression {
			Expression* mLeft = NULL;
			Expression* mRight = NULL;
			OpCode mOpType;
			ExpressionAriphm(Expression* left, Expression* right, OpCode type);
		};

		struct ExpressionBoolean : public Expression {
			Expression* mLeft = NULL;
			Expression* mRight = NULL;

			enum class BoolType : tp::uint1 {
				AND = 24U, 
				OR,
				EQUAL,
				NOT_EQUAL,
				MORE,
				LESS,
				EQUAL_OR_MORE,
				EQUAL_OR_LESS,
				NOT,
			} mBoolType;

			ExpressionBoolean(Expression* left, Expression* right, BoolType type);
			ExpressionBoolean(Expression* invert);
		};

		struct ExpressionConst : public Expression {
			enum ConstType { STR, INT, BOOL, FLT } mConstType;
			tp::string str;
			tp::alni integer = 0;
			tp::alnf floating = 0;
			bool boolean = 0;

			ExpressionConst(tp::string val);
			ExpressionConst(const char* val);
			ExpressionConst(tp::alni val);
			ExpressionConst(tp::int4 val);
			ExpressionConst(tp::flt4 val);
			ExpressionConst(tp::alnf val);
			ExpressionConst(bool val);
		};

		struct ExpressionSelf : public Expression {
			ExpressionSelf();
		};

		ExpressionLocal* ExprLocal(tp::string id);
		ExpressionSelf* ExprSelf();
		ExpressionFunc* ExprFunc(tp::string id);
		ExpressionNew* ExprNew(tp::string id);
		ExpressionAriphm* ExprAriphm(Expression* left, Expression* right, OpCode type);
		ExpressionBoolean* ExprBool(Expression* left, Expression* right, OpCode type);
		ExpressionBoolean* ExprBoolNot(Expression* invert);
		template <typename ConstType>
		ExpressionConst* ExprConst(ConstType val) {
			return new ExpressionConst(val);
		}
	};
};