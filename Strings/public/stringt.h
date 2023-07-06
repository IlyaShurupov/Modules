#pragma once

#include "algorithms.h"
#include "strdata.h"

namespace tp {

	extern ModuleManifest gModuleStringt;

	class string {

		class str_data* datap;

		public:

		string();
		string(const char* in);
		string(char* str);
		string(const string& in);

		template <typename Type>
		string(Type val) {
			datap = new str_data(val2str(val), false);
			refinc(datap);
		}

		explicit operator alni() const;
		explicit operator alnf() const;
		explicit operator bool() const;
		//operator const char*() const;

		~string();

		private: // own utils
		void refinc(str_data* dp);
		void refdec(str_data* dp);
		void assert_modifiable();

		public: // data info access 
		const char* cstr() const;
		char operator[](alni idx) const;
		alni size() const;

		bool operator==(const char* cstring) const;
		bool operator==(const string& string) const;
		bool operator!=(const char* cstring) const;
		bool operator!=(const string& string) const;

		public: // editing interface	

		string& capture();

		char* get_writable();
		void reserve(alni len);

		string& insert(const string& string, alni at, alni len = 0);
		string& insert(const char* string, alni at, alni len = 0);

		string& remove(Range<alni> range);

		void operator=(const string& string);
		void operator=(const char* string);

		string& operator+=(const string& string);
		string& operator+=(const char* string);
		string operator+(const string& string) const;
		string operator+(const char* string) const;

		void trim(Range<alni> range);
		void clear();

		void createEdited();
		str::TextEditor* getEdited();
		void saveEdited();
		void clearEdited();

		alni save_size() const ;
		void save(class File* file) const;
		void load(class File* file);
		bool loadFile(const tp::string& file_path);
		bool loadFile(File* file);

		alni sizeAllocatedMem();
		alni sizeUsedMem();

		static void Initialize();
		static void UnInitialize();

	};

	string sfmt(const char* fmt, ...);

	ualni hash(string);
};