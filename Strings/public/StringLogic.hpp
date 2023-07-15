#pragma once

#include "Utils.hpp"

namespace tp {

	template<typename tChar = uint1>
	class StringLogic {
	public:

		typedef halni Index;
		typedef Index LineId;
		typedef Index ColumnId;

		static inline bool isNewLineChar(tChar in) { return (in == '\n') || (in == '\r'); }
		static inline bool isEndChar(tChar in) { return in == '\0'; }
		static constexpr tChar getEndChar() { return '\0'; }

		static ualni calcLength(const tChar* in) {
			const tChar* iter = in;
			while (*iter != '\0') iter++;
			return iter - in;
		}

		static tChar* insertLogic(const tChar* cur, const tChar* tar, ualni at, ualni len) {
			ualni curLen = calcLength(cur);
			ualni allLen = curLen + len;
			auto* out = new tChar[allLen + 1];

			DEBUG_ASSERT(curLen >= 0)
			DEBUG_ASSERT(len >= 0)
			DEBUG_ASSERT(at < curLen + 1 && at >= 0)

			for (ualni idx = 0; idx < at; idx++) {
				out[idx] = cur[idx];
			}
			for (ualni idx = 0; idx < len; idx++) {
				out[idx + at] = tar[idx];
			}
			for (ualni idx = at + len; idx < allLen; idx++) {
				out[idx] = cur[idx - len];
			}
			out[allLen] = '\0';
			return out;
		}

		static tChar* removeLogic(const tChar* cur, ualni start, ualni end) {
			ualni curLen = calcLength(cur);
			ualni rangeLen = end - start;
			ualni newLen = curLen - rangeLen;
			auto* out = new tChar[newLen + 1];
			out[newLen] = '\0';
			for (ualni idx = 0; idx < start; idx++) {
				out[idx] = cur[idx];
			}
			for (ualni idx = end; idx < curLen; idx++) {
				out[idx - rangeLen] = cur[idx];
			}
			return out;
		}

		static bool isEqualLogic(const tChar* left, const tChar* right) {
			ualni idx = 0;
		LOOP:
			if (left[idx] == '\0' || right[idx] == '\0') {
				if (left[idx] == '\0' && right[idx] == '\0') {
					return true;
				}
				return false;
			}
			if (left[idx] != right[idx]) {
				return false;
			}
			idx++;
			goto LOOP;
		}

		static ualni fromValueLength(alni val, ualni base) {
			ualni iter = val;
			ualni len = 0;
			while (iter /= base) {
				len++;
			}
			bool neg = val < 0;
			len += 1 + ualni(neg);
			return len;
		}

		static tChar* stringFromValue(alni val, tChar* ownBuff, ualni base) {
			tChar* out;
			bool neg = val < 0;
			ualni len = fromValueLength(val, base);
			out = ownBuff ? ownBuff : new tChar[len + 1];
			out[len] = '\0';
			val = abs(val);
			for (ualni i = len - 1; i >= int(neg); i--) {
				out[i] = (tChar) (val % base + 48);
				val /= (alni) base;
			}
			if (neg) {
				out[0] = '-';
			}
			return out;
		}

		static ualni fromValueLength(alnf val, ualni base) {
			auto rounded = (alni) val;
			ualni mantissa = ( (alni) val - rounded) * 100000;
			alni rounded_len = 0;
			alni mantissa_len = 0;
			while (rounded /= (alni) base) {
				rounded_len++;
			}
			if (!rounded_len) {
				rounded_len++;
			}
			while (mantissa /= base) {
				mantissa_len++;
			}
			bool neg = val < 0;
			bool dot = mantissa_len & 1;
			alni tot_len = mantissa_len + rounded_len + dot + neg;
			return tot_len;
		}

		static tChar* fromValue(alnf, ualni, tChar*) {
			DEBUG_BREAK(false)
			return nullptr;
		}

		static ualni fromValueLength(bool val) {
			return val ? 4 : 5;
		}

		static tChar* fromValue(bool val, tChar* ownBuff) {
			alni len = val ? 4 : 5;
			tChar* out = ownBuff ? ownBuff : new tChar[len + 1];
			out[len] = '\0';
			memCopy(out, val ? "True" : "False", len);
			return out;
		}

		static tChar* stringFromValue(int val, ualni base) {
			return fromValue((alni) val, nullptr, base);
		}

		static tChar* fromValue(tChar val) {
			auto* out = new tChar[2];
			out[1] = '\0';
			out[0] = val;
			return out;
		}

		static bool toValue(const tChar*, alni&, ualni) {
			DEBUG_BREAK(false)
			return false;
		}

		static bool toValue(const tChar*, alnf&, ualni) {
			DEBUG_BREAK(false)
			return false;
		}

		static bool toValue(const tChar* in, bool& val) {
			if (!calcLength(in)) return false;
			if (isEqual(in, "False") || isEqual(in, "false") || isEqual(in, "0")) {
				val = false;
			} else {
				val = true;
			}
			return true;
		}

		static void calcLineOffsets(const tChar* aBuff, Index aLength, Buffer<Index>& aOut) {
			halni lines = 0;
			for (auto idx : Range(aLength)) {
				if (isNewLineChar(aBuff[idx])) {
					lines++;
				}
			}
			aOut.reserve(lines + 2); // plus start and end offsets
			lines = 0;
			for (auto idx : Range(aLength)) {
				if (isNewLineChar(aBuff[idx])) {
					aOut[lines + 1] = Index(idx + 1);
					lines++;
				}
			}
			aOut[0] = 0;
			aOut[lines + 1] = aLength;
		}
	};
}