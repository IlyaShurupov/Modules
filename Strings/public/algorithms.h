#pragma once

#include "common.h"

namespace tp {

	alni slen(const char* in);
	char* sinsert(const char* cur, const char* tar, alni atidx, alni len);
	void soverride(char* cur, const char* tar, alni atidx, alni len);
	char* sremove(const char* cur, alni start, alni end);
	alni sequal(const char* left, const char* right);

	char* val2str(int val, alni base = 10);
	char* val2str(alni val, char* ownbuff = NULL, alni base = 10);
	char* val2str(alnf val, alni base = 10, char* ownbuff = NULL);
	char* val2str(bool val, char* ownbuff = NULL);
	char* val2str(char val);

	bool str2val(const char* in, alni& val, alni base = 10);
	bool str2val(const char* in, alnf& val, alni base = 10);
	bool str2val(const char* in, bool& val);

	alni val2str_len(alni val, alni base);
	alni val2str_len(alnf val, alni base);
	alni val2str_len(bool val);

	alni how_many(const char* str, const char val);
	alni sfind(const char target, alni target_len, alni start, alni end, bool reversed);

};