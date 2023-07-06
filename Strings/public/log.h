
#pragma once

#include "list.h"
#include "array.h"
#include "stringt.h"
#include "texteditor.h"

#define LOG_TRANSFER_TO_STD

namespace tp {

	class Logger {
		
	public:

		struct LogEntry {

			enum Type {
				INFO,
				ERR, 
				WARN,
				SUC
			} mType = INFO;

			tp::string mText;
			tp::Array<tp::str::Index> mLineOffsets;

			LogEntry();
			LogEntry(tp::string text);
			LogEntry(tp::string text, Type type);

			tp::str::Index nLines();

		private:
			void calc_nlines();
		};

	private:

		ListNode<LogEntry>* cursor = nullptr;
		List<LogEntry> buff;
		tp::halni mTotalLines = 0;

	public:

		#ifdef ENV_BUILD_DEBUG 
		void write(const tp::string& in, bool post = true, LogEntry::Type type = LogEntry::Type::INFO);
		#else
		void write(const tp::string& in, bool post = false, LogEntry::Type type = LogEntry::Type::INFO);
		#endif

		string read();

		alni sizeAllocatedMem();
		alni sizeUsedMem();

		const List<LogEntry>& getBuff() { return buff; }
		tp::str::Index nTotalLines() { return mTotalLines; }

		static void init();
		static void deinit();
	};

	extern tp::Logger* GLog;

#ifdef ENV_BUILD_DEBUG
	#define DBGLOG(val) GLog->write((val), 1)
	#else
	#define DBGLOG(val) (0)
	#endif // DEBUG

	#define LOG(val) GLog->write((val), 1)
};