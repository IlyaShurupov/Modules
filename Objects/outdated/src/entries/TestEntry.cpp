

#include "MethodObject/methodobject.h"
#include "primitives/primitives.h"

#include "Interpreter/Interpreter.h"

#include "log.h"

#include "ByteCodeGen/Function.h"

using namespace osc;
using namespace tp;
using namespace obj;

void TestCompile(ByteCode& bytecode) {
	using namespace BCgen;

	Genereate(bytecode, 
	StmScope({

		StmDefFunc("main", {}, {
			StmDefLocal("i1", ExprConst(1)),
			StmDefLocal("i2", ExprConst(2)),

			StmDefFunc("add", {"first", "second"}, {
				StmReturn(ExprAdd(ExprLocal("first"), ExprLocal("second")))
			}),

			StmPrint(ExprLocal("i1")),
			StmPrint(ExprConst(" + ")),
			StmPrint(ExprLocal("i2")),
			StmPrint(ExprConst(" = ")),
			
			StmPrint(ExprFunc("add")->ExprCall({ ExprLocal("i1"), ExprLocal("i2") })),

			StmPrint(ExprConst("\n")),
		}),

		StmPrint(ExprBoolNot(ExprConst(0))),

		StmWhile(ExprConst(false), StmScope({
				StmIgnore(ExprFunc("main")->ExprCall({}))
			})
		),

		StmIf(ExprConst(true),
			StmScope({
				//StmIgnore(ExprFunc("main")->ExprCall({})),
				//StmPrint(ExprConst("true")),
			}),
			StmScope({
				//StmPrint(ExprConst("false")),
			})
		)
	})
	);
}

int main(int argc, char* argv[]) {

	tp::alloc_init();
	string::Initialize();
	Logger::init();

	objects_init();
	primitives_define_types();
	MethodObject::Initialize();

	{
		Interpreter interp;

		ByteCode bytecode;
		TestCompile(bytecode);

		GLog->write(" >> Exec: start \n");
		auto start = get_time();
		interp.exec(&bytecode);
		auto end = get_time();
		GLog->write(sfmt("\n >> Exec time %i ms", end - start));
	}

	MethodObject::UnInitialize();
	objects_finalize();
	primitives_uninitialize();

	Logger::deinit();
	string::UnInitialize();
	tp::alloc_uninit();
}