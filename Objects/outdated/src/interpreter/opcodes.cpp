
#include "interpreter/opcodes.h"

#include "cassert.h"

namespace obj {
	OpcodeInfos gOpcodeInfos;
};

using namespace obj;

#define CONST_IDX_BYTES 2

#define OP(opcode, name, desc, ops, params) \
	add(opcode, { #name, #desc, ops, params } );

OpcodeInfos::OperandsInfo::OperandsInfo() {}
OpcodeInfos::OperandsInfo::OperandsInfo(tp::init_list<Operand> list) {
	assert(MAX_OPERANDS >= list.size());
	for (auto item : list) {
		buff[len] = item;
		len++;
	}
}

OpcodeInfos::ParamsInfo::ParamsInfo() {}
OpcodeInfos::ParamsInfo::ParamsInfo(tp::init_list<Param> list) {
	assert(MAX_PARAMS >= list.size());
	for (auto item : list) {
		buff[len] = item;
		len++;
	}
}

tp::uint1 OpcodeInfos::OpInfo::opsize() {
	tp::uint1 out = 1;
	for (auto i = 0; i < params.len; i++) {
		out += params.buff[i].bytes;
	}
	return out;
}

OpcodeInfos::OpcodeInfos() {

	add(OpCode::NONE, { "NONE", "Does Nothing" });
	add(OpCode::HALT, { "HALT", "Halts for 3 sec" });
	add(OpCode::TERMINATE, { "TERMINATE", "Terminates the process" });
	add(OpCode::DEF_LOCAL, {
			.name = "DEF LOCAL",
			.desc = "Adds object to the locals",
			.operands = {
				{ "str", "local id" },
				{ "any", "object to be local" }
			}
		}
	);
	add(OpCode::LOAD_CONST, {
			.name = "LOAD CONST",
			.desc = "Loads const object from the const pool",
			.params = {
				{ "const obj idx", CONST_IDX_BYTES },
			}
		}
	);
	add(OpCode::LOAD_LOCAL, {
			.name = "LOAD LOCAL",
			.desc = "Loads local object from the locals",
			.params = {
				{ "idx of const StringObject - represents name id", CONST_IDX_BYTES },
			}
		}
	);
	add(OpCode::IGNORE, {
			.name = "IGNORE",
			.desc = "Ignores returned object by destroying it",
		}
	);
	add(OpCode::SCOPE_IN, {
			.name = "SCOPE IN",
			.desc = "Enters new scope",
		}
	);
	add(OpCode::SCOPE_OUT, {
			.name = "SCOPE OUT",
			.desc = "Leaves current scope",
		}
	);
	add(OpCode::CALL, {
			.name = "CALL",
			.desc = "Leaves current scope",
			.operands = {
				{ "method", "method object" },
			}
		}
	);
	add(OpCode::RETURN_OBJ, {
			.name = "RETURN OBJ",
			.desc = "Returns Operand",
			.operands = {
				{ "any", "object to be returned" },
			}
		}
	);
	add(OpCode::RETURN, {
			.name = "RETURN",
			.desc = "Returns Null Object",
		}
	);
	add(OpCode::CHILD, {
			.name = "CHILD",
			.desc = "Retrieves child from class",
			.operands = {
				{ "str", "child id" },
				{ "class", "parent class" },
			},
			.params = {
				{ "is method ?", 1 }
			}
		}
	);
	add(OpCode::PUSH_ARGS, {
			.name = "PUSH ARGS",
			.desc = "Pushes Separator on the OperandsStack",
			.params = {
				{ "length to chech number of args", 1 }
			}
		}
	);
	add(OpCode::SAVE_ARGS, {
			.name = "SAVE ARGS",
			.desc = "Pushes operands to locals",
			.params = {
				{ "number of arguments in function defenition", 1 }
			}
		}
	);
	add(OpCode::OBJ_CREATE_LOCAL, {
			.name = "CREATE LOCAL OBJ",
			.desc = "creates object of given type and adds it to the locals",
			.operands = {
				{ "str", "types" },
				{ "str", "name id" },
			}
		}
	);
	add(OpCode::OBJ_CREATE, {
			.name = "CREATE OBJ",
			.desc = "creates object of given type",
			.operands = {
				{ "str", "types" },
			}
		}
	);
	add(OpCode::OBJ_COPY, {
			.name = "COPY",
			.desc = "Copies objects",
			.operands = {
				{ "any", "self" },
				{ "any", "target" },
			}
		}
	);
	add(OpCode::OBJ_SAVE, {
			.name = "SAVE",
			.desc = "Saves object to a file",
			.operands = {
				{ "str", "path" },
				{ "any", "self" },
			}
		}
	);
	add(OpCode::OBJ_LOAD, {
			.name = "LOAD",
			.desc = "loads object from a file",
			.operands = {
				{ "str", "path" },
				{ "any", "self" },
			}
		}
	);
	add(OpCode::OBJ_ADD, {
			.name = "ADD",
			.desc = "Adds objects of the same type that supports ariphmetics",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::OBJ_SUB, {
			.name = "SUB",
			.desc = "Subtruts objects of the same type that supports ariphmetics",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::OBJ_MUL, {
			.name = "MUL",
			.desc = "Multiplies objects of the same type that supports ariphmetics",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::OBJ_DIV, {
			.name = "DIV",
			.desc = "Divides objects of the same type that supports ariphmetics",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::AND, {
			.name = "AND",
			.desc = "Logical AND of objects of the same type that supports boolean ariphmetics",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::OR, {
			.name = "OR",
			.desc = "Logical OR of objects of the same type that supports boolean ariphmetics",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::EQUAL, {
			.name = "EQUAL",
			.desc = "Compares objects",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::NOT_EQUAL, {
			.name = "NOT EQUAL",
			.desc = "Compares objects and inverts the resault",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::MORE, {
			.name = "MORE",
			.desc = " > ",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::LESS, {
			.name = "LESS",
			.desc = " < ",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::EQUAL_OR_MORE, {
			.name = "GRATER OR EQUAL",
			.desc = " >= ",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::EQUAL_OR_LESS, {
			.name = "LESS OR EQUAL",
			.desc = " <= ",
			.operands = {
				{ "any", "left" },
				{ "any", "right" },
			}
		}
	);
	add(OpCode::NOT, {
			.name = "NOT",
			.desc = "Inverts the object bolean representation",
			.operands = {
				{ "any", "self" },
			}
		}
	);
	add(OpCode::JUMP, {
			.name = "JUMP",
			.desc = "Offsets the instruction pointer",
			.params = {
				{ "ip offset unsigned", 2 },
			}
		}
	);
	add(OpCode::JUMP_R, {
			.name = "JUMP R",
			.desc = "Offsets the instruction pointer in reversed direction",
			.params = {
				{ "ip offset unsigned", 2 },
			}
		}
	);
	add(OpCode::JUMP_IF, {
			.name = "JUMP IF",
			.desc = "Offsets the instruction pointer if condition is met",
			.operands = {
				{ "any", "condition object" },
			},
			.params = {
				{ "ip offset unsigned", 2 },
			}
		}
	);
	add(OpCode::JUMP_IF_R, {
			.name = "JUMP IF R",
			.desc = "Offsets the instruction pointer in reversed direction if condition is met",
			.operands = {
				{ "any", "condition object" },
			},
			.params = {
				{ "ip offset unsigned", 2 },
			}
		}
	);
	add(OpCode::JUMP_IF_NOT, {
			.name = "JUMP IF R",
			.desc = "Offsets the instruction pointer if condition is NOT met",
			.operands = {
				{ "any", "condition object" },
			},
			.params = {
				{ "ip offset unsigned", 2 },
			}
		}
	);
	add(OpCode::JUMP_IF_NOT_R, {
			.name = "JUMP IF R",
			.desc = "Offsets the instruction pointer in reversed direction if condition is NOT met",
			.operands = {
				{ "any", "condition object" },
			},
			.params = {
				{ "ip offset unsigned", 2 },
			}
		}
	);
	add(OpCode::PRINT, {
			.name = "PRINT",
			.desc = "Prints the object string representation",
			.operands = {
				{ "any", "self" },
			}
		}
	);
	add(OpCode::CLASS_CONSTRUCT, {
			.name = "CLASS CONSTRUCT",
			.desc = "Creates class from the function execution state",
		}
	);
	add(OpCode::SELF, {
			.name = "SELF",
			.desc = "retrieves parent class of the current method",
		}
	);
}

OpcodeInfos::OpInfo OpcodeInfos::fetch(OpCode code) {
	assert((tp::alni)code >= 0 && (tp::alni)code < (tp::alni)OpCode::END_OPCODES);
	return buff[(tp::alni)code];
}

void OpcodeInfos::add(OpCode code, const OpInfo& info) {
	buff[(tp::alni)code] = info;
}