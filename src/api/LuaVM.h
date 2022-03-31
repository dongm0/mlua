#ifndef LUAVM_H_
#define LUAVM_H_

#include "LuaState.h"
#include <mlua.h>
namespace API {
template <typename _State> class LuaVMTraits {
public:
  static int pc(_State &state) { return state.get_pc(); }
  static void add_pc(_State &state, int n) { return state.add_pc(n); }
  static CInt fetch(_State &state) { return state.fetch(); }
  static void get_const(_State &state, int idx) { return state.get_const(idx); }
  static void get_rk(_State &state, int rk) { return state.get_rk(rk); }
  static int register_count(_State &state) { return state.register_count(); }
  static void load_vararg(_State &state, int n) { return state.load_vararg(n); }
  static void load_proto(_State &state, int idx) {
    return state.load_proto(idx);
  }
};
} // namespace API

#endif