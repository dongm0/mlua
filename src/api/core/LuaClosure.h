#ifndef LUACLOSURE_H_
#define LUACLOSURE_H_
#include <functional>
#include <memory>
#include <mlua.h>

namespace BinTrunk {
struct Prototype;
}

namespace API {
class LuaState;
using CFunction = std::function<int(LuaState &)>;
struct LuaClosure {
  std::shared_ptr<BinTrunk::Prototype> proto;
  CFunction c_function;

  LuaClosure(std::shared_ptr<BinTrunk::Prototype> proto);
  LuaClosure(CFunction f);
  LuaClosure();
  LuaClosure(const LuaClosure &);
  LuaClosure(LuaClosure &&);

  LuaClosure &operator=(const LuaClosure &);
  LuaClosure &operator=(LuaClosure &&);
};
} // namespace API

#endif