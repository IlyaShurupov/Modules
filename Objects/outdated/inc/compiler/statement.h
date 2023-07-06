
#pragma once

#include "expression.h"

namespace obj {
	namespace BCgen {
		struct Statement {
			enum class Type {
				NONE,
				SCOPE,
				DEF_FUNC,
				DEF_LOCAL,
				RET,
				PRINT,
				COPY,
				IF,
				WHILE,
				IGNORE,
				CALL,
				CLASS_DEF,
			} mType = Type::NONE;

			bool mValueUsed = false;

			Statement() {}
			Statement(Type type);
		};

		struct StatementScope : public Statement {
			tp::Array<Statement*> mStatements;
			bool mPushToScopeStack = false;

			StatementScope(tp::init_list<Statement*> statements, bool aPushToScopeStack);
		};

		struct StatementFuncDef : public Statement {
			tp::Array<tp::string> mArgs;
			tp::string mFunctionId;
			tp::Array<Statement*> mStatements;

			StatementFuncDef(tp::string function_id, tp::init_list<tp::string> args, tp::init_list<Statement*> statements);
		};

		struct StatementLocalDef : public Statement {
			tp::string mLocalId;
			Expression* mNewExpr = NULL;
			ExpressionConst* mConstExpr = NULL;
			bool mIsConstExpr = false;

			StatementLocalDef(tp::string id, Expression* value);
			StatementLocalDef(tp::string id, ExpressionConst* value);
		};

		struct StatementCopy : public Statement {
			Expression* mLeft = NULL;
			Expression* mRight = NULL;

			StatementCopy(Expression* left, Expression* right);
		};

		struct StatementReturn : public Statement {
			Expression* mRet = NULL;

			StatementReturn(Expression* ret);
			StatementReturn();
		};

		struct StatementPrint : public Statement {
			Expression* mTarget = NULL;

			StatementPrint(Expression* mTarget);
		};

		struct StatementIgnore : public Statement {
			Expression* mExpr = NULL;

			StatementIgnore(Expression* expr);
		};

		struct StatementIf : public Statement {
			Expression* mCondition = NULL;
			StatementScope* mOnTrue = NULL;
			StatementScope* mOnFalse = NULL;

			StatementIf(Expression* condition, StatementScope* on_true, StatementScope* on_false);
		};

		struct StatementWhile : public Statement {
			Expression* mCondition = NULL;
			StatementScope* mScope = NULL;

			StatementWhile(Expression* condition, StatementScope* scope);
		};

		struct StatementClassDef : public Statement {
			tp::string mClassId;
			StatementScope* mScope = NULL;

			StatementClassDef(tp::string class_id, StatementScope* scope);
		};

		// Helpers
		StatementFuncDef* StmDefFunc(tp::string id, tp::init_list<tp::string> args, tp::init_list<Statement*> stms);
		StatementLocalDef* StmDefLocal(tp::string id, Expression* value);
		StatementCopy* StmCopy(Expression* left, Expression* right);
		StatementPrint* StmPrint(Expression* target);
		StatementReturn* StmReturn(Expression* obj);
		StatementReturn* StmReturn();
		StatementIf* StmIf(Expression* condition, StatementScope* on_true, StatementScope* on_false);
		StatementScope* StmScope(tp::init_list<Statement*> statements, bool aPushToScopeStack);
		StatementWhile* StmWhile(Expression* condition, StatementScope* scope);
		StatementIgnore* StmIgnore(Expression* expr);
		StatementClassDef* StmClassDef(tp::string id, StatementScope* scope);
	};
};