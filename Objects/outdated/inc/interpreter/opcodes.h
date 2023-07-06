
#pragma once

#include "common.h"

// No Nested Scopes

// Opcodes:
// Opcode input can be:
// 0) No Input
// 1) operands from OperandsStack (Operand)
// 2) Index of bytecode->ConstData from bytecode->Instruction (ConstData)
// 3) Raw Bytes from bytecode->Instruction (Param)

namespace obj {

	extern struct OpcodeInfos gOpcodeInfos;

	enum class OpCode : tp::uint1 {

		NONE = 0x00,
		HALT,
		TERMINATE,

		DEF_LOCAL,
		// Operand : String : local_id
		// Operand : Any : object to be local

		LOAD_CONST,
		LOAD_LOCAL,
		// ConstData : idx of const object

		IGNORE,

		SCOPE_IN,
		SCOPE_OUT,
		
		CALL,
		RETURN_OBJ,
		// Operand : Base : returned object

		RETURN,

		CHILD,
		// Operand : String : child_id
		// Operand : Base : parent
		// Param : 1 bytes : is method?

		PUSH_ARGS,
		SAVE_ARGS,
		// Param : nubber of args

		OBJ_CREATE_LOCAL,
		//	Operand : String : type 
		//	Operand : String : id

		OBJ_CREATE,
		//	Operand : String : type 

		OBJ_COPY,
		//	Operand : Base: target
		//	Operand : Base: blueprint

		OBJ_SAVE,
		OBJ_LOAD,
		//	Operand : String : path
		//	Operand : Base : target

		OBJ_ADD,
		OBJ_SUB,
		OBJ_MUL,
		OBJ_DIV,
		
		AND,
		OR,
		EQUAL,
		NOT_EQUAL,
		MORE,
		LESS,
		EQUAL_OR_MORE,
		EQUAL_OR_LESS,
		//	Operand : Base: left
		//	Operand : Base: right

		NOT,
		//	Operand : Base: inv

		JUMP,
		JUMP_R,
		//	Param : 2 bytes : offset

		JUMP_IF,
		JUMP_IF_R,
		JUMP_IF_NOT,
		JUMP_IF_NOT_R,
		//	Operand : Base: condition
		//	Param : 2 bytes : offset


		PRINT,
		//	Operand : Base: target

		CLASS_CONSTRUCT,
		SELF,

		// ...

		END_OPCODES
	};

	struct OpcodeInfos {
		
		struct OperandsInfo {
			struct Operand {
				const char* obj_type = NULL;
				const char* desc = NULL;
			};

			enum { MAX_OPERANDS = 5 };
			Operand buff[MAX_OPERANDS];
			tp::halni len = 0;

			OperandsInfo();
			OperandsInfo(tp::init_list<Operand> list);
		};

		struct ParamsInfo {
			struct Param {
				const char* desc = NULL;
				tp::halni bytes = 1;
			};

			enum { MAX_PARAMS = 5 };
			Param buff[MAX_PARAMS];
			tp::halni len = 0;

			ParamsInfo();
			ParamsInfo(tp::init_list<Param> list);
		};

		struct OpInfo {
			const char* name = NULL;
			const char* desc = NULL;
			OperandsInfo operands;
			ParamsInfo params;

			tp::uint1 opsize();
		};

		OpcodeInfos();
		OpInfo fetch(OpCode code);

	private:
		OpInfo buff[(tp::alni)OpCode::END_OPCODES];

		void add(OpCode code, const OpInfo& info);
	};
};