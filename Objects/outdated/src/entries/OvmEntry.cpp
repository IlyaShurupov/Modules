

#include "MethodObject/methodobject.h"
#include "primitives/primitives.h"

#include "Interpreter/Interpreter.h"

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
		time_ms load_start = get_time();
		RelAssert(argc == 2 && "Invalid Arguments");
		auto obj = NDO->load(argv[1]);
		RelAssert(obj && "Failed to Load the File");
		RelAssert(obj->type == &MethodObject::TypeData && "Innvalid file content");

		Interpreter interp;
		NDO_CASTV(MethodObject, obj, method);

		RelAssert(method->mScript && "Innvalid file content");
		time_ms load_end = get_time();

		GLog->write("\noscvm - execution started\n");
		time_ms exec_start = get_time();
		interp.exec(&method->mScript->mBytecode);
		time_ms exec_end = get_time();

		auto exec_time = (exec_end - exec_start);
		auto load_time = (load_end - load_start);
		GLog->write(sfmt("\noscvm - execution finished (Load : %f Exec : %f)", load_time, exec_time));
	}

	try {
		MethodObject::UnInitialize();
		objects_finalize();
		primitives_uninitialize();

		Logger::deinit();
		string::UnInitialize();
		tp::alloc_uninit();

	} catch (...) {
		tp::terminate();
	}
}