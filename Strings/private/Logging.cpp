
#include "Logging.hpp"
#include "Allocators.hpp"

#include <cstdio>

namespace tp {

	Logger* gLogger = nullptr;

	ualni Logger::Report::getLineCount() const {
		return (ualni)(mLineOffsets.size() - 1);
	}

	String Logger::Report::getString() const {
		return mData;
	}

	void Logger::Report::calcLineCount() {
		mData.calcLineOffsets(mLineOffsets);
	}

	Logger::Report::Report() {
		mData = " - ";
	}

	Logger::Report::Report(const String& text) : mData(text) {
		calcLineCount();
	}

	Logger::Report::Report(const String& text, Type type) : mType(type), mData(text) {
		calcLineCount();
	}

	void Logger::write(const String& in, bool post, Report::Type type) {
		mBuff.pushBack(Report(in, type));
		mLineCount += mBuff.last()->data.getLineCount();
		if (!mCursor) mCursor = mBuff.last();
		if (post) printf("%s", in.read());
	}

	String Logger::read() {
		if (!mCursor) return {};
    const Report& out = mCursor->data;
    mCursor = mCursor->next;
    return out.getString();
	}

	void Logger::initializeGlobal() {
		DEBUG_ASSERT(!gLogger)
		gLogger = new Logger();
	}

	void Logger::deinitializeGlobal() {
		DEBUG_ASSERT(gLogger)
		delete gLogger;
	}
}