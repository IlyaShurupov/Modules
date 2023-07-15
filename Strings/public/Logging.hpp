
#pragma once

#include "Buffer.hpp"
#include "List.hpp"
#include "Strings.hpp"

namespace tp {

	class Logger {
	public:

		class Report {
			String mData;
			Buffer<String::Index> mLineOffsets;

		public:
			enum Type { INFO, ERR, WARN, SUC } mType = INFO;

		public:
			Report();
			explicit Report(const String& text);
			Report(const String& text, Type type);

		public:
			[[nodiscard]] ualni getLineCount() const;
			[[nodiscard]] String getString() const;

		private:
			void calcLineCount();
		};

	private:
		List<Report>::Node* mCursor = nullptr;
		List<Report> mBuff;
		ualni mLineCount = 0;

	public:
		void write(const String& in, bool post = true, Report::Type type = Report::Type::INFO);

		[[nodiscard]] String read();
		[[nodiscard]] const List<Report>& getBuff() const { return mBuff; }
		[[nodiscard]] ualni getLineCount() const { return mLineCount; }

	public:
		static void initializeGlobal();
		static void deinitializeGlobal();
	};

	extern Logger* gLogger;
}

#define LOG(val) tp::str::gLogger->write((val), 1)

#ifdef ENV_BUILD_DEBUG
#define DEBUG_LOG(val) tp::str::gLogger->write((val), 1)
#else
#define DEBUG_LOG(val) (0)
#endif