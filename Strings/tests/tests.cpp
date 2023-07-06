

#include "common.h"

#include "allocators.h"

#include "strings.h"

#include <iostream> 

#include <stdio.h> 
#include <string> 
#include <algorithm> 

#include "texteditor.h"

#include "log.h"

using namespace tp;

void test_conversions() {

	for (auto i : tp::Range(1000)) {

		auto sign = bool(std::rand() % 2) ? 1 : -1;

		auto floating = std::rand() / (tp::alnf)std::rand() * sign;
		auto tmp = std::to_string(floating);
		auto left = tp::string(tmp.c_str());
		auto right = tp::string(floating);
		DBG_BREAK(left != right);
		auto back = (tp::alnf)right;
		//DBG_BREAK(back != floating);

		auto integer = std::rand() * sign;
		tmp = std::to_string(integer);
		left = tp::string(tmp.c_str());
		right = tp::string(integer);
		DBG_BREAK(left != right);
		back = (tp::alni)right;
		DBG_BREAK(back != integer);
	}
}

void test_editor() {
	tp::string str = " initial ";
	str.createEdited();
	auto edited = str.getEdited();
	edited->setCursor({0, 4});
	edited->insert({"aaa", 3});
	str.saveEdited();
	str.clearEdited();

	GLog->write(str, true);
}

int main() {
	tp::alloc_init();
	string::Initialize();
	Logger::init();
	//test_conversions();
	test_editor();
	Logger::deinit();
	string::UnInitialize();
	tp::alloc_uninit();
}