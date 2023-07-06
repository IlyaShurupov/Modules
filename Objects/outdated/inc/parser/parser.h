#pragma once

#include "compiler/statement.h"
#include "compiler/expression.h"

#include "npple.h"
#include "tokenizer.h"

namespace obj {

	class Parser {

		struct Token {

			tp::string token;

			tp::alni integer;
			tp::alnf floating;
			bool boolean;
			tp::string str;

			enum TokType : tp::alni {
				__START__ = 0,
				ID,
				CONST_INT,
				CONST_FLOAT,
				CONST_STRING,
				SCOPE_IN,
				SCOPE_OUT,
				ASSIGN,
				DEF_FUNC,
				PRINT,
				IF,
				ELSE,
				WHILE,
				BRACKET_IN,
				BRACKET_OUT,
				COMMA,
				NEW,
				CHILD,
				RETURN,
				EQUAL,
				NOT_EQUAL,
				MORE,
				LESS,
				QE_OR_MORE,
				QE_OR_LESS,
				BOOL_NOT,
				BOOL_AND,
				BOOL_OR,
				ADD,
				MUL,
				SUB,
				DIV,
				QUOTES,
				CONST_TRUE,
				CONST_FALSE,
				STM_END,
				SPACE,
				VAR,
				CLASS_DEF,
				SELF,
				COMMENT_BLOCK,
				__END__,
				NO_TOKEN,
				FAILED,
				SOURCE_END_TOKEN,
			} type;

			operator TokType() {
				return type;
			}

			bool isAriphm();
			bool isBoolean();
		};

		typedef tp::SimpleTokenizer<char, Token::TokType, Token::NO_TOKEN, Token::FAILED, Token::SOURCE_END_TOKEN> Tokenizer;
		Tokenizer mTok;

		enum class ExprType {
			BOOLEAN_NOT = 0,
			BOOLEAN,
			Ariphm,
			NEW,
			LOCAL,
			CONST,
			Compound
		};

		enum class StmType {
			DefFunc = 0,
			DefLocal,
			Return,
			Print,
			If,
			While,
			Copy,
			Call,
			CLASS_DEF
		};

	public:
		
		Parser();

		struct Error {
			tp::string mDescr = "No Description";
			tp::alni mAdvanecedIdx = NULL;

			Error() {}
			Error(tp::string descr, tp::alni idx) {
				mDescr = descr;
				mAdvanecedIdx = idx;
			}

			tp::Tuple<tp::alni, tp::alni> get_err_location(const char* stream) {
				tp::alni line = 1;
				tp::alni colomn = 1;

				//assert(mAdvanecedIdx);

				for (auto i : tp::Range(mAdvanecedIdx)) {
					if (stream[i] == '\n') {
						colomn = 0;
						line++;
					}
					colomn++;
				}

				return { line, colomn };
			}
		};

		struct Resault {
			Error* err = NULL;
			obj::BCgen::StatementScope* scope = NULL;
		} mRes;

		Resault parse(const tp::string& stream);

	private:
		
		Token tokRead();
		Token tokLookup();
		void tokSkip();
		bool tokInputLeft();
		Tokenizer::Cursor tokGetCursor();
		void tokSetCursor(Tokenizer::Cursor);

		BCgen::Expression* parseExprCompound();
		BCgen::Expression* parseExprNEW();
		BCgen::Expression* parseExprLOCAL();
		BCgen::Expression* parseExprCONST();
		BCgen::Expression* parseExprCHILD(BCgen::Expression* prnt);
		BCgen::Expression* parseExprCALL(BCgen::Expression* prnt);
		BCgen::Expression* parseExprAriphm();
		BCgen::Expression* parseExprBOOLEAN();
		BCgen::Expression* parseExprBOOLEAN_NOT();
		BCgen::Expression* parseExprFUNC();
		BCgen::Expression* parseExprChain(BCgen::Expression* prnt);

		BCgen::Expression* parseExpr(tp::init_list<ExprType> expressions = {
			ExprType::BOOLEAN_NOT,
			ExprType::BOOLEAN,
			ExprType::Ariphm,
			ExprType::NEW,
			ExprType::LOCAL,
			ExprType::CONST,
		});
		
		BCgen::Statement* parseStmCall();
		BCgen::Statement* parseStmDefFunc();
		BCgen::Statement* parseStmDefLocal();
		BCgen::Statement* parseStmReturn();
		BCgen::Statement* parseStmPrint();
		BCgen::Statement* parseStmIf();
		BCgen::Statement* parseStmWhile();
		BCgen::Statement* parseStmCopy();
		BCgen::Statement* parseStmClassDef();
		
		BCgen::Statement* parseStm(tp::init_list<StmType> stm_types = {
			StmType::DefFunc,
			StmType::DefLocal,
			StmType::Return,
			StmType::Print,
			StmType::If,
			StmType::While,
			StmType::Copy,
			StmType::Call,
			StmType::CLASS_DEF
		});

		BCgen::StatementScope* parseScope(bool aPushToScopeStack);
	};
};