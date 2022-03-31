#include <api/LuaState.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaTable.h>
#include <api/core/LuaValue.h>

namespace API {
void LuaState::len(int idx) {
  auto &val = cur_stack().lock()->get(idx);
  if (val.value_type == CValueType::C_TSTRING) {
    cur_stack().lock()->push(
        LuaValue((LInt)(*(std::string *)(val.val_ptr)).size()));
  } else if (val.value_type == CValueType::C_TTABLE) {
    cur_stack().lock()->push(LuaValue((*(LuaTable *)(val.val_ptr)).len()));
  } else {
    throw std::runtime_error("length error!");
  }
}

void LuaState::concat(int n) {
  if (n == 0) {
    cur_stack().lock()->push(std::string(""));
  } else if (n >= 2) {
    for (int i = 1; i < n; ++i) {
      if (is_string(-1) && is_string(-2)) {
        auto s2 = this->to_string(-1);
        auto s1 = this->to_string(-2);
        cur_stack().lock()->pop();
        cur_stack().lock()->pop();
        cur_stack().lock()->push(LuaValue(s1 + s2));
        continue;
      }
      throw std::runtime_error("concatenation error!");
    }
  }
}
} // namespace API