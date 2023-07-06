
#include "tokenizer.h"
#include "trigonometry.h"
#include "stringt.h"

static tp::ModuleManifest* sModuleDependencies[] = {
	&tp::gModuleMath, 
	&tp::gModuleStringt,
	NULL
};

tp::ModuleManifest tp::gModuleTokenizer = ModuleManifest("Tokenizer", NULL, NULL, sModuleDependencies);

void foo() {}