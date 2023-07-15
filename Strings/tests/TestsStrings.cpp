
#include "Testing.hpp"
#include "Strings.hpp"

TEST_DEF_STATIC(StringLogic) {
	TEST(false);
}

TEST_DEF_STATIC(Simple) {
	TEST(false);
}

TEST_DEF(Strings) {
	testStringLogic();
	testSimple();
}