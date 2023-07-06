
#include <cstdarg>

#include "strdata.h"
#include "stringt.h"

#include "filesystem.h"
#include "containers.h"
#include "log.h"

tp::str_data* gNullString = NULL;

static tp::ModuleManifest* sModuleDependencies[] = {
	&tp::gModuleContainer,
	NULL
};

static bool string_init() {
	tp::Logger::init();
	return true;
}

static void string_deinit() {
	tp::Logger::deinit();
}

tp::ModuleManifest tp::gModuleStringt = tp::ModuleManifest("String", string_init, string_deinit, sModuleDependencies);

namespace tp {

	void string::Initialize() {
		RelAssert(!gEditedStringsTable);
		gEditedStringsTable = new EditedStringsTable();
	}

	void string::UnInitialize() {
		RelAssert(gEditedStringsTable && "Forgot to Initialize Strings?");
		delete gEditedStringsTable;
	}

	string::string() {
		MODULE_SANITY_CHECK(gModuleStringt);

		//datap = gNullString;
		datap = new str_data(NULL, true);
		refinc(datap);
	}

	string::string(const char* in) { 
		MODULE_SANITY_CHECK(gModuleStringt);

		datap = NULL;
		operator=(in);
	}

	string::string(char* in) { 
		MODULE_SANITY_CHECK(gModuleStringt);

		datap = NULL;
		operator=((const char*) in);
	}

	string::string(const string& in) {
		MODULE_SANITY_CHECK(gModuleStringt);

		datap = NULL;
		operator=(in);
	}

	void string::operator=(const string& in) {
		if (datap) {
			refdec(datap);
		}
		datap = in.datap;
		refinc(datap);
	}

	void string::operator=(const char* in) {
		if (datap) {
			refdec(datap);
		}
		datap = new str_data(in, true);
		refinc(datap);
	}

	string::operator alni() const {
		alni val;
		return (datap && datap->buff && str2val(datap->buff, val, 10)) ? val : -1;
	}

	string::operator alnf() const {
		alnf val;
		return (datap && datap->buff && str2val(datap->buff, val, 10)) ? val : -1;
	}

	string::operator bool() const {
		bool val;
		return (datap && datap->buff && str2val(datap->buff, val)) ? val : -1;
	}

	const char* string::cstr() const { return datap->buff; }

	char string::operator[](alni idx) const {
		return (datap && datap->buff) ? datap->buff[idx] : 0;
	}

	alni string::size() const {
		return (datap && datap->buff) ? slen(datap->buff) : 0;
	}

	string::~string() { refdec(datap); }

	void string::refinc(str_data* dp) { dp->refc++; }

	void string::refdec(str_data* dp) {
		dp->refc--;
		if (!dp->refc) {
			delete datap;
		}
	}

	void string::assert_modifiable() {
		// if have no rights to modify create new copy of string data
		if (datap->refc > 1) {
			refdec(datap);
			datap = new str_data(*datap);
			refinc(datap);
		}

		if (datap->is_const) {
			alni len = slen(datap->buff);

			char* target = datap->buff;
			datap->buff = NULL;

			datap->reserve(len);
			memcp(datap->buff, target, len);
		}
	}

	bool string::operator==(const char* cstring) const {
		if (!datap || !datap->buff || !cstring) {
			return false;
		}

		return sequal(datap->buff, cstring);
	}

	bool string::operator==(const string& in) const {
		return in.datap ? this->operator==(in.datap->buff) : false;
	}

	bool string::operator!=(const char* cstring) const {
		return !this->operator==(cstring);
	}

	bool string::operator!=(const string& in) const {
		return !this->operator==(in);
	}

	string& string::insert(const char* in, alni at, alni len) {
		assert_modifiable();
		alni own_len = slen(datap->buff);
		assert(own_len > at && at >= 0);
		datap->insert(in, at, len ? len : slen(in));
		return *this;
	}

	string& string::capture() {
		assert_modifiable();
		return *this;
	}

	char* string::get_writable() {
		assert_modifiable();
		return datap->buff;
	}

	void string::reserve(alni len) {
		if (datap->refc > 1) {
			refdec(datap);
			datap = new str_data(*datap);
			refinc(datap);
		}

		datap->reserve(len);
	}

	string& string::insert(const string& in, alni at, alni len) {
		assert(in.datap && in.datap->buff);
		insert(in.datap->buff, at);
		return *this;
	}

	string& string::remove(Range<alni> rng) {
		assert(rng.valid());
		alni len = slen(datap->buff);
		assert(len > rng.idxEnd() && rng.idxEnd() >= 0);
		assert_modifiable();
		datap->remove(rng.idxBegin(), rng.idxEnd());
		return *this;
	}

	string& string::operator+=(const char* in) {
		assert_modifiable();
		datap->insert(in, slen(datap->buff), slen(in));
		return *this;
	}

	string& string::operator+=(const string& in) {
		assert(in.datap->buff);
		return operator+=(in.datap->buff);
	}

	string string::operator+(const string& in) const {
		string out(*this);
		return out += in;
	}

	string string::operator+(const char* in) const {
		string out(*this);
		return out += in;
	}

	void string::trim(Range<alni> rng) {
		assert(rng.valid());
		alni len = slen(datap->buff);
		assert(len > rng.idxEnd() && rng.idxEnd() >= 0);
		assert_modifiable();
		datap->remove(0, rng.idxBegin());
		datap->remove(rng.idxEnd(), len);
	}

	void string::createEdited() {
		datap->createEdited();
	}

	str::TextEditor* string::getEdited() {
		return datap->getEdited();
	}

	void string::saveEdited() {
		if (getEdited()) {
			assert_modifiable();
			datap->saveEdited();
		}
	}

	void string::clearEdited() {
		if (getEdited()) {
			assert_modifiable();
			datap->clearEdited();
		}
	}

	alni string::save_size() const {
		return size() + 1;
	}

	void string::save(File* file) const {
		file->write_bytes((int1*) cstr(), sizeof(int1) * size() + 1);
	}

	void string::load(File* file) {
		alni tmp_adress = file->adress;
		int1 val = -1;
		alni len = 0;

		while (val != '\0') {
			file->read_bytes(&val, 1);
			len++;
			assert(len < 1024 * 4);
		}

		file->adress = tmp_adress;
		assert(len);

		reserve(len);

		file->read_bytes((int1*) get_writable(), sizeof(int1) * len);
	}

	bool string::loadFile(const tp::string& file_path) {
		File file(file_path.cstr(), osfile_openflags::LOAD);
		if (!file.opened) {
			return false;
		}
		auto len = file.size();
		reserve(len);
		file.read_bytes((int1*) get_writable(), sizeof(int1) * len);
		return true;
	}

	bool string::loadFile(File* file) {
		if (!file->opened) {
			return false;
		}
		auto len = file->size();
		reserve(len);
		file->read_bytes((int1*) get_writable(), sizeof(int1) * len, 0);
		return true;
	}

	alni string::sizeAllocatedMem() {
		alni out = sizeof(str_data*);
		if (datap) {

			out += sizeof(uint4); // refc
			out += sizeof(char*); // buff
			out += sizeof(uint4); // flags
			
			if (!datap->is_const) {
				out += size() + 1;
			}
		}
		return out;
	}

	alni string::sizeUsedMem() {
		return sizeAllocatedMem();
	}

	void string::clear() {
		assert_modifiable();
		datap->clear();
	}

	#define MAX_FORMAT_ARGUMENTS 16

	string sfmt(const char* fmt, ...) {
		string out;
		alni outlen = 0;

		// chache up all sizes
		static alni sizes[MAX_FORMAT_ARGUMENTS];
		alni arg_len = 0;

		va_list args;

		// calculate size of output
		va_start(args, fmt);
		for (alni idx = 0; fmt[idx] != '\0'; idx++) {
			if (fmt[idx] != '%') {
				outlen++;
				continue;
			}
			switch (fmt[idx + 1]) {
				case 'i':
				{
					sizes[arg_len] = val2str_len(va_arg(args, alni), 10);
					break;
				}
				case 'f':
				{
					sizes[arg_len] = val2str_len(va_arg(args, alnf), 10);
					break;
				}
				case 'b':
				{
					sizes[arg_len] = val2str_len(va_arg(args, bool));
					break;
				}
				case 'c':
				{
					sizes[arg_len] = slen(va_arg(args, const char*));
					break;
				}
				case 's':
				{
#ifdef ENV_OS_ANDROID
					RelAssert(0 && "unsupported");
#else
					sizes[arg_len] = va_arg(args, string).size();
#endif
					break;
				}
				default:
				{
					sizes[arg_len] = 17 /* slen("$not implemented$") */;
					break;
				}
			}
			outlen += sizes[arg_len];
			arg_len++;
			idx++;
		}
		outlen++;
		va_end(args);

		out.reserve(outlen);
		char* outbuff = out.get_writable();

		alni out_idx = 0;
		alni arg_idx = 0;
		alni in_idx = 0;

		// fill up output
		va_start(args, fmt);
		for (; fmt[in_idx] != '\0'; in_idx++) {
			if (fmt[in_idx] != '%') {
				outbuff[out_idx] = fmt[in_idx];
				out_idx++;
				continue;
			}
			switch (fmt[in_idx + 1]) {
				case 'i':
				{
					val2str(va_arg(args, alni), &outbuff[out_idx], 10);
					break;
				}
				case 'f':
				{
					val2str(va_arg(args, alnf), 10, &outbuff[out_idx]);
					break;
				}
				case 'b':
				{
					val2str(va_arg(args, bool), &outbuff[out_idx]);
					break;
				}
				case 'c':
				{
					memcp(&outbuff[out_idx], va_arg(args, const char*), sizes[arg_idx]);
					break;
				}
				case 's':
				{
#ifdef ENV_OS_ANDROID
					RelAssert(0 && "unsupported");
#else
					memcp(&outbuff[out_idx], va_arg(args, string).cstr(), sizes[arg_idx]);
#endif
					break;
				}
				default:
				{
					memcp(&outbuff[out_idx], "$not implemented", sizes[arg_idx]);
					break;
				}
			}
			out_idx += sizes[arg_idx];
			arg_idx++;
			in_idx++;
		}
		outbuff[out_idx] = '\0';
		va_end(args);
		return out;
	}

	ualni hash(string in) { return hash(in.cstr()); }

};