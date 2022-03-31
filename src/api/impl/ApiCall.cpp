#include <api/LuaState.h>
#include <api/core/LuaClosure.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaTable.h>
#include <api/core/LuaValue.h>
#include <vm/OpCodes.h>

namespace API {
int LuaState::load(Byte *chunk, std::string &chunk_name, std::string &name) {
  auto proto = BinTrunk::undump(chunk);
  auto c = LuaClosure(proto);
  cur_stack().lock()->push(LuaValue(c));
  return 0;
}

void LuaState::call(int n_args, int n_results) {
  auto val = cur_stack().lock()->get(-(n_args + 1));
  if (val.value_type == CValueType::C_TFUNCTION) {
    auto closure = get_void_ptr_type_val<LuaClosure>(val.val_ptr);
    if (!closure.c_function) {
      _call_lua_closure(n_args, n_results, closure);
    } else {
      _call_c_function(n_args, n_results, closure);
    }
  } else {
    throw std::runtime_error("not a function!");
  }
}

void LuaState::_call_lua_closure(int n_args, int n_results, LuaClosure &c) {
  int n_regs = c.proto->max_stack_size;
  int n_params = c.proto->num_params;
  auto is_vararg = (c.proto->is_var_arg == 1);
  auto new_stack = std::make_shared<LuaStack>(LuaStack(n_regs + 20, &registry));

  new_stack->closure = c;

  auto func_and_args = cur_stack().lock()->pop_n(n_args + 1);
  auto arg1 =
      std::vector<LuaValue>(func_and_args.begin() + 1, func_and_args.end());
  auto arg2 = std::vector<LuaValue>(func_and_args.begin() + n_params + 1,
                                    func_and_args.end());
  new_stack->push_n(arg1, n_params);
  new_stack->top = n_regs;
  if (n_args > n_params && is_vararg) {
    new_stack->varargs = arg2;
  }

  push_lua_stack(new_stack);
  _run_lua_closure();
  pop_lua_stack();

  if (n_results != 0) {
    auto results = new_stack->pop_n(new_stack->top - n_regs);
    cur_stack().lock()->check(results.size());
    cur_stack().lock()->push_n(results, n_results);
  }
}
void LuaState::_run_lua_closure() {
  while (true) {
    auto inst =
        VM::Instruction<LuaState>(LuaState::LuaVmInterface::fetch(*this));
    inst.execute(*this);
    if (inst.op_code() == (CInt)VM::OpCode::OP_RETURN) {
      break;
    }
  }
}
void LuaState::_call_c_function(int n_args, int n_results, LuaClosure &c) {
  auto new_stack = std::make_shared<LuaStack>(LuaStack(n_args + 20, &registry));
  new_stack->closure = c;

  auto args = cur_stack().lock()->pop_n(n_args);
  new_stack->push_n(args, n_args);
  cur_stack().lock()->pop();

  push_lua_stack(new_stack);
  auto r = c.c_function(*this);
  pop_lua_stack();

  if (n_results != 0) {
    auto results = new_stack->pop_n(r);
    cur_stack().lock()->check(results.size());
    cur_stack().lock()->push_n(results, n_results);
  }
}
} // namespace API