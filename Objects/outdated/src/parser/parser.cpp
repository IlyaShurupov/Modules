
#include "parser/parser.h"
#include "log.h"
#include "list.h"
#include "compiler/function.h"
#include <cstdlib>

#define MAX_STREAM_LENGTH 1024 * 8 * 200

#define REQUIRE(expr, desc, on_catch) DBG_BREAK(mRes.err); if (!(expr)) { mRes.err = new Error(desc, mTok.getCursorPrev().mAdvancedOffset); on_catch; return NULL; }
#define CHECK(expr, on_catch) if (!(expr)) { on_catch; return NULL; }
#define CHECK_ERROR(value, on_catch) if (mRes.err || !value) { on_catch; return NULL; }

using namespace tp;
using namespace obj;
using namespace BCgen;

bool Parser::Token::isAriphm() {
	switch (type) {
	case Token::ADD:
	case Token::SUB:
	case Token::DIV:
	case Token::MUL:
		return true;
	default:
		return false;
	}
}

bool Parser::Token::isBoolean() {
	switch (type) {
	case Token::EQUAL:
	case Token::NOT_EQUAL:
	case Token::BOOL_AND:
	case Token::BOOL_OR:
	case Token::MORE:
	case Token::LESS:
	case Token::QE_OR_MORE:
	case Token::QE_OR_LESS:
		return true;
	default:
		return false;
	}
}

Parser::Parser() {
	mTok.build({
		{ "\n|\t| |\r", Token::SPACE },
		{ "var", Token::VAR },
		{ "class", Token::CLASS_DEF },
		{ "self",	 Token::SELF },
		{ "\\{", Token::SCOPE_IN },
		{ "\\}", Token::SCOPE_OUT },
		{ "=", Token::ASSIGN },
		{ "def", Token::DEF_FUNC },
		{ "<<",	 Token::PRINT },
		{ "if",	 Token::IF },
		{ "else",	 Token::ELSE },
		{ "while", Token::WHILE },
		{ "\\(", Token::BRACKET_IN },
		{ "\\)", Token::BRACKET_OUT },
		{ ",", Token::COMMA },
		{ "new", Token::NEW },
		{ "\\.",	 Token::CHILD },
		{ "return",	 Token::RETURN },
		{ "==",	 Token::EQUAL },
		{ "!=",	 Token::NOT_EQUAL },
		{ ">", Token::MORE },
		{ "<", Token::LESS },
		{ ">=",	 Token::QE_OR_MORE },
		{ "<=",	 Token::QE_OR_LESS },
		{ "!", Token::BOOL_NOT },
		{ "&&",	 Token::BOOL_AND },
		{ "\\|\\|",	 Token::BOOL_OR },
		{ "\\+", Token::ADD },
		{ "\\*", Token::MUL },
		{ "\\-", Token::SUB },
		{ "/", Token::DIV },
		{ ";", Token::STM_END },
		{ "true",	 Token::CONST_TRUE },
		{ "false", Token::CONST_FALSE },
		{ "((\\-)|(\\+))?[0-9]+i?",	 Token::CONST_INT },
		{ "((\\-)|(\\+))?([0-9]+)(\\.)([0-9]*)?f?",	 Token::CONST_FLOAT },
		{ "(/\\*){\\*-\\*}*(\\*/)",	 Token::COMMENT_BLOCK },
		{ "\"{\"-\"}*\"",	 Token::CONST_STRING },
		{ "([a-z]|[A-Z]|_)+([a-z]|[A-Z]|[0-9]|_)*",	 Token::ID },
	});
}

tp::string to_string(const char* stream, tp::alni len) {
	tp::string out;
	out.reserve(len + 1);
	tp::memcp(out.get_writable(), stream, len);
	out.get_writable()[len] = '\0';
	return out;
}

Parser::Token Parser::tokRead() {
	Token out;

	if (!tokInputLeft()) {
		Token out;
		out.type = Token::NO_TOKEN;
		return out;
	}

	auto crs_start = mTok.getCursor();
	out.type = mTok.readTok();
	out.token = to_string(crs_start.str(), mTok.lastTokLEn());

	switch (out.type) {
		case Token::CONST_FALSE:
			out.boolean = false;
			break;
		case Token::CONST_TRUE:
			out.boolean = true;
			break;
		case Token::CONST_INT: {
			char* p_end;
			out.integer = std::strtol(out.token.cstr(), &p_end, 10);
			assert(out.token.cstr() != p_end);
			break;
		}
		case Token::CONST_FLOAT: {
			char* p_end;
			out.floating = std::strtod(out.token.cstr(), &p_end);
			assert(out.token.cstr() != p_end);
			break;
		}
		case Token::CONST_STRING: {
			out.str = to_string(crs_start.str() + 1, mTok.lastTokLEn() - 2);
			break;
		}
	}

	return out;
}

Parser::Token Parser::tokLookup() {
	auto crs = mTok.getCursor();
	auto out = tokRead();
	mTok.setCursor(crs);
	return out;
}

bool Parser::tokInputLeft() {
	auto type = mTok.lookupTok();

	while (mTok.inputLeft() && type == Token::SPACE || (type >= Token::__END__ && type <= Token::__START__)) {
		mTok.skipTok();
		type = mTok.lookupTok();
	}

	return type != Token::SOURCE_END_TOKEN;
}

void Parser::tokSkip() {
	if (!tokInputLeft()) {
		return;
	}
	mTok.skipTok();
}

Parser::Tokenizer::Cursor Parser::tokGetCursor() { return mTok.getCursor();}
void Parser::tokSetCursor(Tokenizer::Cursor crs) { mTok.setCursor(crs); }

Expression* Parser::parseExprCompound() {
	CHECK(tokRead() == Token::BRACKET_IN, {});
	auto ret = parseExpr();
	CHECK_ERROR(ret, {});
	REQUIRE(tokRead() == Token::BRACKET_OUT, "Expected Closing Bracket", {});
	return ret;
}

Expression* Parser::parseExprNEW() {
	CHECK(tokRead() == Token::NEW, {});
	auto name = tokRead();
	REQUIRE(name == Token::ID, "Expected Identifier", {});

	REQUIRE(tokRead() == Token::BRACKET_IN, "Expected opening brackets", {});
	REQUIRE(tokRead() == Token::BRACKET_OUT, "Expected closing brackets", {});

	return ExprNew(name.token);
}

Expression* Parser::parseExprLOCAL() {
	auto local_id_tok = tokRead();
	if (local_id_tok == Token::ID) {
		return ExprLocal(local_id_tok.token);
	}
	else if (local_id_tok == Token::SELF) {
		return ExprSelf();
	}
	return NULL;
}

Expression* Parser::parseExprCONST() {
	auto tok = tokRead();
	switch (tok) {
	case Token::CONST_FALSE:
		return ExprConst(false);
	case Token::CONST_TRUE:
		return ExprConst(true);
	case Token::CONST_INT:
		return ExprConst(tok.integer);
	case Token::CONST_FLOAT:
		return ExprConst(tok.floating);
	case Token::CONST_STRING:
		return ExprConst(tok.str);
	default:
		REQUIRE(0, "not a const Expr", {});
	}
}

Expression* Parser::parseExprCHILD(Expression* prnt) {
	CHECK(tokRead() == Token::CHILD, {});
	auto id = tokRead();
	REQUIRE(id == Token::ID, "Exprected Identifier", {});
	return prnt->ExprChild(id.str);
}

Expression* Parser::parseExprCALL(Expression* prnt) {
	CHECK(tokRead() == Token::BRACKET_IN, {});

	List<Expression*> args;

READ_ARG:
	Expression* expr = NULL;
	auto tok = tokLookup();
	REQUIRE(tok != Token::NO_TOKEN, "Missing Closing Bracket", {});

	if (tok != Token::BRACKET_OUT) {

		expr = parseExpr();
		CHECK_ERROR(expr, {
			for (auto arg : args) {
				delete arg.data();
			}
		});

		args.pushBack(expr);

		tok = tokLookup();
		if (tok == Token::COMMA) {
			tokSkip();
			goto READ_ARG;
		}
		else if (tok != Token::BRACKET_OUT) {
			for (auto arg : args) {
				delete arg.data();
			}
			REQUIRE(0, "Expected A Closing Bracket", {});
		}
	}
	tokSkip();

	auto out = prnt->ExprCall({});
	out->mArgs.reserve(args.length());

	for (auto arg : args) {
		out->mArgs[arg.idx()] = arg.data();
	}

	return out;
}

Expression* Parser::parseExprAriphm() {

	tp::init_list<ExprType> expessions = {
		ExprType::Compound,
		ExprType::NEW,
		ExprType::LOCAL,
		ExprType::CONST,
	};

	Expression* left = parseExpr(expessions);
	CHECK_ERROR(left, {});

	Expression* ret = NULL;

PARSE:

	Expression* right = NULL;

	auto tok = tokLookup();
	if (tok.isAriphm()) {
		tokSkip();

			
		right = parseExpr(expessions);

		if (mRes.err) {
			if (ret) {
				goto PRECEDENCE;
			}

			delete left;
			return NULL;
		}

	}
	else {
		CHECK(ret, {
			delete left;
		});
		goto PRECEDENCE;
	}

	switch (tok) {
	case Token::ADD:
		ret = ExprAriphm(left, right, OpCode::OBJ_ADD);
		break;
	case Token::SUB:
		ret = ExprAriphm(left, right, OpCode::OBJ_SUB);
		break;
	case Token::DIV:
		ret = ExprAriphm(left, right, OpCode::OBJ_DIV);
		break;
	case Token::MUL:
		ret = ExprAriphm(left, right, OpCode::OBJ_MUL);
	}

	left = ret;
	goto PARSE;

PRECEDENCE:
	// FIXME: Not Implemented

	return ret;
}

Expression* Parser::parseExprBOOLEAN() {

	tp::init_list<ExprType> expessions = {
		ExprType::Compound,
		ExprType::NEW,
		ExprType::LOCAL,
		ExprType::CONST,
		ExprType::BOOLEAN_NOT,
	};


	Expression* left = parseExpr(expessions);
	CHECK_ERROR(left, {});

	Expression* ret = NULL;

PARSE:

	Expression* right = NULL;

	auto tok = tokLookup();
	if (tok.isBoolean()) {
		tokSkip();

		right = parseExpr(expessions);

		if (mRes.err) {
			if (ret) {
				goto PRECEDENCE;
			}

			delete left;
			return NULL;
		}

	}
	else {
		CHECK(ret, {
			delete left;
		});
		goto PRECEDENCE;
	}

	switch (tok) {
	case Token::EQUAL:
		ret = ExprBool(left, right, OpCode::EQUAL);
		break;
	case Token::NOT_EQUAL:
		ret = ExprBool(left, right, OpCode::NOT_EQUAL);
		break;
	case Token::BOOL_AND:
		ret = ExprBool(left, right, OpCode::AND);
		break;
	case Token::BOOL_OR:
		ret = ExprBool(left, right, OpCode::OR);
		break;
	case Token::MORE:
		ret = ExprBool(left, right, OpCode::MORE);
		break;
	case Token::LESS:
		ret = ExprBool(left, right, OpCode::LESS);
		break;
	case Token::QE_OR_MORE:
		ret = ExprBool(left, right, OpCode::EQUAL_OR_MORE);
		break;
	case Token::QE_OR_LESS:
		ret = ExprBool(left, right, OpCode::EQUAL_OR_LESS);
	}

	left = ret;
	goto PARSE;

PRECEDENCE:
	// FIXME: Not Implemented

	return ret;
}

Expression* Parser::parseExprBOOLEAN_NOT() {
	CHECK(tokRead() == Token::BOOL_NOT, {});

	tp::init_list<ExprType> exprs = {
		ExprType::Compound,
		ExprType::BOOLEAN,
		ExprType::Ariphm,
		ExprType::NEW,
		ExprType::LOCAL,
		ExprType::CONST,
	};

	auto out = parseExpr(exprs);
	CHECK_ERROR(out, {});

	return ExprBoolNot(out);
}

Expression* Parser::parseExprFUNC() {
	auto op_tok = tokRead();
	CHECK(op_tok == Token::ID, {});
	return ExprFunc(op_tok.token);
}

// passed prnt isno longer responsability of callee
// suc : returns top level parent
// err : returns NULL
Expression* Parser::parseExprChain(Expression* prnt) {
		
	PARSE_CHAIN:
	auto tok = tokLookup();

	switch (tok) {
		case Token::CHILD: {
			tokRead();
					
			auto child_id = tokRead();
			REQUIRE(child_id == Token::ID, "expected an id", {
				delete prnt;
			});

			prnt = prnt->ExprChild(child_id.token);
			goto PARSE_CHAIN;
		}

		case Token::BRACKET_IN: {

			auto new_prnt = parseExprCALL(prnt);
			CHECK_ERROR(new_prnt, { 
				delete prnt;
			});

			if (prnt->mType == Expression::Type::CHILD) {
				((ExpressionChild*)prnt)->mMethod = true;
			}

			prnt = new_prnt;
			goto PARSE_CHAIN;
		}
	}

	return prnt;
}

Expression* Parser::parseExpr(tp::init_list<ExprType> expressions) {

	Expression* out = NULL;

	List<Error*> errors;

	for (auto expr_type : expressions) {
		auto crs = tokGetCursor();
		switch (expr_type) {
			case Parser::ExprType::BOOLEAN_NOT:
				out = parseExprBOOLEAN_NOT();
				break;
			case Parser::ExprType::BOOLEAN:
				out = parseExprBOOLEAN();
				break;
			case Parser::ExprType::Ariphm:
				out = parseExprAriphm();
				break;
			case Parser::ExprType::NEW:
				out = parseExprNEW();
				break;
			case Parser::ExprType::LOCAL:
				out = parseExprLOCAL();
				break;
			case Parser::ExprType::CONST:
				out = parseExprCONST();
				break;
			case Parser::ExprType::Compound:
				out = parseExprCompound();
				break;
		}

		if (out) {
			break;
		}

		tokSetCursor(crs);
		if (mRes.err) {
			errors.pushBack(mRes.err);
			mRes.err = NULL;
		}
	}

	if (out) {
		// check for child expression and calls
		out = parseExprChain(out);

		// check errors
		if (mRes.err) {
			errors.pushBack(mRes.err);
			mRes.err = NULL;
			goto ERRORS;
		}

		for (auto err : errors) {
			delete err.data();
		}

		return out;
	}

	ERRORS:
	Error* error = NULL;
	tp::alni max_advanced = 0;
	for (auto err : errors) {
		if (err.data()->mAdvanecedIdx && err.data()->mAdvanecedIdx > max_advanced) {
			max_advanced = err.data()->mAdvanecedIdx;
			error = err.data();
		}
	}

	if (error) {
		mRes.err = new Error(*error);

		for (auto err : errors) {
			delete err.data();
		}

		return NULL;
	}
	REQUIRE(0, "Expected and expression", {});
}

Statement* Parser::parseStmCall() {
		
	Expression* expr = parseExpr();
	CHECK_ERROR(expr, {});

	REQUIRE(expr->mType == Expression::Type::CALL, 
		"expression statement can only be function call", { 
		delete expr; 
	});

	if (tokRead() != Token::STM_END) {
		delete expr;
		REQUIRE(0, "Expected End Of Statement", {});
	}

	return StmIgnore(expr);
}

Statement* Parser::parseStmDefFunc() {
	CHECK(tokRead() == Token::DEF_FUNC, {});
	List<tp::string> args;
	auto name = tokRead();
	REQUIRE(name == Token::ID, "Expected an Identifier", {});

	REQUIRE(tokRead() == Token::BRACKET_IN, "Expected Opening Bracket", {});

READ_ARG:
	auto tok = tokLookup();

	REQUIRE(tok != Token::NO_TOKEN, "Missing Closing Bracket", {});

	if (tok == Token::ID) {
		args.pushBack(tok.token);
		tokSkip();
		tok = tokLookup();
	}

	if (tok == Token::COMMA) {
		tokSkip();
		goto READ_ARG;
	}

	REQUIRE(tok == Token::BRACKET_OUT, "Expected Closing Bracket", {});
	tokSkip();

	auto func_def = StmDefFunc(name.token, {}, {});

	func_def->mArgs.reserve(args.length());
	for (auto iter : args) {
		func_def->mArgs[iter.idx()] = iter.data();
	}

	StatementScope* scope = parseScope(true);
	CHECK_ERROR(scope, {
		delete func_def;
	});

	func_def->mStatements.reserve(scope->mStatements.length());
	for (auto stm : scope->mStatements) {
		func_def->mStatements[stm.idx()] = stm.data();
	}

	delete scope;
	return func_def;
}

Statement* Parser::parseStmDefLocal() {
	CHECK(tokRead() == Token::VAR, {});

	auto tok = tokRead();
	REQUIRE(tok == Token::ID, "Expected an Identifier", {});
	REQUIRE(tokRead() == Token::ASSIGN, "Expected an assigment", {});

	auto expr = parseExpr();
	CHECK_ERROR(expr, {});

	if (tokRead() != Token::STM_END) {
		delete expr;
		REQUIRE(0, "Expected End Of Statement", {});
	}

	return StmDefLocal(tok.token, expr);
}

Statement* Parser::parseStmReturn() {
	CHECK(tokRead() == Token::RETURN, {});
	auto crs = tokGetCursor();

	auto expr = parseExpr();
	if (mRes.err) {
		tokSetCursor(crs);
		delete mRes.err;
		mRes.err = NULL;
	}

	if (tokRead() != Token::STM_END) {
		if (expr) delete expr;
		REQUIRE(0, "Expected End Of Statement", {});
	}

	return expr ? StmReturn(expr) : StmReturn();
}

Statement* Parser::parseStmPrint() {
	CHECK(tokRead() == Token::PRINT, {});
	auto expr = parseExpr();
	CHECK_ERROR(expr, {});

	if (tokRead() != Token::STM_END) {
		delete expr;
		REQUIRE(0, "Expected End Of Statement", {});
	}
	return StmPrint(expr);
}

Statement* Parser::parseStmIf() {
	CHECK(tokRead() == Token::IF, {});

	// single if
	REQUIRE(tokRead() == Token::BRACKET_IN, "Expected Opening Bracket", {});
	auto expr = parseExpr();
	CHECK_ERROR(expr, {});

	if (tokRead() != Token::BRACKET_OUT) {
		delete expr;
		REQUIRE(0, "Expected Closing Bracket", {});
	}

	auto ontrue = parseScope(false);
	CHECK_ERROR(ontrue, {});

	// if-else
	auto crs = tokGetCursor();
	if (tokRead() == Token::ELSE) {
		StatementScope* onfalse = parseScope(false);
		CHECK_ERROR(onfalse, {
			delete ontrue;
			delete expr;
		});

		return StmIf(expr, ontrue, onfalse);
	}
	tokSetCursor(crs);

	return StmIf(expr, ontrue, NULL);
}

Statement* Parser::parseStmWhile() {
	CHECK(tokRead() == Token::WHILE, {});

	REQUIRE(tokRead() == Token::BRACKET_IN, "Expected Opening Bracket", {});
	auto expr = parseExpr();
	CHECK_ERROR(expr, {});

	if (tokRead() != Token::BRACKET_OUT) {
		delete expr;
		REQUIRE(0, "Expected Closing Bracket", {});
	}

	auto scope = parseScope(false);
	CHECK_ERROR(scope, {});

	return StmWhile(expr, scope);
}

Statement* Parser::parseStmCopy() {
	auto left = parseExpr();
	CHECK_ERROR(left, {});

	CHECK(tokRead() == Token::ASSIGN, {});

	auto right = parseExpr();
	CHECK_ERROR(right, {
		delete left;
	});

	if (tokRead() != Token::STM_END) {
		delete left;
		delete right;
		REQUIRE(0, "Expected End Of Statement", {});
	}

	return StmCopy(left, right);
}

Statement* Parser::parseStmClassDef() {
	CHECK(tokRead() == Token::CLASS_DEF, {});
		
	auto id = tokRead();
	REQUIRE(id == Token::ID, "Expected an Identifier", {});

	auto scope = parseScope(true);
	CHECK_ERROR(scope, {});

	return StmClassDef(id.token, scope);
}

Statement* Parser::parseStm(tp::init_list<StmType> stm_types) {
	Statement* out = NULL;

	List<Error*> errors;

	for (auto stm_type : stm_types) {
		auto crs = tokGetCursor();
		switch (stm_type) {
			case StmType::DefFunc:
				out = parseStmDefFunc();
				break;
			case StmType::DefLocal:
				out = parseStmDefLocal();
				break;
			case StmType::Return:
				out = parseStmReturn();
				break;
			case StmType::Print:
				out = parseStmPrint();
				break;
			case StmType::If:
				out = parseStmIf();
				break;
			case StmType::While:
				out = parseStmWhile();
				break;
			case StmType::Copy:
				out = parseStmCopy();
				break;
			case StmType::Call:
				out = parseStmCall();
				break;
			case StmType::CLASS_DEF:
				out = parseStmClassDef();
				break;
		}

		if (out) {
			break;
		}

		tokSetCursor(crs);
		if (mRes.err) {
			errors.pushBack(mRes.err);
			mRes.err = NULL;
		}
	}

	if (out) {
		for (auto err : errors) {
			delete err.data();
		}
		return out;
	}

	Error* error = NULL;
	tp::alni max_advanced = 0;
	for (auto err : errors) {
		if (err.data()->mAdvanecedIdx && max_advanced < err.data()->mAdvanecedIdx) {
			max_advanced = err.data()->mAdvanecedIdx;
			error = err.data();
		}
	}

	if (error) {
		mRes.err = new Error(*error);

		for (auto err : errors) {
			delete err.data();
		}

		return NULL;
	}
		
	REQUIRE(0, "Exprected A Statement", {});
}

StatementScope* Parser::parseScope(bool aPushToScopeStack) {
	CHECK(tokRead() == Token::SCOPE_IN, {});

	auto out = new StatementScope({}, aPushToScopeStack);
	List<Statement*> stms;

READ_STM:
	auto crs = tokGetCursor();
	auto tok = tokRead();

	if (tok == Token::NO_TOKEN) {
		delete out;
		for (auto stm : stms) {
			delete stm.data();
		}
		REQUIRE(0, "Missing Scope Closing Bracket", {});
	}

	if (tok != Token::SCOPE_OUT) {
		tokSetCursor(crs);
			
		auto stm = parseStm();
		CHECK_ERROR(stm, {
			delete out;
			for (auto stm : stms) {
				delete stm.data();
			}
		});

		stms.pushBack(stm);
			
		goto READ_STM;
	}

	out->mStatements.reserve(stms.length());
	for (auto stm : stms) {
		out->mStatements[stm.idx()] = stm.data();
	}

	return out;
}

Parser::Resault Parser::parse(const tp::string& oscript) {
	mTok.bindSource(oscript.cstr());

	mRes.scope = new StatementScope({}, true);

	List<Statement*> stms;
	do {
		auto stm = parseStm();

		if (mRes.err || !stm) { // catch
			delete mRes.scope;
			mRes.scope = NULL;
			for (auto stm : stms) {
				delete stm.data();
			}

			return mRes;
		}

		stms.pushBack(stm);

	} while (tokInputLeft());

	mRes.scope->mStatements.reserve(stms.length());
	for (auto stm : stms) {
		mRes.scope->mStatements[stm.idx()] = stm.data();
	}

	return mRes;
}
