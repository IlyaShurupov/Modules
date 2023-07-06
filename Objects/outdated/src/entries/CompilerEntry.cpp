
#include "Compiler/Compiler.h"

#include "primitives/primitives.h"
#include "MethodObject/MethodObject.h"
#include "log.h"

using namespace osc;
using namespace tp;
using namespace obj;

int main(int argc, char* argv[]) {

	tp::alloc_init();
	string::Initialize();
	Logger::init();

	objects_init();
	primitives_define_types();
	MethodObject::Initialize();

	{
		Compiler compiler;

		tp::string script;

		RelAssert(argc == 2 && "Invalid Arguments");

		script.loadFile(argv[1]);

		compiler.compile(script);

		GLog->write("oscc : compilation finished", 1);
	}

	try {
		MethodObject::UnInitialize();
		objects_finalize();
		primitives_uninitialize();

		Logger::deinit();
		string::UnInitialize();
		tp::alloc_uninit();

	}
	catch (...) {
		tp::terminate();
	}
}