
#include "interpreter/interpreter.h"

#include "primitives/primitives.h"

#include "log.h"

using namespace obj;

inline tp::uint1 read_byte(ByteCode* bytecode) {
	auto out = (tp::uint1)bytecode->mInstructions[bytecode->mInstructionIdx]; 
	bytecode->mInstructionIdx++;
	return out;
}

// exeption for opcodes 
// reads 2 bytes from instructions in order to load constant onto operands stack
tp::uint2 loadConstDataIdx(ByteCode* bytecode) {
	tp::uint2 out = 0;
	out |= ((tp::uint2) read_byte(bytecode));
	out |= ((tp::uint2) read_byte(bytecode) << 8);
	return out;
}

// exeption for opcodes 
// reads 2 bytes from instructions in order to load constant onto operands stack
void skip_param(ByteCode* bytecode) {
	bytecode->mInstructionIdx += 2;
}

tp::uint2 param(ByteCode* bytecode) {
	return loadConstDataIdx(bytecode);
}

void Interpreter::exec(obj::MethodObject* method, obj::ClassObject* self, obj::DictObject* globals, tp::init_list<GlobalDef> globals2) {
	if (!method->mScript->mBytecode.mInstructions.length()) {
		return;
	}

	mCallStack.enter({ NULL, method, 0 });
	mCallStack.mStack.last().mSelf = self;

	stepBytecodeIn();

	if (globals) {
		for (auto glb : globals->getItems()) {
			mScopeStack.addLocal(glb->val, glb->key);
		}
	}

	for (auto global_def : globals2) {
		mScopeStack.addLocal(global_def.obj, global_def.id);
	}
}

bool Interpreter::finished() {
	return !mCallStack.len();
}

void Interpreter::stepBytecode() {
	tp::halni call_depth = mCallStack.len();
	do {
		stepBytecodeIn();
		if (finished()) {
			return;
		}
	} while (call_depth != mCallStack.len());
}

void Interpreter::stepBytecodeOut() {
	if (finished()) {
		return;
	}

	tp::halni call_depth = mCallStack.len();
	if (!call_depth) {
		return;
	}

	do {
		stepBytecodeIn();
		if (finished()) {
			return;
		}
	} while (call_depth - 1 != mCallStack.len());
}

void Interpreter::stepBytecodeIn() {
	using namespace obj;
	using namespace tp;

	DBG_BREAK(finished());

	auto bytecode = mCallStack.getBytecode();

	if (bytecode->mInstructionIdx >= (tp::ualni) bytecode->mInstructions.length()) {
		// just return 
		if (mScopeStack.mIdx) {
			mScopeStack.leaveScope();
		}

		mCallStack.leave();

		if (mCallStack.len()) {
			mOperandsStack.push(NDO_NULL_REF);
		}
		return;
	}

	auto opcode = bytecode->mInstructions[bytecode->mInstructionIdx];
	bytecode->mInstructionIdx++;

	switch (opcode) {

		case OpCode::NONE: {
			break;
		}

		case OpCode::HALT: {
			while (true) {
				tp::sleep(3);
			}
			break;
		}
	
		case OpCode::TERMINATE: {
			//tp::terminate(0);
		}

		case OpCode::IGNORE: {
			NDO->destroy(mOperandsStack.getOperand());
			break;
		}

		case OpCode::LOAD_CONST: {
			auto idx = loadConstDataIdx(bytecode);
			auto const_obj = bytecode->mConstants[idx];
			mOperandsStack.push(const_obj);
			break;
		}

		case OpCode::LOAD_LOCAL: {
			auto idx = loadConstDataIdx(bytecode);
			auto const_obj = bytecode->mConstants[idx];
			NDO_CASTV(StringObject, const_obj, local_id);
			RelAssert(local_id && "Invalid Object Type");
			auto local = mScopeStack.getLocal(local_id->val);
			mOperandsStack.push(local);
			break;
		}

		case OpCode::SCOPE_IN: {
			mScopeStack.enterScope(true);
			break;
		}
		
		case OpCode::JUMP: {
			bytecode->mInstructionIdx += param(bytecode);
			break;
		}

		case OpCode::JUMP_IF: {
			auto cond = mOperandsStack.getOperand();
			if (NDO->toBool(cond)) {
				bytecode->mInstructionIdx += param(bytecode);
			} else {
				skip_param(bytecode);
			}
			break;
		}

		case OpCode::JUMP_IF_NOT: {
			auto cond = mOperandsStack.getOperand();
			if (!NDO->toBool(cond)) {
				bytecode->mInstructionIdx += param(bytecode);
			} else {
				skip_param(bytecode);
			}
			break;
		}

		case OpCode::JUMP_R: {
			bytecode->mInstructionIdx -= param(bytecode);
			break;
		}

		case OpCode::JUMP_IF_R: {
			auto cond = mOperandsStack.getOperand();
			if (NDO->toBool(cond)) {
				bytecode->mInstructionIdx -= param(bytecode);
			} else {
				skip_param(bytecode);
			}
			break;
		}

		case OpCode::JUMP_IF_NOT_R: {
			auto cond = mOperandsStack.getOperand();
			if (!NDO->toBool(cond)) {
				bytecode->mInstructionIdx -= param(bytecode);
			} else {
				skip_param(bytecode);
			}
			break;
		}

		case OpCode::SCOPE_OUT: {
			mScopeStack.leaveScope();
			break;
		}

		case OpCode::PRINT: {
			auto obj = mOperandsStack.getOperand();
			if (obj->type->convesions && obj->type->convesions->to_string) {
				auto str = NDO->toString(obj);
				GLog->write(str.cstr(), true);
			} else {
				GLog->write(obj->type->name, true);
			}
			break;
		}

		case OpCode::OBJ_CREATE_LOCAL: {
			auto type = mOperandsStack.getOperand<StringObject>();
			auto id = mOperandsStack.getOperand<StringObject>();
			mScopeStack.addLocal(NDO->create(type->val), id->val);
			break;
		}
		
		case OpCode::DEF_LOCAL: {
			auto id = mOperandsStack.getOperand<StringObject>();
			auto obj = mOperandsStack.getOperand();
			mScopeStack.addLocal(obj, id->val);
			NDO->refinc(obj);
			//mScopeStack.popTemp();
			break;
		}

		case OpCode::OBJ_CREATE: {
			auto type = mOperandsStack.getOperand<StringObject>();
			Object* new_obj = NULL;

			// basic types
			auto idx = NDO->types.presents(type->val);
			if (idx) {
				auto new_obj = NDO->create(type->val);

				mOperandsStack.push(new_obj);
				mScopeStack.addTemp(new_obj);
				break;
			}


			// class creation
			auto local_class = mScopeStack.getLocal(type->val);
			NDO_CASTV(MethodObject, local_class, method);
			RelAssert(method);

			// class is a function - execute it as a constructor

			// PUSH_ARGS protocol
			tp::uint2 len = 0;
			mOperandsStack.push((Object*)len);
			mOperandsStack.push(NULL);

			// CALL protocol
			mScopeStack.enterScope(false);
			mCallStack.enter({ NULL, method, 0 });
			break;
		}

		case OpCode::CLASS_CONSTRUCT: {
			auto class_obj = (ClassObject*)NDO->create("class");

			for (auto local : mScopeStack.getCurrentScope()->mLocals) {
				class_obj->addMember(local->val, local->key);
			}

			mOperandsStack.push(class_obj);
			mScopeStack.addTempReturn(class_obj);

			mScopeStack.leaveScope();
			mCallStack.leave();
			return;
		}

		case OpCode::RETURN: {
			//mScopeStack.addTempReturn(NDO_NULL_REF);
			mScopeStack.leaveScope();
			mCallStack.leave();
			if (mCallStack.len()) {
				mOperandsStack.push(NDO_NULL_REF);
			}
			break;
		}
		
		case OpCode::PUSH_ARGS: {
			
			// Layout of OperandsStack:
			// ....
			// +1) length : to chech number of args
			// +2) NULL : stop saving args
			// +3) object1
			// +4) object2
			// ...

			tp::uint2 len = read_byte(bytecode);
			mOperandsStack.push((Object*) len);
			mOperandsStack.push(NULL);
			break;
		}

		case OpCode::SAVE_ARGS: {
			tp::uint2 args_len = read_byte(bytecode);
			auto argument = mOperandsStack.getOperand();

			while (argument) {
				NDO->refinc(argument);

				auto argument_id = bytecode->mConstants[loadConstDataIdx(bytecode)];
				NDO_CASTV(StringObject, argument_id, id);
				assert(id);
				mScopeStack.addLocal(argument, id->val);
				bytecode->mArgumentsLoaded++;
				argument = mOperandsStack.getOperand();
			}

			tp::uint2 saved_len = tp::uint2 (tp::ualni (mOperandsStack.getOperand()));
			RelAssert(args_len == saved_len && "invalid number of arguments passefd");
			
			break;
		}

		case OpCode::RETURN_OBJ: {
			if (mCallStack.len() > 1) {
				auto ret = mOperandsStack.getOperand();
				mOperandsStack.push(ret);
				mScopeStack.addTempReturn(ret);
			}

			mScopeStack.leaveScope();
			mCallStack.leave();
			break;
		}

		case OpCode::CALL: {
			auto obj = mOperandsStack.getOperand();

			if (!mIsTypeMethod) {
				NDO_CASTV(MethodObject, obj, method);

				mScopeStack.enterScope(false);
				mCallStack.enter({ NULL, method, 0 });

				// push self
				mCallStack.mStack.last().mSelf = mLastParent;
				break;
			}

			(*mTypeMethod)(this);
			mIsTypeMethod = false;
			mTypeMethod = NULL;
			break;
		}

		case OpCode::OBJ_ADD: {
			auto left = mOperandsStack.getOperand();
			auto right = mOperandsStack.getOperand();

			RelAssert(left->type == right->type && "addition of different types is not implemented");
			RelAssert(left->type->ariphmetics && left->type->ariphmetics->add && "cannot add object of this type");

			auto res = NDO->instatiate(left);
			res->type->ariphmetics->add(res, right);

			mScopeStack.addTemp(res);
			mOperandsStack.push(res);
			break;
		}
		case OpCode::OBJ_SUB: {
			auto left = mOperandsStack.getOperand();
			auto right = mOperandsStack.getOperand();

			RelAssert(left->type == right->type && "addition of different types is not implemented");
			RelAssert(left->type->ariphmetics && left->type->ariphmetics->add && "cannot add object of this type");

			auto res = NDO->instatiate(left);
			res->type->ariphmetics->sub(res, right);

			mScopeStack.addTemp(res);
			mOperandsStack.push(res);
			break;
		}
		case OpCode::OBJ_MUL: {
			auto left = mOperandsStack.getOperand();
			auto right = mOperandsStack.getOperand();

			RelAssert(left->type == right->type && "addition of different types is not implemented");
			RelAssert(left->type->ariphmetics && left->type->ariphmetics->add && "cannot add object of this type");

			auto res = NDO->instatiate(left);
			res->type->ariphmetics->mul(res, right);

			mScopeStack.addTemp(res);
			mOperandsStack.push(res);
			break;
		}
		case OpCode::OBJ_DIV: {
			auto left = mOperandsStack.getOperand();
			auto right = mOperandsStack.getOperand();

			RelAssert(left->type == right->type && "addition of different types is not implemented");
			RelAssert(left->type->ariphmetics && left->type->ariphmetics->add && "cannot add object of this type");

			auto res = NDO->instatiate(left);
			res->type->ariphmetics->div(res, right);

			mScopeStack.addTemp(res);
			mOperandsStack.push(res);
			break;
		}

		case OpCode::OBJ_COPY: {
			auto left = mOperandsStack.getOperand();
			auto right = mOperandsStack.getOperand();
			NDO->copy(left, right);
			break;
		}
		
		case OpCode::CHILD: {
			auto child_id = mOperandsStack.getOperand<StringObject>();
			auto parent = mOperandsStack.getOperand();
			bool is_method = read_byte(bytecode);

			Object* child = NULL;
			TypeMethods::LookupKey tm_key;

			if (is_method) {
				tm_key = TypeMethods::presents(parent->type, child_id->val);
			}

			if (tm_key) {
				mTypeMethod = TypeMethods::getMethod(parent->type, tm_key);
				mIsTypeMethod = true;

			}	else {
				NDO_CASTV(ClassObject, parent, class_obj);
				RelAssert(class_obj && "not a class object");
				auto idx = class_obj->members->presents(child_id->val);
				RelAssert(idx && "No child with such id");
				child = class_obj->members->getSlotVal(idx);
			}
	
			//mScopeStack.addTemp(obj);
			mOperandsStack.push(child);
			mLastParent = parent;
			break;
		}

		case OpCode::SELF: {
			//mScopeStack.addTemp(obj);
			RelAssert(mCallStack.mStack.last().mSelf);
			mOperandsStack.push(mCallStack.mStack.last().mSelf);
			break;
		}
		
		case OpCode::OBJ_LOAD: {
			auto path = mOperandsStack.getOperand<StringObject>();
			auto obj = NDO->load(path->val);
			mScopeStack.addTemp(obj);
			mOperandsStack.push(obj);
			break;
		}
		case OpCode::OBJ_SAVE: {
			auto path = mOperandsStack.getOperand<StringObject>();
			auto target = mOperandsStack.getOperand();
			NDO->save(target, path->val);
			break;
		}

		case OpCode::AND: { 
			auto left = NDO->toBool(mOperandsStack.getOperand());
			auto right = NDO->toBool(mOperandsStack.getOperand());
			auto out = BoolObject::create(left && right);
			mScopeStack.addTemp(out);
			mOperandsStack.push(out);
			break; 
		}
		case OpCode::OR: { 
			auto left = NDO->toBool(mOperandsStack.getOperand());
			auto right = NDO->toBool(mOperandsStack.getOperand());
			auto out = BoolObject::create(left || right);
			mScopeStack.addTemp(out);
			mOperandsStack.push(out);
			break; 
		}
		case OpCode::NOT: {
			auto inv = NDO->toBool(mOperandsStack.getOperand());
			auto out = BoolObject::create(!inv);
			mScopeStack.addTemp(out);
			mOperandsStack.push(out);
			break;
		}
		case OpCode::NOT_EQUAL: {
			auto left = mOperandsStack.getOperand();
			auto right = mOperandsStack.getOperand();
			auto out = BoolObject::create(!NDO->compare(left, right));
			mScopeStack.addTemp(out);
			mOperandsStack.push(out);
			break;
		}
		case OpCode::EQUAL: { 
			auto left = mOperandsStack.getOperand();
			auto right = mOperandsStack.getOperand();
			auto out = BoolObject::create(NDO->compare(left, right));
			mScopeStack.addTemp(out);
			mOperandsStack.push(out);
			break; 
		}

		case OpCode::MORE: {
			auto left = NDO->toFloat(mOperandsStack.getOperand());
			auto right = NDO->toFloat(mOperandsStack.getOperand());
			auto out = BoolObject::create(left > right);
			mScopeStack.addTemp(out);
			mOperandsStack.push(out);
			break;
		}
		case OpCode::LESS: {
			auto left = NDO->toFloat(mOperandsStack.getOperand());
			auto right = NDO->toFloat(mOperandsStack.getOperand());
			auto out = BoolObject::create(left < right);
			mScopeStack.addTemp(out);
			mOperandsStack.push(out);
			break;
		}
		case OpCode::EQUAL_OR_MORE: {
			auto left = NDO->toFloat(mOperandsStack.getOperand());
			auto right = NDO->toFloat(mOperandsStack.getOperand());
			auto out = BoolObject::create(left >= right);
			mScopeStack.addTemp(out);
			mOperandsStack.push(out);
			break;
		}
		case OpCode::EQUAL_OR_LESS: { 
			auto left = NDO->toFloat(mOperandsStack.getOperand());
			auto right = NDO->toFloat(mOperandsStack.getOperand());
			auto out = BoolObject::create(left <= right);
			mScopeStack.addTemp(out);
			mOperandsStack.push(out);
			break; 
		}

		default: {
			RelAssert("Invalid OpCode");
		}
	}
}

void Interpreter::execAll(obj::MethodObject* method, obj::ClassObject* self, obj::DictObject* globals, tp::init_list<GlobalDef> globals2) {
	if (!method->mScript->mBytecode.mInstructions.length()) {
		return;
	}

	exec(method, self, globals, globals2);
	while (!finished()) {
		stepBytecodeIn();
	}
}