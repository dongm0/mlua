#ifndef LUASTATE_H_
#define LUASTATE_H_
#include <api/LuaConsts.h>
#include <api/LuaVM.h>
#include <api/core/LuaValue.h>
#include <chunk/BinTrunk.h>
#include <functional>
#include <memory>
#include <vector>

namespace API {
class LuaValue;
class LuaClosure;
class LuaStack;
class LuaState;

using CFunction = std::function<int(LuaState &)>;
class LuaState {
private:
  std::vector<std::shared_ptr<LuaStack>> _frame;

private:
  LuaValue registry;

public:
  LuaState();
  // LuaState(int stack_size, std::shared_ptr<BinTrunk::Prototype> proto);
  std::weak_ptr<LuaStack> cur_stack();

  // stack
  int get_top();
  int abs_index(int idx);
  bool check_stack(int n);
  void pop(int n);
  void copy(int from_idx, int to_idx);
  void push_value(int idx);
  void replace(int idx);
  void insert(int idx);
  void remove(int idx);
  void rotate(int idx, int n);
  void set_top(int idx);

  // access
  std::string_view type_name(LValueType tp);
  LValueType type(int idx);
  bool is_none(int idx);
  bool is_nil(int idx);
  bool is_none_or_nil(int idx);
  bool is_boolean(int idx);
  bool is_integer(int idx);
  bool is_number(int idx);
  bool is_string(int idx);
  bool is_c_function(int idx);
  bool to_boolean(int idx);
  LInt to_integer(int idx);
  std::pair<LInt, bool> to_integer_x(int idx);
  LNumber to_number(int idx);
  std::pair<LNumber, bool> to_number_x(int idx);
  std::string to_string(int idx);
  std::pair<std::string, bool> to_string_x(int idx);
  CFunction to_c_function(int idx);

  // push
  void push_nil();
  void push_boolean(bool b);
  void push_integer(int64_t n);
  void push_number(double n);
  void push_string(char *s);
  void push_c_function(CFunction f);
  void push_global_table();

  // arith & compare
  void arith(ArithOp op);
  bool compare(int idx1, int idx2, CompareOp op);

  // misc
  void len(int idx);
  void concat(int n);

  // get
  void new_table();
  void create_table(int n_arr, int n_rec);
  LValueType get_table(int idx);
  LValueType get_field(int idx, std::string &k);
  LValueType get_i(int idx, LInt i);
  LValueType get_global(std::string &name);

  // set
  void set_table(int idx);
  void set_field(int idx, std::string &k);
  void set_i(int idx, LInt n);
  void set_global(std::string &name);
  void state_register(std::string &name, CFunction f);

  // call
  int load(Byte *chunk, std::string &chunk_name, std::string &name);
  void call(int n_args, int n_results);

private:
  void _call_lua_closure(int n_args, int n_results, LuaClosure &c);
  void _run_lua_closure();
  void _call_c_function(int n_args, int n_results, LuaClosure &c);

private:
  LValueType _get_table(LuaValue &t, LuaValue &k);
  void _set_table(LuaValue &t, LuaValue &k, LuaValue &v);

  // luavm_trait
public:
  using LuaVmInterface = LuaVMTraits<LuaState>;
  friend class LuaVmInterface;

private:
  // luavm interface
  CSize get_pc();
  void add_pc(int n);
  CInt fetch();
  void get_const(int idx);
  void get_rk(int rk);
  int register_count();
  void load_vararg(int n);
  void load_proto(int idx);

  // frame
  void pop_lua_stack();
  void push_lua_stack(std::shared_ptr<LuaStack> stack);

public:
};
} // namespace API

#endif