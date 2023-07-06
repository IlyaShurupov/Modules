
#pragma once

#include "RegularExpression.h"
#include "stringt.h"

namespace tp {

	extern ModuleManifest gModuleTokenizer;

	template <typename tAlphabetType, typename tTokType, typename tTokType tNoTokVal, typename tTokType tFailedTokVal>
	class Tokenizer {

		TransitionMatrix<tAlphabetType, tTokType, tNoTokVal, tFailedTokVal> mTransitionMatrix;

		RegEx::CmpileError<tTokType> mError;

		bool scanFailed() {
			return mTransitionMatrix.isTraped();
		}

	public:

		Tokenizer() {
			MODULE_SANITY_CHECK(gModuleTokenizer);
		}

		void build(const init_list<Tuple<const tAlphabetType*, tTokType>>& rulles) {
			NFA<tAlphabetType, tTokType, tNoTokVal, tFailedTokVal> nfa;
			
			mError = RegEx::compile(nfa, rulles);
			if (mError.isError()) {
				return;
			}

			DFA<tAlphabetType, tTokType, tNoTokVal, tFailedTokVal> dfa(nfa);
			mTransitionMatrix.construct(dfa);
		}

		bool isBuilded() const {
			return !mError.isError();
		}

		const RegEx::CmpileError<tTokType>& getBuildError() {
			return mError;
		}

		void resetMatrix() {
			mTransitionMatrix.reset();
		}

		tTokType advanceSymbol(tAlphabetType symbol) {
			return mTransitionMatrix.move(symbol);
		}

		tTokType advanceToken(const tAlphabetType* source, ualni source_len, ualni* token_len) {
			tTokType out = tNoTokVal;
			*token_len = 0;
			for (ualni idx = 0; idx < source_len; idx++) {
				out = advanceSymbol(source[idx]);
				if (out != tNoTokVal) {
					*token_len = idx;
					break;
				}
			}
			return out;
		}

		~Tokenizer() {}
	};

	template <typename tAlphabetType, typename tTokType, typename tTokType tNoTokVal, typename tTokType tFailedTokVal, typename tTokType tSourceEndTokVal>
	class SimpleTokenizer {
		
		Tokenizer<tAlphabetType, tTokType, tNoTokVal, tFailedTokVal> mTokenizer;

		const tAlphabetType* mSource = NULL;
		ualni mLastTokLen = 0;
		ualni mSourceLen = 0;
		ualni mAdvancedOffset = 0;

	public:

		struct Cursor {
			const tAlphabetType* mSource = NULL;
			ualni mAdvancedOffset = 0;
			const tAlphabetType* str() { return mSource + mAdvancedOffset; }
		};

		SimpleTokenizer() {}

		void build(const init_list<Tuple<const tAlphabetType*, tTokType>>& rulles) {
			mTokenizer.build(rulles);
		}

		bool isBuilded() const {
			return mTokenizer.isBuilded();
		}

		const RegEx::CmpileError<tTokType>& getBuildError() {
			return mTokenizer.getBuildError();
		}

		void bindSource(const tAlphabetType* source) {
			mSource = source;
			while (mSource[mSourceLen]) mSourceLen++;
			mSourceLen++;
		}

		bool inputLeft() const {
			if (!mSource) {
				return false;
			}
			if (mSource[mAdvancedOffset] == NULL) {
				return false;
			}
			return true;
		}

		Cursor getCursor() const {
			return { mSource, mAdvancedOffset };
		}

		Cursor getCursorPrev() const {
			return { mSource, mAdvancedOffset - mLastTokLen };
		}

		void setCursor(const Cursor& crs) {
			mAdvancedOffset = crs.mAdvancedOffset;
			mLastTokLen = 0;
		}

		tTokType readTok() {
			if (mSourceLen == mAdvancedOffset + 1) {
				return tSourceEndTokVal;
			}

			tTokType out = mTokenizer.advanceToken(mSource + mAdvancedOffset, mSourceLen - mAdvancedOffset, &mLastTokLen);
			mAdvancedOffset += mLastTokLen;
			return out;
		}

		tTokType lookupTok() {
			auto out = readTok();
			discardTok();
			return out;
		}

		void discardTok() {
			mTokenizer.resetMatrix();
			mAdvancedOffset -= mLastTokLen;
			mLastTokLen = 0;
		}

		void skipTok() {
			readTok();
		}

		void reset() {
			mAdvancedOffset = mLastTokLen = 0;
			mTokenizer.resetMatrix();
		}

		ualni lastTokLEn() const {
			return mLastTokLen;
		}

		string extractVal() {
			auto crs = getCursorPrev();
			string out;
			out.reserve(mLastTokLen + 1);
			tp::memcp(out.get_writable(), crs.str(), mLastTokLen);
			out.get_writable()[mLastTokLen] = '\0';
			return out;
		}
	};

};