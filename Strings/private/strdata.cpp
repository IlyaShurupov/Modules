
#include "strdata.h"

#include "allocators.h"

#include "algorithms.h"

#include "texteditor.h"

//static tp::PoolAlloc* alloc = NULL;

namespace tp {

	EditedStringsTable* gEditedStringsTable = NULL;

	EditedStringsTable::EditedStringsTable() : mTable(sizeof(str::TextEditor*), MAX_EDITED_STRINGS) {
		mTableStart = (str::TextEditor**) mTable.Alloc(sizeof(str::TextEditor*));
		auto end = (str::TextEditor**) mTable.Alloc(sizeof(str::TextEditor*));
		mBlockSize = (uint2)(((tp::uint1*)end) - ((tp::uint1*)mTableStart));
	}
	
	str::TextEditor* EditedStringsTable::get(tp::uint2 idx) {
		assert(this && "Forgot to Initialize Strings?");
		auto out = (str::TextEditor**)( ((tp::uint1*) mTableStart) + (idx) * (mBlockSize));
		return *out;
	}
	
	tp::uint2 EditedStringsTable::alloc(const char* orig_buff) {
		auto entry = (str::TextEditor**) mTable.Alloc(sizeof(str::TextEditor*));
		
		*entry = new str::TextEditor(tp::str::Input{ orig_buff, tp::halni(slen(orig_buff)) });

		auto diff = tp::halni(((tp::uint1*)entry) - ((tp::uint1*)mTableStart));
		auto out = tp::uint2(diff / (mBlockSize));

		return out;
	}
	
	void EditedStringsTable::dealloc(tp::uint2 idx) {
		auto entry = (str::TextEditor**)(((tp::uint1*)mTableStart) + (idx) * (mBlockSize));
		delete *entry;
		mTable.Free(entry);
	}

	str_data::str_data(const char* p_buff, bool p_ref) {

		if (p_buff) {
			buff = (char*) p_buff;
		} else {
			buff = (char*) " ";
			p_ref = true;
		}

		refc = 0;

		is_const = p_ref;
		edited_idx = 0;
	}

	str_data::str_data(const str_data& in) {
		buff = in.buff;
		is_const = true;
		refc = 0;
		edited_idx = 0;
	}

	str_data::~str_data() {
		if (!is_const) {
			delete[] buff;
		}
		if (edited_idx != 0) {
			gEditedStringsTable->dealloc(edited_idx);
		}
	}

	void* str_data::operator new(size_t size) {
		return pickalloc->Alloc(sizeof(tp::str_data));
	}

	void str_data::release() {
		if (buff && !is_const) {
			delete[] buff;
		}
	}

	void str_data::reserve(alni len) {
		release();
		buff = new char[len + 1];
		buff[len] = '\0';
		is_const = false;
	}

	void str_data::createEdited() {
		assert(edited_idx == 0);
		edited_idx = gEditedStringsTable->alloc(buff);
	}

	str::TextEditor* str_data::getEdited() {
		if (edited_idx == 0) {
			return NULL;
		}

		return gEditedStringsTable->get(edited_idx);
	}

	void str_data::saveEdited() {
		if (edited_idx == 0) {
			return;
		}

		auto edited = gEditedStringsTable->get(edited_idx);
		edited->mOrigBuff.makeOwnOriginalInput();

		auto len = 0;
		for (const auto& character : *edited) {
			len++;
		}
		
		auto old_buff = buff;
		
		buff = new char[len + 1];
		buff[len] = '\0';

		auto idx = 0;
		for (auto character : *edited) {
			buff[idx] = character.character();
			idx++;
		}

		if (old_buff && !is_const) {
			delete[] old_buff;
		}

		is_const = false;
	}

	void str_data::clearEdited() {
		if (edited_idx == 0) {
			return;
		}

		gEditedStringsTable->dealloc(edited_idx);
		edited_idx = 0;
	}

	void str_data::clear() {
		if (buff && !is_const) {
			delete[] buff;
		}
		is_const = true;
		buff = (char*) " ";
	}

	void str_data::insert(const char* in, alni atidx, alni len) {
		char* cur = buff;
		buff = sinsert(buff, in, atidx, len);
		delete[] cur;
	}

	void str_data::remove(alni start, alni end) {
		char* cur = buff;
		buff = sremove(buff, start, end);
		delete[] cur;
	}

	void str_data::override(const char* in, alni atidx, alni len) {
		soverride(buff, in, atidx, len);
	}

};