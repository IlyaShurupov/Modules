
#include "stack.h"

#include "compiler/function.h"

#include "primitives/primitives.h"
#include "primitives/methodobject.h"

#include "log.h"

#include "parser/parser.h"

using namespace obj;
using namespace tp;
using namespace obj;
using namespace BCgen;

void obj::BCgen::Genereate(ByteCode& out, StatementScope* body) {
	auto root = FunctionDefinition();

	root.inst(Instruction(OpCode::SCOPE_IN));

	for (auto child_stm : body->mStatements) {
		root.EvalStatement(child_stm.data());
	}

	root.inst(Instruction(OpCode::SCOPE_OUT));

	root.generateByteCode(out);
}

ConstObject* FunctionDefinition::defineLocal(tp::string id) {
	auto idx = mLocals.presents(id);
	//RelAssert(!idx && "Local Redefinition");
	auto const_str_id = mConstants.get(id);
	mLocals.put(id, const_str_id);
	return const_str_id;
}

FunctionDefinition::FunctionDefinition(tp::string function_id, tp::Array<tp::string> args, FunctionDefinition* prnt) {
	mFunctionId = function_id;
	inst(Instruction(OpCode::SAVE_ARGS, args.length(), 1));
	for (auto id : args) {
		RelAssert(!mLocals.presents(id.data()) && "Argument Redefinition");
		auto const_data = mConstants.get(id.data());
		mArgsOrder.pushBack(const_data);
		mLocals.put(id.data(), const_data);
		inst(Instruction(const_data));
	}
}

void FunctionDefinition::EvalStatement(Statement* stm) {
	switch (stm->mType) {
		case Statement::Type::IGNORE: {
			auto stm_ignore = (StatementIgnore*)stm;
			EvalExpr(stm_ignore->mExpr);
			inst(OpCode::IGNORE);
			break;
		}
		case Statement::Type::WHILE: {
			auto stm_while = (StatementWhile*)stm;

			auto check_mark = inst(Instruction());

			EvalExpr(stm_while->mCondition);

			auto jump_if_inst = inst(Instruction(NULL, Instruction::InstType::JUMP_IF_NOT));

			if (stm_while->mScope) {
				EvalStatement(stm_while->mScope);
			}

			auto jump_inst = inst(Instruction(NULL, Instruction::InstType::JUMP));
			auto end_mark = inst(Instruction());

			jump_if_inst->data.mInstTarget = end_mark;
			jump_inst->data.mInstTarget = check_mark;

			break;
		}
		case Statement::Type::IF: {
			auto stm_if = (StatementIf*)stm;

			EvalExpr(stm_if->mCondition);

			auto jump_if_inst = inst(Instruction(NULL, Instruction::InstType::JUMP_IF_NOT));

			if (stm_if->mOnTrue) {
				EvalStatement(stm_if->mOnTrue);
			}

			auto jump_inst = inst(Instruction(NULL, Instruction::InstType::JUMP));
			auto else_mark = inst(Instruction());

			if (stm_if->mOnFalse) {
				EvalStatement(stm_if->mOnFalse);
			}

			auto end_mark = inst(Instruction());

			jump_if_inst->data.mInstTarget = else_mark;
			jump_inst->data.mInstTarget = end_mark;

			break;
		}
		case Statement::Type::SCOPE: {
			auto stm_scope = (StatementScope*)stm;
			if (stm_scope->mPushToScopeStack) {
				inst(Instruction(OpCode::SCOPE_IN));
			}
			for (auto child_stm : stm_scope->mStatements) {
				EvalStatement(child_stm.data());
			}
			if (stm_scope->mPushToScopeStack) {
				inst(Instruction(OpCode::SCOPE_OUT));
			}
			break;
		}
		case Statement::Type::DEF_FUNC: {
			auto stm_func_def = (StatementFuncDef*) stm;

			FunctionDefinition func(stm_func_def->mFunctionId, stm_func_def->mArgs, this);

			// define method as local
			auto idx = mLocals.presents(func.mFunctionId);
			RelAssert(!idx && "Local Redefinition with function name");
			mLocals.put(func.mFunctionId, mConstants.get(func.mFunctionId));

			// create and register const func object
			auto function_obj = NDO_CAST(MethodObject, NDO->create("method"));
			auto method_const_obj = mConstants.addMethod(func.mFunctionId, function_obj);

			for (auto child_stm : stm_func_def->mStatements) {
				func.EvalStatement(child_stm.data());
			}

			func.generateByteCode(function_obj->mScript->mBytecode);

			inst(Instruction(OpCode::LOAD_CONST, method_const_obj));
			inst(Instruction(OpCode::LOAD_CONST, mConstants.get(func.mFunctionId)));
			inst(Instruction(OpCode::DEF_LOCAL));
			break;
		}
		case Statement::Type::CLASS_DEF: {

			// do the function definition
			auto stm_class_def = (StatementClassDef*)stm;

			FunctionDefinition func(stm_class_def->mClassId, {}, this);

			// define method as local
			auto idx = mLocals.presents(func.mFunctionId);
			RelAssert(!idx && "Local Redefinition with function name");
			mLocals.put(func.mFunctionId, mConstants.get(func.mFunctionId));

			// create and register const func object
			auto function_obj = NDO_CAST(MethodObject, NDO->create("method"));
			auto method_const_obj = mConstants.addMethod(func.mFunctionId, function_obj);

			// compile function
			for (auto child_stm : stm_class_def->mScope->mStatements) {
				// check for return statements
				RelAssert(child_stm.data()->mType != Statement::Type::RET 
					&& "return statements are not allowed in class definition");

				func.EvalStatement(child_stm.data());
			}
			// create one last instruction - constructing class from function execution state
			func.inst(Instruction(OpCode::CLASS_CONSTRUCT));
			func.generateByteCode(function_obj->mScript->mBytecode);

			inst(Instruction(OpCode::LOAD_CONST, method_const_obj));
			inst(Instruction(OpCode::LOAD_CONST, mConstants.get(func.mFunctionId)));
			inst(Instruction(OpCode::DEF_LOCAL));
			break;
		}
		case Statement::Type::DEF_LOCAL: {
			auto stm_local_def = (StatementLocalDef*)stm;

			if (!stm_local_def->mIsConstExpr) {
				auto const_id = defineLocal(stm_local_def->mLocalId);
				EvalExpr(stm_local_def->mNewExpr);
				inst(Instruction(OpCode::LOAD_CONST, const_id));
				inst(Instruction(OpCode::DEF_LOCAL));

			} else {

				tp::string type;
				switch (stm_local_def->mConstExpr->mConstType) {
				case ExpressionConst::BOOL: {
					type = "bool";
					break;
				}
				case ExpressionConst::INT: {
					type = "int";
					break;
				}
				case ExpressionConst::FLT: {
					type = "float";
					break;
				}
				case ExpressionConst::STR: {
					type = "str";
					break;
				}
				default: {
					RelAssert(0 && "Cantbe");
				}
				}

				auto new_expr = new ExpressionNew(type);

				EvalStatement(StmDefLocal(stm_local_def->mLocalId, new_expr));
				EvalExpr(stm_local_def->mConstExpr);
				inst(Instruction(OpCode::LOAD_LOCAL, mConstants.get(stm_local_def->mLocalId)));
				inst(Instruction(OpCode::OBJ_COPY));
			}
			break;
		}
		case Statement::Type::COPY: {
			auto stm_cp = (StatementCopy*)stm;
			EvalExpr(stm_cp->mRight);
			EvalExpr(stm_cp->mLeft);
			inst(Instruction(OpCode::OBJ_COPY));
			break;
		}
		case Statement::Type::PRINT: {
			auto stm_prnt = (StatementPrint*)stm;
			EvalExpr(stm_prnt->mTarget);
			inst(Instruction(OpCode::PRINT));
			break;
		}
		case Statement::Type::RET: {
			auto stm_ret = (StatementReturn*)stm;
			if (stm_ret->mRet) {
				EvalExpr(stm_ret->mRet);
				inst(Instruction(OpCode::RETURN_OBJ));
			} else {
				inst(Instruction(OpCode::RETURN));
			}
			break;
		}

		SwitchDefError
	}

	delete stm;
}


void FunctionDefinition::EvalExpr(Expression* expr) {
	switch (expr->mType) {
		case Expression::Type::BOOLEAN: {
			auto boolean = (ExpressionBoolean*)expr;
			if (boolean->mBoolType == ExpressionBoolean::BoolType::NOT) {
				EvalExpr(boolean->mLeft);
				inst(OpCode::NOT);
			} else {
				EvalExpr(boolean->mRight); 
				EvalExpr(boolean->mLeft);
				inst(OpCode(boolean->mBoolType));
			}
			break;
		}
		case Expression::Type::FUNC: {
			auto func = (ExpressionFunc*)expr;

			//RelAssert(mConstants.mMethods.presents(func->mFuncId) && "No such function");
			inst(Instruction(OpCode::LOAD_LOCAL, mConstants.get(func->mFuncId)));
			break;
		}
		case Expression::Type::ARIPHM: {
			auto ariphm = (ExpressionAriphm*)expr;
			EvalExpr(ariphm->mRight);
			EvalExpr(ariphm->mLeft);
			inst(Instruction(ariphm->mOpType));
			break;
		}
		case Expression::Type::NEW: {
			auto create_new = (ExpressionNew*)expr;
			inst(Instruction(OpCode::LOAD_CONST, mConstants.get(create_new->mNewType)));
			inst(Instruction(OpCode::OBJ_CREATE));
			break;
		}
		case Expression::Type::LOCAL: {
			auto local = (ExpressionLocal*)expr;
			// RelAssert(mLocals.presents(local->mLocalId) && "undefined local");
			inst(Instruction(OpCode::LOAD_LOCAL, mConstants.get(local->mLocalId)));
			break;
		}
		case Expression::Type::CONST: {
			auto constobj = (ExpressionConst*)expr;
			switch (constobj->mConstType) {
				case ExpressionConst::STR:	{
					inst(Instruction(OpCode::LOAD_CONST, mConstants.get(constobj->str)));
					break;
				}
				case ExpressionConst::INT: {
					inst(Instruction(OpCode::LOAD_CONST, mConstants.get(constobj->integer)));
					break;
				}
				case ExpressionConst::FLT: {
					inst(Instruction(OpCode::LOAD_CONST, mConstants.get(constobj->floating)));
					break;
				}
				case ExpressionConst::BOOL: {
					inst(Instruction(OpCode::LOAD_CONST, mConstants.get(constobj->boolean)));
					break;
				}
			};
			break;
		}
		case Expression::Type::CHILD: {
			auto child = (ExpressionChild*)expr;
			EvalExpr(child->mParent);
			inst(Instruction(OpCode::LOAD_CONST, mConstants.get(child->mLocalId)));
			inst(Instruction(OpCode::CHILD, child->mMethod, 1));
			break;
		}
		case Expression::Type::SELF: {
			inst(Instruction(OpCode::SELF));
			break;
		}
		case Expression::Type::CALL: {
			auto call = (ExpressionCall*)expr;
			inst(Instruction(OpCode::PUSH_ARGS, call->mArgs.length(), 1));
			for (auto arg : call->mArgs) {
				EvalExpr(arg.data());
			}
			EvalExpr(call->mParent);
			inst(Instruction(OpCode::CALL));
			break;
		}

		SwitchDefError
	}
	delete expr;
}

tp::alni instSize(const Instruction& inst) {
	switch (inst.mInstType) {
		case Instruction::InstType::JUMP:
		case Instruction::InstType::JUMP_IF_NOT:
		case Instruction::InstType::JUMP_IF: {
			// 2 bytes for offset 1 byte for instrtuction
			return 3;
		}
		case Instruction::InstType::PURE_CONST: {
			return 2;
		}
		case Instruction::InstType::EXEC: {
			tp::alni out = 1;
			switch (inst.mArgType) {
				case Instruction::ArgType::PARAM: {
					out += inst.mParamBytes;
					return out;
				}
				case Instruction::ArgType::CONST: {
					out += 2;
					if (inst.mConstData2) {
						out += 2;
					}
					return out;
				}
				case Instruction::ArgType::NO_ARG: {
					return out;
				}
				default: {
					assert(0);
				}
			}
		}
		case Instruction::InstType::NONE: {
			return 0;
		}
		default: {
			assert(0);
		}
	}

	assert(0);
	return 0;
}

void writeConst(ByteCode& out, tp::alni& idx, tp::uint2 data) {
	for (auto byte : tp::Range(sizeof(tp::uint2))) {
		out.mInstructions[idx] = OpCode((tp::int1)(data >> byte * 8));
		idx++;
	}
}

void writeParam(ByteCode& out, tp::alni& idx, tp::int1* data, tp::alni size) {
	for (auto byte : tp::Range(size)) {
		out.mInstructions[idx] = OpCode(data[byte]);
		idx++;
	}
}

tp::alni calcOffset(tp::ListNode<Instruction>* jump_inst, tp::ListNode<Instruction>* to) {
	tp::alni offset = 0;
	bool reversed = jump_inst->data.mInstIdx > to->data.mInstIdx;
	auto iter_node = reversed ? jump_inst : jump_inst->next;
	while (iter_node != to) {
		offset += instSize(iter_node->data);
		iter_node = reversed ? iter_node->prev : iter_node->next;
	}
	if (reversed) {
		offset += instSize(iter_node->data);
	}
	return reversed ? -offset : offset;
}

void FunctionDefinition::generateByteCode(ByteCode& out) {

	out.~ByteCode();
	new (&out) ByteCode();

	mConstants.save(out.mConstants);

	tp::alni inst_len = 0;
	for (auto inst_iter : mInstructions) {
		inst_len += instSize(inst_iter.data());
	}
	out.mInstructions.reserve(inst_len);

	tp::alni idx = 0;
	for (auto inst_iter : mInstructions) {
		auto inst = inst_iter.data();

		switch (inst.mInstType) {
			case Instruction::InstType::JUMP_IF:
			case Instruction::InstType::JUMP_IF_NOT:
			case Instruction::InstType::JUMP: {
				tp::alni offset = calcOffset(inst_iter.node(), inst.mInstTarget);

				if (offset == 0) {
					out.mInstructions[idx] = OpCode::NONE;
					idx += 3;
					break;
				} 
							
				if (inst.mInstType == Instruction::InstType::JUMP_IF) {
					if (offset > 0) {
						out.mInstructions[idx] = OpCode::JUMP_IF;
					}
					else {
						out.mInstructions[idx] = OpCode::JUMP_IF_R;
					}
				}
				else if (inst.mInstType == Instruction::InstType::JUMP_IF_NOT)  {
					if (offset > 0) {
						out.mInstructions[idx] = OpCode::JUMP_IF_NOT;
					}
					else {
						out.mInstructions[idx] = OpCode::JUMP_IF_NOT_R;
					}
				}
				else {
					if (offset > 0) {
						out.mInstructions[idx] = OpCode::JUMP;
					}
					else {
						out.mInstructions[idx] = OpCode::JUMP_R;
					}
				}

				idx++;
				
				tp::alni offset_mod = ABS(offset);
				tp::uint2 offset_param = (tp::uint2)offset_mod;
				writeParam(out, idx, (tp::int1*)&offset_param, 2);
				break;
			}
			case Instruction::InstType::PURE_CONST: {
				writeConst(out, idx, (tp::uint2)inst.mConstData->mConstIdx);
				break;
			}
			case Instruction::InstType::EXEC: {
				out.mInstructions[idx] = inst.mOp;
				idx++;
				switch (inst.mArgType) {
					case Instruction::ArgType::PARAM: {
						writeParam(out, idx, (tp::int1*) &inst.mParam, inst.mParamBytes);
						break;
					}
					case Instruction::ArgType::CONST: {
						writeConst(out, idx, (tp::uint2)inst.mConstData->mConstIdx);
						if (inst.mConstData2) {
							writeConst(out, idx, (tp::uint2)inst.mConstData2->mConstIdx);
						}
						break;
					}
					case Instruction::ArgType::NO_ARG: {
						break;
					}
					default:{
						assert(0);
					}
				}
				break;
			}
			default: {
				assert(0);
			}
			case Instruction::InstType::NONE: {}
		}
	}
}

tp::ListNode<Instruction>* FunctionDefinition::inst(Instruction inst) {
	mInstructions.pushBack(inst);
	auto out = &mInstructions.last()->data;
	out->mInstIdx = mInstructions.length() - 1;
	return mInstructions.last();
}

static Parser* sParger = NULL;

void obj::BCgen::init() {
	assert(!sParger);
	if (!sParger) {
		sParger = new Parser();
	}
}

void obj::BCgen::deinit() {
	assert(sParger);
	if (sParger) {
		delete sParger;
	}
}

bool obj::BCgen::Compile(obj::MethodObject* method) {
	
	assert(method);
	assert(sParger);

	if (!sParger) return false;

	auto script = method->mScript->mReadable->val;
	auto res = sParger->parse(script);

	if (res.err) {
		auto loc = res.err->get_err_location(script.cstr());
		tp::GLog->write(tp::sfmt("Parser Error (%i,%i): \n", loc.head, loc.tail), true, tp::Logger::LogEntry::ERR);
		tp::GLog->write(res.err->mDescr, true, tp::Logger::LogEntry::ERR);
		return false;
	}

	BCgen::Genereate(method->mScript->mBytecode, res.scope);

	delete res.scope;

	return true;
}