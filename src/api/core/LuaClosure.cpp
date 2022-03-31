#include "LuaClosure.h"
#include <chunk/BinTrunk.h>

namespace API {
LuaClosure::LuaClosure() {}
LuaClosure::LuaClosure(std::shared_ptr<BinTrunk::Prototype> proto) {
  this->proto = proto;
}
LuaClosure::LuaClosure(CFunction f) { this->c_function = f; }
LuaClosure::LuaClosure(const LuaClosure &rhs) {
  this->proto = rhs.proto;
  this->c_function = rhs.c_function;
}
LuaClosure::LuaClosure(LuaClosure &&rhs) {
  this->proto = std::move(rhs.proto);
  this->c_function = std::move(rhs.c_function);
}

LuaClosure &LuaClosure::operator=(const LuaClosure &rhs) {
  this->proto = rhs.proto;
  this->c_function = rhs.c_function;
  return *this;
}
LuaClosure &LuaClosure::operator=(LuaClosure &&rhs) {
  this->proto = std::move(rhs.proto);
  this->c_function = std::move(rhs.c_function);
  return *this;
}

} // namespace API
