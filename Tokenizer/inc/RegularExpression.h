
#pragma once

#include "AutomataGraph.h"

#include "npple.h"

namespace tp {
	namespace RegEx {
		
		struct AstNode {
			enum Type {
				NONE,
				ANY,
				OR,
				IF,
				CLASS,
				COMPOUND,
				REPEAT,
				VAL,
			} mType = NONE;
		};

		template <typename tAlphabetType>
		struct AstVal : public AstNode {
			AstVal(tAlphabetType val) : mVal(val) { mType = VAL; }
			tAlphabetType mVal;
		};

		struct AstCompound : public AstNode {
			AstCompound() { mType = COMPOUND; }
			List<AstNode*> mChilds;
		};

		struct AstAlternation : public AstNode {
			AstAlternation() { mType = OR;	}
			AstNode* mFirst = NULL;
			AstNode* mSecond = NULL;
		};

		struct AstIf : public AstNode {
			AstIf() { mType = IF; }
			AstNode* mNode = NULL;
		};

		struct AstAny : public AstNode {
			AstAny() { mType = ANY; }
		};

		struct AstRepetition : public AstNode {
			AstRepetition() { mType = REPEAT; }
			AstNode* mNode = NULL;
			bool mPlus = false;
		};

		template <typename tAlphabetType>
		struct AstClass : public AstNode {
			AstClass() { mType = CLASS; }
			tp::List<Range<tAlphabetType>> mRanges;
			bool mExclude;
		};

		struct ParseError {
			const char* descr = NULL;
			uhalni offset = NULL;
			bool isError() { return descr != NULL; }
		};

		template <typename tAlphabetType, typename tStateType, typename tStateType tNoStateVal>
		class Parser {

			enum TokType : uint1 {
				TOK_COMPOUND_START = 0,
				TOK_COMPOUND_END,
				TOK_CLASS_START,
				TOK_CLASS_END,
				TOK_CLASS_START_EXCLUDE,
				TOK_CLASS_END_EXCLUDE,
				TOK_OR,
				TOK_IF,
				TOK_ANY,
				TOK_REPEAT,
				TOK_REPEAT_PLUS,
				TOK_HYPHEN,
				TOK_SPECIALS_END__,
				TOK_VAL,
				TOK_NONE,
			};

			tAlphabetType SpetialSymbols[TOK_SPECIALS_END__] = {
				'(', ')', '[', ']', '{', '}', '|', '?', '.', '*', '+', '-',
			};

			tAlphabetType mEscapeSymbol = '\\';

			struct Token {
				TokType type;
				tAlphabetType val;
			};

			const tAlphabetType* mSorce = NULL;
			uhalni mOffset = NULL;
			Token mCurToken;
			uhalni mTokLength = NULL;

		public:

			ParseError mError;

			// regular expression must be a zero termination string
			AstCompound* parse(const tAlphabetType* regex) {
				mSorce = regex;
				return parseRegEx();
			}

		private:

			AstCompound* parseRegEx() {
				auto out = new AstCompound();
				for (AstNode* node = parseElement(); node; node = parseElement()) {
					out->mChilds.pushBack(node);
				}
				if (!out->mChilds.length()) {
					genError("Expected A Expression");
				}
				if (mError.descr) {
					delete out;
					return NULL;
				}
				return out;
			}

			AstNode* parseElement() {
				AstNode* out = NULL;
				switch (readTok().type) {
					case TOK_COMPOUND_START: out = parseCompound(); break;
					case TOK_CLASS_START: out = parseClass(); break;
					case TOK_CLASS_START_EXCLUDE: out = parseClass(true); break;
					case TOK_ANY: out = parseAny(); break;
					case TOK_VAL: out = parseVal(); break;
					case TOK_NONE: { discardTok(); return NULL; };
				}
				if (!out) {
					discardTok();
					return NULL;
				}
				switch (readTok().type) {
					case TOK_OR: out = parseAlternation(out); break;
					case TOK_REPEAT: out = parseRepetition(out); break;
					case TOK_REPEAT_PLUS: out = parseRepetition(out, true); break;
					case TOK_IF: out = parseIf(out); break;
					case TOK_NONE: break;
					default: { discardTok(); }
				}
				return out;
			}

			AstCompound* parseCompound() {
				auto out = new AstCompound();
				for (AstNode* node = parseElement(); node; node = parseElement()) {
					out->mChilds.pushBack(node);
				}
				if (readTok().type != TOK_COMPOUND_END) {
					genError("Expected Compound End");
				}
				if (mError.descr) {
					delete out;
					return NULL;
				}
				return out;
			}

			AstClass<tAlphabetType>* parseClass(bool exclude = false) {
				auto out = new AstClass<tAlphabetType>();
				out->mExclude = exclude;
				auto& ranges = out->mRanges;

				readTok();

			READ_VAL:
				if (mCurToken.type != TOK_VAL) {
					delete out;
					genError("Expected A Value");
					return NULL;
				}
				char range_start = mCurToken.val;

				readTok();
				if (mCurToken.type != TOK_HYPHEN) {
					delete out;
					genError("Expected A Range");
					return NULL;
				}

				readTok();
				if (mCurToken.type != TOK_VAL) {
					delete out;
					genError("Expected A Value");
					return NULL;
				}
				char range_end = mCurToken.val;

				ranges.pushBack({ range_start, range_end });

				readTok();
				if ((mCurToken.type == TOK_CLASS_END && !exclude) || (mCurToken.type == TOK_CLASS_END_EXCLUDE && exclude)) {
					return out;
				}
				else {
					goto READ_VAL;
				}
			}

			AstAny* parseAny() {
				return new AstAny();
			}

			AstVal<tAlphabetType>* parseVal() {
				auto out = new AstVal<tAlphabetType>(mCurToken.val);
				return out;
			}

			AstAlternation* parseAlternation(AstNode* left) {
				auto right = parseElement();
				if (!right) {
					genError("Expected Alternation right Side");
					delete left;
					return NULL;
				}

				auto out = new AstAlternation();

				out->mFirst = left;
				out->mSecond = right;

				return out;
			}

			AstRepetition* parseRepetition(AstNode* left, bool plus = false) {
				auto out = new AstRepetition();
				out->mNode = left;
				out->mPlus = plus;
				return out;
			}

			AstIf* parseIf(AstNode* left) {
				auto out = new AstIf();
				out->mNode = left;
				return out;
			}

			void genError(const char* desc) {
				mError = { desc, mOffset };
			}

			Token& readTok() {

				const tAlphabetType* crs = mSorce + mOffset;

				// zero termination string
				if (*crs == 0) {
					mCurToken.type = TOK_NONE;
					return mCurToken;
				}

				mTokLength = 1;
				mCurToken.type = TOK_VAL;
				mCurToken.val = crs[0];

				if (crs[0] == mEscapeSymbol) {
					mCurToken.val = crs[1];
					mTokLength = 2;
				}
				else {
					for (uhalni tok = 0; tok < TOK_SPECIALS_END__; tok++) {
						if (SpetialSymbols[tok] == mCurToken.val) {
							mCurToken.type = TokType(tok);
							break;
						}
					}
				}

				mOffset += mTokLength;
				return mCurToken;
			}

			void discardTok() {
				mOffset -= mTokLength;
				mTokLength = NULL;
			}
		};
		
		template <typename tStateType>
		struct CmpileError {
			ParseError mParseError;
			uhalni mRulleIndex = NULL;
			tStateType mRulleState;
			const char* descr = NULL;
			bool isError() const { return descr; }
		};

		template <typename tAlphabetType, typename tStateType, typename tStateType tNoStateVal, typename tStateType tFailedStateVal>
		class Compiler {

			typedef NFA<tAlphabetType, tStateType, tNoStateVal, tFailedStateVal> Graph;
			typedef typename Graph::Vertex Vertex;
			typedef Parser<tAlphabetType, tStateType, tNoStateVal> Parser;

			struct Node {
				Vertex* left = NULL;
				Vertex* right = NULL;
			};

		private:
			Graph* mGraph = NULL;

		public:

			CmpileError<tStateType> mError;

			Node compile(Graph& graph, const tAlphabetType* regex, tStateType state) {
				mGraph = &graph;
				return compileUtil(regex, state);
			}

			Node compile(Graph& graph, init_list<Tuple<const tAlphabetType*, tStateType>> rulles) {
				mGraph = &graph;

				auto left = mGraph->addVertex();
				auto right = mGraph->addVertex();

				halni idx = 0;
				for (auto rulle : rulles) {

					auto node = idx ? compileUtil(rulle.head, rulle.tail) : compileUtil(rulle.head, rulle.tail, left, right);

					if (!(node.left && node.right)) {
						mError.mRulleIndex = idx;
						return {};
					}

					if (idx) {
						transitionAny(left, node.left);
						transitionAny(node.right, right);
					}

					idx++;
				}

				mGraph->setStartVertex(left);

				return { left, right };
			}

		private:

			Node compileUtil(const tAlphabetType* regex, tStateType state, Vertex* aLeft = NULL, Vertex* aRight = NULL) {
				Parser parser;
				auto astnode = parser.parse(regex);
				if (parser.mError.isError()) {
					mGraph->setStartVertex(NULL);
					mError.descr = "Parsing Of Regular Expression Failed";
					mError.mRulleState = state;
					mError.mParseError = parser.mError;
					return {};
				}

				auto node = compileNode(astnode, aLeft, aRight);
				delete astnode;

				mGraph->setVertexState(node.right, state);
				mGraph->setStartVertex(node.left);

				return node;
			}

			Node compileVal(AstVal<tAlphabetType>* val, Vertex* aLeft = NULL, Vertex* aRight = NULL) {
				auto left = aLeft ? aLeft : mGraph->addVertex();
				auto right = aRight ? aRight : mGraph->addVertex();
				transitionVal(left, right, val->mVal);
				return { left, right };
			}

			Node compileAlternation(AstAlternation* alt, Vertex* aLeft = NULL, Vertex* aRight = NULL) {
				auto first_node = compileNode(alt->mFirst, aLeft, aRight);
				auto second_node = compileNode(alt->mSecond);
				transitionAny(first_node.left, second_node.left);
				transitionAny(second_node.right, first_node.right);
				return first_node;
			}

			Node compileAny(AstAny*, Vertex* aLeft = NULL, Vertex* aRight = NULL) {
				auto left = aLeft ? aLeft : mGraph->addVertex();
				auto right = aRight ? aRight : mGraph->addVertex();
				transitionAny(left, right, true);
				return { left, right };
			}

			Node compileRepeat(AstRepetition* repeat, Vertex* aLeft = NULL, Vertex* aRight = NULL) {
				if (repeat->mPlus) {
					auto middle = mGraph->addVertex();

					auto left_node = compileNode(repeat->mNode, aLeft, middle);

					auto right_node = compileNode(repeat->mNode, middle, aRight);
					transitionAny(right_node.right, right_node.left);
					transitionAny(right_node.left, right_node.right);

					return { left_node.left, right_node.right };
				}
				else {
					auto node = compileNode(repeat->mNode, aLeft, aRight);
					transitionAny(node.right, node.left);
					transitionAny(node.left, node.right);
					return node;
				}
			}

			Node compileIf(AstIf* ifnode, Vertex* aLeft = NULL, Vertex* aRight = NULL) {
				auto node = compileNode(ifnode->mNode, aLeft, aRight);
				transitionAny(node.left, node.right);
				return node;
			}

			Node compileClass(AstClass<tAlphabetType>* node, Vertex* aLeft = NULL, Vertex* aRight = NULL) {
				auto left = aLeft ? aLeft : mGraph->addVertex();
				auto right = aRight ? aRight : mGraph->addVertex();
				
				if (node->mRanges.length() == 1) {
					auto const& range = node->mRanges.first()->data;
					transitionRange(left, right, { range.mBegin, range.mEnd }, node->mExclude);
					return { left, right };
				}

				for (auto range : node->mRanges) {
					auto middle = mGraph->addVertex();
					transitionRange(left, middle, { range.data().mBegin, range.data().mEnd }, node->mExclude);
					transitionAny(middle, right);
				}
				return { left, right };
			}

			Node compileCompound(AstCompound* compound, Vertex* aLeft = NULL, Vertex* aRight = NULL) {
				Vertex* left = NULL;
				Vertex* rigth = NULL;

				for (auto child : compound->mChilds) {
					
					auto pass_left = child.idx() == 0 ? aLeft : rigth;
					auto pass_right = child.idx() == compound->mChilds.length() - 1 ? aRight : NULL;
					auto node = compileNode(child.data(), pass_left, pass_right);

					if (!left) {
						left = node.left;
					}
					rigth = node.right;
				}

				return { left, rigth };
			}

			Node compileNode(AstNode* node, Vertex* aLeft = NULL, Vertex* aRight = NULL) {
				switch (node->mType) {
					case AstNode::CLASS: return compileClass((AstClass<tAlphabetType>*)node, aLeft, aRight);
					case AstNode::COMPOUND: return compileCompound((AstCompound*)node, aLeft, aRight);
					case AstNode::IF: return compileIf((AstIf*)node, aLeft, aRight);
					case AstNode::REPEAT: return compileRepeat((AstRepetition*)node, aLeft, aRight);
					case AstNode::ANY: return compileAny((AstAny*)node, aLeft, aRight);
					case AstNode::OR: return compileAlternation((AstAlternation*)node, aLeft, aRight);
					case AstNode::VAL: return compileVal((AstVal<tAlphabetType>*)node, aLeft, aRight);
				}
				assert(0);
				return {};
			}

			void transitionAny(Vertex* from, Vertex* to, bool consumes = false) {
				mGraph->addTransition(from, to, {}, consumes, true, false);
			}

			void transitionVal(Vertex* from, Vertex* to, tAlphabetType val) {
				mGraph->addTransition(from, to, { val, val }, true, false, false);
			}

			void transitionRange(Vertex* from, Vertex* to, Range<tAlphabetType> range, bool exclude) {
				mGraph->addTransition(from, to, range, true, false, exclude);
			}
		};

		template <typename tAlphabetType, typename tStateType, typename tStateType tNoStateVal, typename tStateType tFailedStateVal>
		CmpileError<tStateType>	compile(NFA<tAlphabetType, tStateType, tNoStateVal, tFailedStateVal>& out, const tAlphabetType* regex, tStateType state) {
			Compiler<tAlphabetType, tStateType, tNoStateVal, tFailedStateVal> compiler;
			compiler.compile(out, regex, state);
			return compiler.mError;
		}

		template <typename tAlphabetType, typename tStateType, typename tStateType tNoStateVal, typename tStateType tFailedStateVal>
		CmpileError<tStateType>	compile(NFA<tAlphabetType, tStateType, tNoStateVal, tFailedStateVal>& out, const init_list<Tuple<const tAlphabetType*, tStateType>>& rulles) {
			Compiler<tAlphabetType, tStateType, tNoStateVal, tFailedStateVal> compiler;
			compiler.compile(out, rulles);
			return compiler.mError;
		}
	};
};