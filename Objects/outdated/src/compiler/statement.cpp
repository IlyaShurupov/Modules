
#include "compiler/statement.h"

using namespace obj;
using namespace BCgen;


StatementFuncDef::StatementFuncDef(tp::string function_id, tp::init_list<tp::string> args, tp::init_list<Statement*> statements) {
	mType = Type::DEF_FUNC;
	mArgs = args;
	mStatements = statements;
	mFunctionId = function_id;
}

StatementLocalDef::StatementLocalDef(tp::string id, Expression* value) : mLocalId(id), mNewExpr(value) {
	mType = Type::DEF_LOCAL;
}

StatementLocalDef::StatementLocalDef(tp::string id, ExpressionConst* value) : mLocalId(id), mConstExpr(value), mIsConstExpr(true) {
	mType = Type::DEF_LOCAL;
}

StatementCopy::StatementCopy(Expression* left, Expression* right) : mLeft(left), mRight(right) {
	mType = Type::COPY;
}

StatementReturn::StatementReturn() {
	mType = Type::RET;
}

StatementReturn::StatementReturn(Expression* ret) : mRet(ret) {
	mType = Type::RET;
}

StatementPrint::StatementPrint(Expression* target) : mTarget(target) {
	mType = Type::PRINT;
}

StatementScope::StatementScope(tp::init_list<Statement*> statements, bool aPushToScopeStack) {
	mType = Type::SCOPE;
	mStatements = statements;
	mPushToScopeStack = aPushToScopeStack;
}

StatementIf::StatementIf(Expression* condition, StatementScope* on_true, StatementScope* on_false) {
	mType = Type::IF;
	mOnFalse = on_false;
	mOnTrue = on_true;
	mCondition = condition;
}

StatementWhile::StatementWhile(Expression* condition, StatementScope* scope) {
	mType = Type::WHILE;
	mScope = scope;
	mCondition = condition;
}

StatementIgnore::StatementIgnore(Expression* expr) {
	mType = Type::IGNORE;
	mExpr = expr;
}

StatementClassDef::StatementClassDef(tp::string class_id, StatementScope* scope) {
	mClassId = class_id;
	mScope = scope;
	mType = Type::CLASS_DEF;
}
// helpers 

StatementFuncDef* obj::BCgen::StmDefFunc(tp::string id, tp::init_list<tp::string> args, tp::init_list<Statement*> stms) {
	return new StatementFuncDef(id, args, stms);
}

StatementLocalDef* obj::BCgen::StmDefLocal(tp::string id, Expression* value) {
	if (value->mType == Expression::Type::CONST) {
		return new StatementLocalDef(id, (ExpressionConst*)value);
	}
	
	return new StatementLocalDef(id, value);
}

StatementCopy* obj::BCgen::StmCopy(Expression* left, Expression* right) {
	return new StatementCopy(left, right);
}

StatementPrint* obj::BCgen::StmPrint(Expression* target) {
	return new StatementPrint(target);
}

StatementReturn* obj::BCgen::StmReturn() {
	return new StatementReturn();
}

StatementReturn* obj::BCgen::StmReturn(Expression* obj) {
	return new StatementReturn(obj);
}

StatementIf* obj::BCgen::StmIf(Expression* condition, StatementScope* on_true, StatementScope* on_false) {
	return new StatementIf(condition, on_true, on_false);
}

StatementScope* obj::BCgen::StmScope(tp::init_list<Statement*> statements, bool aPushToScopeStack = false) {
	return new StatementScope(statements, aPushToScopeStack);
}

StatementWhile* obj::BCgen::StmWhile(Expression* condition, StatementScope* scope) {
	return new StatementWhile(condition, scope);
}

StatementIgnore* obj::BCgen::StmIgnore(Expression* expr) {
	return new StatementIgnore(expr);
}

StatementClassDef* obj::BCgen::StmClassDef(tp::string id, StatementScope* scope) {
	return new StatementClassDef(id, scope);
}