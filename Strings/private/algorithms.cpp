
#include "algorithms.h"

#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdexcept>

namespace tp {

	alni slen(const char* in) {
		alni out = 0;
		for (const char* iter = in; *iter != '\0'; iter++) {
			out++;
		}
		return out;
	}

	char* sinsert(const char* cur, const char* tar, alni atidx, alni len) {
		alni cur_len = slen(cur);
		alni all_len = cur_len + len;
		char* out = new char[all_len + 1];

		assert(cur_len >= 0);
		assert(len >= 0);
		assert(atidx < cur_len + 1 && atidx >= 0);

		for (alni idx = 0; idx < atidx; idx++) {
			out[idx] = cur[idx];
		}

		for (alni idx = 0; idx < len; idx++) {
			out[idx + atidx] = tar[idx];
		}

		for (alni idx = atidx + len; idx < all_len; idx++) {
			out[idx] = cur[idx - len];
		}

		out[all_len] = '\0';

		return out;
	}

	void soverride(char* cur, const char* tar, alni atidx, alni len) {
		for (alni idx = 0; idx < len; idx++) {
			cur[idx + atidx] = tar[idx];
		}
	}

	char* sremove(const char* cur, alni start, alni end) {
		alni cur_len = slen(cur);
		alni range_len = end - start;
		alni new_len = cur_len - range_len;
		char* out = new char[new_len + 1];
		out[new_len] = '\0';

		for (alni idx = 0; idx < start; idx++) {
			out[idx] = cur[idx];
		}

		for (alni idx = end; idx < cur_len; idx++) {
			out[idx - range_len] = cur[idx];
		}

		return out;
	}

	alni sequal(const char* left, const char* right) {
		alni idx = 0;

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

		return false;
	}

	alni val2str_len(alni val, alni base) {
		alni iter = val;
		alni len = 0;
		while (iter /= base) {
			len++;
		}
		bool neg = val < 0;
		len += 1 + alni(neg);
		return len;
	}

	char* val2str(alni val, char* ownbuff, alni base) {
		char* out = NULL;
		bool neg = val < 0;
		alni len = val2str_len(val, base);

		out = ownbuff ? ownbuff : new char[len + 1];
		out[len] = '\0';

		val = ABS(val);

		for (alni i = len - 1; i >= int(neg); i--) {
			out[i] = (char) (val % base + 48);
			val /= base;
		}

		if (neg) {
			out[0] = '-';
		}

		return out;
	}

	alni val2str_len(alnf val, alni base) {
		alni rounded = (alni) val;
		alni mantissa = (alni) (val - rounded) * 100000;

		alni rounded_len = 0;
		alni mantissa_len = 0;

		while (rounded /= base) {
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

	char* val2str(alnf val, alni base, char* ownbuff) {
		auto str = std::to_string(val);
		auto len = slen(str.c_str()) + 1;
		char* out = ownbuff ? ownbuff : new char[len];
		memcp(out, str.c_str(), len);
		return out;
	}

	alni val2str_len(bool val) { return val ? 4 : 5; }

	char* val2str(bool val, char* ownbuff) {
		alni len = val ? 4 : 5;
		char* out = ownbuff ? ownbuff : new char[len + 1];
		out[len] = '\0';

		memcp(out, val ? "True" : "False", len);

		return out;
	}

	char* val2str(int val, alni base) { return val2str((alni) val, NULL, base); }

	char* val2str(char val) {
		char* out = new char[2];
		out[1] = '\0';
		out[0] = val;
		return out;
	}

	bool str2val(const char* in, alni& val, alni base) {
#ifdef ENV_OS_ANDROID
		val = 0;
		val = std::stol(in);
		return true;
#else 
		try {
			val = std::stol(in);
			return true;
		}
		catch (...) {
			return false;
		}
#endif 
	}

	bool str2val(const char* in, alnf& val, alni base) {
#ifdef ENV_OS_ANDROID
		val = 0;
		val = std::stod(in);
		return true;
#else 
		try {
			val = std::stod(in);
			return true;
		}
		catch (...) {
			return false;
		}
#endif 
	}

	bool str2val(const char* in, bool& val) {
		alni length = slen(in);

		if (length == 0) {
			return false;
		}

		if (sequal(in, "False") || sequal(in, "false") || sequal(in, "0")) {
			val = 0;
		} else {
			val = 1;
		}

		return true;
	}

};