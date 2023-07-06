
#include "primitives/primitives.h"

#include "compiler/function.h"
#include "interpreter/interpreter.h"

#include "CmdArgParser.h"

void compile(char argc, const char* argv[]) {
	tp::CmdArgParser args = {
		{ "script", tp::CmdArgParser::Arg::FILE_IN },
		{ "out", "out.mo" }
	};

	if (!args.parse(argc, argv, true)) {
		return;
	}

	auto& script_file = args.getFile("script");
	auto outpath = args.getString("out");

	tp::string script;
	script.loadFile(&script_file);

	auto method = obj::MethodObject::create(script);

	obj::NDO->save(method, outpath);
}

void execute(char argc, const char* argv[]) {
	tp::CmdArgParser args = {
		{ "object_path", tp::CmdArgParser::Arg::STR },
		{ "debug", false }
	};

	if (!args.parse(argc, argv, true)) {
		return;
	}

	auto object_path = args.getString("object_path");
	auto debug = args.getBool("debug");

	auto obj = obj::NDO->load(object_path);
	if (!obj) {
		printf("Invalid Object File\n");
		return;
	}

	NDO_CASTV(obj::MethodObject, obj, method);
	if (!method) {
		printf("Object Is Not A Method\n");
		return;
	}

	obj::Interpreter interpreter;
	interpreter.execAll(method);
}


int main(char argc, const char* argv[]) {

	tp::ModuleManifest* ModuleDependencies[] = { &obj::gModuleObjects, NULL };
	tp::ModuleManifest TestModule("Test", NULL, NULL, ModuleDependencies);
	if (!TestModule.initialize()) {
		return 1;
	}

#ifdef OBC
	compile(argc, argv);
#elif OBVM
	execute(argc, argv);
#endif // OBC

	TestModule.deinitialize();
}