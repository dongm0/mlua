#ifndef LUACONSTS_H_
#define LUACONSTS_H_

#include <cstring>
#include <functional>
#include <mlua.h>
#include <stdexcept>

namespace API {
class LuaState;

const int LUA_MINSTACK = 20;
const int LUAI_MAXSTACK = 1000000;
const int LUA_REGISTRYINDEX = -LUAI_MAXSTACK - 1000;
const int64_t LUA_RIDX_GLOBALS = 2;
using CFunction = std::function<int(LuaState &)>;
enum class ArithOp {
  LUA_OPADD,
  LUA_OPSUB,
  LUA_OPMUL,
  LUA_OPMOD,
  LUA_OPPOW,
  LUA_OPDIV,
  LUA_OPIDIV,
  LUA_OPBAND,
  LUA_OPBOR,
  LUA_OPBXOR,
  LUA_OPSHL,
  LUA_OPSHR,
  LUA_OPUNM,
  LUA_OPBNOT
};

enum class CompareOp { LUA_OPEQ, LUA_OPLT, LUA_OPLE };

enum class CValueType {
  C_TNONE = -1,
  C_TNIL,
  C_TBOOLEAN,
  C_TLIGHTUSERDATA,
  C_TINT,
  C_TREAL,
  C_TTABLE,
  C_TFUNCTION,
  C_TSTRING,
  C_TUSERDATA,
  C_TTHREAD
};

enum class LValueType {
  LUA_TNONE = -1,
  LUA_TNIL,
  LUA_TBOOLEAN,
  LUA_TLIGHTUSERDATA,
  LUA_TNUMBER,
  LUA_TTABLE,
  LUA_TFUNCTION,
  LUA_TSTRING,
  LUA_TUSERDATA,
  LUA_TTHREAD
};
} // namespace API

#endif