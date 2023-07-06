
#include "allocators.h"

#include "CmdArgParser.h"

#include "log.h"

tp::CmdArgParser::Arg::Arg(const Arg& arg) {
	mId = arg.mType;
	switch (arg.mType) {
	case tp::CmdArgParser::Arg::INT: mType = INT;	mInt = arg.mInt; break;
	case tp::CmdArgParser::Arg::FLOAT: mType = FLOAT; mFloat = arg.mFloat; break;
	case tp::CmdArgParser::Arg::BOOL: mType = BOOL; mBool = arg.mBool; break;
	case tp::CmdArgParser::Arg::STR: mType = STR; new (&mStr) StringArg(); mStr = arg.mStr; break;
	case tp::CmdArgParser::Arg::FILE_IN: mType = FILE_IN;	new (&mFile) FileInputArg(); mFile = arg.mFile; break;
	}
	mOptional = arg.mOptional;
}

tp::CmdArgParser::Arg::Arg(tp::string id, Type type) {
	mId = id;
	switch (type) {
	case tp::CmdArgParser::Arg::INT: mType = INT;	break;
	case tp::CmdArgParser::Arg::FLOAT: mType = FLOAT; break;
	case tp::CmdArgParser::Arg::BOOL: mType = BOOL; break;
	case tp::CmdArgParser::Arg::STR: mType = STR; new (&mStr) StringArg(); break;
	case tp::CmdArgParser::Arg::FILE_IN: mType = FILE_IN;	new (&mFile) FileInputArg(); break;
	}
	mOptional = false;
}

tp::CmdArgParser::Arg::Arg(tp::string id, Range<alni> aAcceptingRange) {
	mId = id;
	mType = FLOAT;
	mInt = { 0, 0, aAcceptingRange };
	mOptional = false;
}

tp::CmdArgParser::Arg::Arg(tp::string id, Range<alnf> aAcceptingRange) {
	mId = id;
	mType = FLOAT;
	mFloat = { 0, 0, aAcceptingRange };
	mOptional = false;
}

tp::CmdArgParser::Arg::Arg(tp::string id, Range<alni> aAcceptingRange, alni aDefault) {
	mId = id;
	mType = INT;
	mInt = { 0, aDefault, aAcceptingRange };
}

tp::CmdArgParser::Arg::Arg(tp::string id, Range<alnf> aAcceptingRange, alnf aDefault) {
	mId = id;
	mType = FLOAT;
	mFloat = { 0, aDefault, aAcceptingRange };
}

tp::CmdArgParser::Arg::Arg(tp::string id, bool aDefault) {
	mId = id;
	mType = BOOL;
	mBool.mDefault = aDefault;
}

tp::CmdArgParser::Arg::Arg(tp::string id, const char* aDefault) {
	mId = id;
	mType = STR;
	new (&mStr) StringArg();
	mStr.mDefault = aDefault;
}

tp::CmdArgParser::Arg::~Arg() {
	switch (mType) {
	case tp::CmdArgParser::Arg::STR:
		mStr.~StringArg();
		break;
	case tp::CmdArgParser::Arg::FILE_IN:
		mFile.~FileInputArg();
		break;
	}
}

tp::CmdArgParser::CmdArgParser(tp::init_list<Arg> args) {
	bool optional_start = false;

	for (auto& arg : args) {
		assert(!mArgs.presents(arg.mId) && "Argument Redefinition");

		auto copy_arg = new Arg(arg);

		mArgs.put(arg.mId, copy_arg);
		mArgsOrder.pushBack(mArgs.get(arg.mId));

		if (arg.mOptional) {
			mOptionals++;
		}

		if (optional_start) {
			assert(arg.mOptional && "Not Optional Argument After Optionals");
		}
		else if (arg.mOptional) {
			optional_start = true;
		}
	}

	mTokenizer.build({
		{ "\n|\t| |\r", TokType::SPACE },
		{ "N|n|(False)|(false)", TokType::BOOL_FALSE },
		{ "Y|y|(True)|(true)", TokType::BOOL_TRUE },
		{ "((\\-)|(\\+))?[0-9]+", TokType::INT },
		{ "((\\-)|(\\+))?([0-9]+)(\\.)([0-9]*)?", TokType::FLOAT },
		{ "'{'-'}*'", TokType::STR },
		});

	assert(mTokenizer.isBuilded() && "Internal Error");
}

tp::CmdArgParser::~CmdArgParser() {
	for (auto arg : mArgsOrder) {
		delete arg.data();
	}
}

bool tp::CmdArgParser::parse(char argc, const char* argv[], bool logError) {
	// discard windows working directory argument
	argc--;

	if (argc < mArgs.size() - mOptionals) {
		ErrInvalidArgCount();
		if (logError) {
			ErrLog();
		}
		return false;
	}

	for (auto arg : mArgsOrder) {
		if (arg.idx() < argc) {
			parseArg(*arg.data(), argv[arg.idx() + 1]);
		}
		else {
			initDefault(*arg.data());
		}
		if (mError) {
			if (logError) {
				ErrLog();
			}
			return false;
		}
	}
	return true;

	//tp::set_working_dir();
}

tp::alni tp::CmdArgParser::getInt(string id) { auto& arg = getArg(id, Arg::INT); return arg.mInt.mVal; }
tp::alnf tp::CmdArgParser::getFloat(string id) { auto& arg = getArg(id, Arg::FLOAT); return arg.mFloat.mVal; }
bool tp::CmdArgParser::getBool(string id) { auto& arg = getArg(id, Arg::BOOL); return arg.mBool.mFlag; }
tp::string tp::CmdArgParser::getString(string id) { auto& arg = getArg(id, Arg::STR); return arg.mStr.mStr; }
tp::File& tp::CmdArgParser::getFile(string id) { auto& arg = getArg(id, Arg::FILE_IN); return arg.mFile.mFile; }


tp::CmdArgParser::Arg& tp::CmdArgParser::getArg(string id, Arg::Type type) {
	auto idx = mArgs.presents(id);
	assert(idx && "Invalid Id");
	auto& arg = mArgs.getSlotVal(idx);
	assert(arg->mType == type && "Invalid Type Requested");
	return *arg;
}

void tp::CmdArgParser::ErrInvalidArgCount() { mError = { "Invalid Number Of Arguments Passed", NULL }; }
void tp::CmdArgParser::ErrInvalidArgSyntax(Arg* arg) { mError = { "Invalid Syntax Of Argument", arg }; }
void tp::CmdArgParser::ErrInvalidArgType(Arg* arg) { mError = { "Invalid Type Of Argument", arg }; }
void tp::CmdArgParser::ErrFileNotExists(Arg* arg) { mError = { "File Not Exists", arg }; }
void tp::CmdArgParser::ErrFileCouldNotOpen(Arg* arg) { mError = { "Could Not Open File", arg }; }
void tp::CmdArgParser::ErrValNotinRange(Arg* arg) { mError = { "Value Not In Range", arg }; }

void tp::CmdArgParser::ErrLog() {
	printf("Command Line Error: \n");

	if (mError.mArg) {
		alni idx = 0;
		for (auto arg : mArgsOrder) {
			if (mError.mArg == arg.data()) {
				idx = arg.idx();
				break;
			}
		}

		printf("\nArgument : %lli\n", idx);
		ArgLog(*mError.mArg);
	}

	printf("Error Description : %s. \n", mError.mDescr);

	printf("\nExpected Arguments were : \n");

	for (auto arg : mArgsOrder) {
		printf(" -- %lli -- \n", arg.idx());
		ArgLog(*arg.data());
	}
}

void tp::CmdArgParser::ArgLog(Arg& arg) {
	switch (arg.mType) {
	case Arg::INT: {
		printf("Type    : Int\n");
		printf("Range   : %lli - %lli\n", arg.mInt.mAcceptingRange.mBegin, arg.mInt.mAcceptingRange.mEnd);
		if (arg.mOptional) {
			printf("Default : %lli\n", arg.mInt.mDefault);
		}
		printf("Given : %lli\n", arg.mInt.mVal);
	} break;
	case Arg::FLOAT: {
		printf("Type    : Float\n");
		printf("Range   : %f - %f\n", arg.mFloat.mAcceptingRange.mBegin, arg.mFloat.mAcceptingRange.mEnd);
		if (arg.mOptional) {
			printf("Default : %f\n", arg.mFloat.mDefault);
		}
		printf("Given : %f\n", arg.mFloat.mVal);
	} break;
	case Arg::BOOL: {
		printf("Type    : Bool\n");
		if (arg.mOptional) {
			printf("Default : %s\n", arg.mBool.mDefault ? "True" : "False");
		}
		printf("Given : %s\n", arg.mBool.mFlag ? "True" : "False");
	} break;
	case Arg::STR: {
		printf("Type    : String\n");
		if (arg.mOptional) {
			printf("Default : %s\n", arg.mStr.mDefault.cstr());
		}
		printf("Given : %s\n", arg.mStr.mStr.cstr());
	} break;
	case Arg::FILE_IN: {
		printf("Type : File Input\n");
		printf("Given Path : %s\n", arg.mFile.mFilepath.cstr());
	} break;
	}
}

void tp::CmdArgParser::initDefault(Arg& arg) {
	switch (arg.mType) {
	case Arg::INT: arg.mInt.mVal = arg.mInt.mDefault; break;
	case Arg::FLOAT: arg.mFloat.mVal = arg.mFloat.mDefault; break;
	case Arg::BOOL: arg.mBool.mFlag = arg.mBool.mDefault; break;
	case Arg::STR: arg.mStr.mStr = arg.mStr.mDefault; break;
	}
}

void tp::CmdArgParser::parseArg(Arg& arg, const char* src) {
	mTokenizer.reset();
	mTokenizer.bindSource(src);

	auto tok = mTokenizer.readTok();
	if (tok < TokType::INT || tok > TokType::STR) {
		ErrInvalidArgSyntax(&arg);
	}
	auto val = mTokenizer.extractVal();

	switch (arg.mType) {
	case Arg::INT: {
		if (tok != TokType::INT) {
			ErrInvalidArgType(&arg);
			return;
		}
		arg.mInt.mVal = val.operator tp::alni();
		if (arg.mInt.mVal < arg.mInt.mAcceptingRange.mBegin || arg.mInt.mVal > arg.mInt.mAcceptingRange.mEnd) {
			ErrValNotinRange(&arg);
			return;
		}
	} break;

	case Arg::FLOAT: {
		if (tok != TokType::FLOAT) {
			ErrInvalidArgType(&arg);
			return;
		}
		arg.mFloat.mVal = val.operator tp::alnf();
		if (arg.mFloat.mVal < arg.mInt.mAcceptingRange.mBegin || arg.mFloat.mVal > arg.mInt.mAcceptingRange.mEnd) {
			ErrValNotinRange(&arg);
			return;
		}
	} break;

	case Arg::STR: {
		if (tok != TokType::STR) {
			ErrInvalidArgType(&arg);
			return;
		}
		auto len = val.size();
		arg.mStr.mStr.reserve(len - 1);
		tp::memcp(arg.mStr.mStr.get_writable(), val.cstr() + 1, len - 2);
		arg.mStr.mStr.get_writable()[len - 2] = '\0';
	} break;

	case Arg::BOOL: {
		if (tok != TokType::BOOL_FALSE && tok != TokType::BOOL_TRUE) {
			ErrInvalidArgType(&arg);
			return;
		}
		arg.mBool.mFlag = val.operator bool();
	} break;

	case Arg::FILE_IN: {
		if (tok != TokType::STR) {
			ErrInvalidArgType(&arg);
			return;
		}

		auto len = val.size();
		arg.mFile.mFilepath.reserve(len - 1);
		tp::memcp(arg.mFile.mFilepath.get_writable(), val.cstr() + 1, len - 2);
		arg.mFile.mFilepath.get_writable()[len - 2] = '\0';

		if (!File::exists(arg.mFile.mFilepath.cstr())) {
			ErrFileNotExists(&arg);
			return;
		}
		arg.mFile.mFile.open(arg.mFile.mFilepath.cstr(), osfile_openflags::LOAD);
		if (!arg.mFile.mFile.opened) {
			ErrFileCouldNotOpen(&arg);
			return;
		}
	} break;
	}
}