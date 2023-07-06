
#include "log.h"

#include "texteditor.h"

#include <stdio.h>

namespace tp {

	Logger* GLog = NULL;

	tp::str::Index Logger::LogEntry::nLines() {
		return (tp::str::Index)(mLineOffsets.length() - 1);
	}

	void Logger::LogEntry::calc_nlines() {
		tp::str::Input input = { mText.cstr(), (tp::str::Index) mText.size() };
		input.getLineOfssets(mLineOffsets);
	}

	Logger::LogEntry::LogEntry() { 
		mText = " - "; 
	}

	Logger::LogEntry::LogEntry(tp::string text) : mText(text) {
		calc_nlines();
	}

	Logger::LogEntry::LogEntry(tp::string text, Type type) : mType(type), mText(text) {
		calc_nlines();
	}

	void Logger::write(const tp::string& in, bool post, LogEntry::Type type) {
		tp::string copy = in;
		copy.capture();
		buff.pushBack(LogEntry(copy, type));

		mTotalLines += buff.last()->data.nLines();

		if (!cursor) {
			cursor = buff.last();
		}

		#ifdef LOG_TRANSFER_TO_STD
		if (post) {
			printf(in.cstr());
		}
		#endif
	}

	string Logger::read() {
		if (cursor) {
			const LogEntry& out = cursor->data;
			cursor = cursor->next;
			return out.mText;
		}
		return string();
	}

	alni Logger::sizeAllocatedMem() {
		alni out = sizeof(ListNode<string>*); // cursor
		out += buff.sizeAllocatedMem();
		return out;
	}

	alni Logger::sizeUsedMem() {
		alni out = sizeof(ListNode<string>*); // cursor
		out += buff.sizeUsedMem();
		return out;
	}

	void Logger::init() {
		assert(!GLog);
		GLog = new Logger;
	}

	void Logger::deinit() {
		assert(GLog);
		delete GLog;
	}
};