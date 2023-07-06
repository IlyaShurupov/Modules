#pragma once

#include "chunkalloc.h"

#if defined(ENV_OS_LINUX)
	#include <stddef.h>
#endif

namespace tp {

	namespace str {
		class TextEditor;
	};

	class EditedStringsTable {
		enum { MAX_EDITED_STRINGS = 65536 };
		tp::ChunkAlloc mTable;
		str::TextEditor** mTableStart;
		tp::uint2 mBlockSize;

		public:

		EditedStringsTable();
		str::TextEditor* get(tp::uint2 idx);
		tp::uint2 alloc(const char* orig_buff);
		void dealloc(tp::uint2 idx);
	};

	extern EditedStringsTable* gEditedStringsTable;

	class str_data {

		uint2 is_const; // source is non-modifiable
		uint2 edited_idx; // index in edited strings
		uint4 refc; // number of users of this string data
		char* buff; // actual string data

		friend class string;

		str_data(const char* p_buff, bool p_ref);
		str_data(const str_data& in);
		~str_data();

		void* operator new(size_t size);

		// control utils
		void release();
		void reserve(alni len);

		void createEdited();
		str::TextEditor* getEdited();
		void saveEdited();
		void clearEdited();

		// modification functionallity
		void clear();
		void insert(const char* in, alni atidx, alni len);
		void remove(alni start, alni end);
		void override(const char* in, alni atidx, alni len);
	};

};