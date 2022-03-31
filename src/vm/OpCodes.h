#ifndef OPCODES_H_
#define OPCODES_H_

#include "InstructCall.h"
#include "InstructFor.h"
#include "InstructLoad.h"
#include "InstructMisc.h"
#include "InstructOperator.h"
#include "InstructTable.h"
#include "InstructUpValue.h"
#include "Instruction.h"
#include <api/LuaState.h>
#include <functional>
#include <mlua.h>
#include <string_view>
#include <vector>

namespace VM {

template <typename LVM> struct Instruction;
enum class OpMode {
  IABC,
  IABx,
  IAsBx,
  IAx,
};

enum class OpArgMask { OpArgN, OpArgU, OpArgR, OpArgK };

enum class OpCode {
  OP_MOVE,
  OP_LOADK,
  OP_LOADKX,
  OP_LOADBOOL,
  OP_LOADNIL,
  OP_GETUPVAL,
  OP_GETTABUP,
  OP_GETTABLE,
  OP_SETTABUP,
  OP_SETUPVAL,
  OP_SETTABLE,
  OP_NEWTABLE,
  OP_SELF,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_MOD,
  OP_POW,
  OP_DIV,
  OP_IDIV,
  OP_BAND,
  OP_BOR,
  OP_BXOR,
  OP_SHL,
  OP_SHR,
  OP_UNM,
  OP_BNOT,
  OP_NOT,
  OP_LEN,
  OP_CONCAT,
  OP_JMP,
  OP_EQ,
  OP_LT,
  OP_LE,
  OP_TEST,
  OP_TESTSET,
  OP_CALL,
  OP_TAILCALL,
  OP_RETURN,
  OP_FORLOOP,
  OP_FORPREP,
  OP_TFORCALL,
  OP_TFORLOOP,
  OP_SETLIST,
  OP_CLOSURE,
  OP_VARARG,
  OP_EXTRAARG
};

using ActionFunc =
    std::function<void(Instruction<API::LuaState>, API::LuaState &)>;

template <typename LVM> struct FulOpCode {
  Byte test_flag;
  Byte set_a_flag;
  Byte arg_b_mode;
  Byte arg_c_mode;
  Byte op_mode;
  std::string_view name;

  std::function<void(Instruction<LVM>, LVM &)> action;
};

const std::vector<FulOpCode<API::LuaState>> opcodes = {
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABC,
                             "MOVE    ", move<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABx,
                             "LOADK   ", load_k<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgN,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABx,
                             "LOADKX  ", load_kx<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgU, (Byte)OpMode::IABC,
                             "LOADBOOL", load_bool<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABC,
                             "LOADNIL ", load_nil<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABC,
                             "GETUPVAL"},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "GETTABUP", get_tab_up<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "GETTABLE", get_table<API::LuaState>},
    FulOpCode<API::LuaState>{0, 0, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "SETTABUP"},
    FulOpCode<API::LuaState>{0, 0, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABC,
                             "SETUPVAL"},
    FulOpCode<API::LuaState>{0, 0, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "SETTABLE", set_table<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgU, (Byte)OpMode::IABC,
                             "NEWTABLE", new_table<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "SELF    ", self<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "ADD     ", add<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "SUB     ", sub<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "MUL     ", mul<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "MOD     ", mod<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "POW     ", pow<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "DIV     ", div<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "IDIV    ", idiv<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "BAND    ", band<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "BOR     ", bor<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "BXOR    ", bxor<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "SHL     ", shl<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "SHR     ", shr<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABC,
                             "UNM     ", unm<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABC,
                             "BNOT    ", bnot<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABC,
                             "NOT     ", not <API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABC,
                             "LEN     ", len<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgR, (Byte)OpMode::IABC,
                             "CONCAT  ", concat<API::LuaState>},
    FulOpCode<API::LuaState>{0, 0, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IAsBx,
                             "JMP     ", jmp<API::LuaState>},
    FulOpCode<API::LuaState>{1, 0, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "EQ      ", eq<API::LuaState>},
    FulOpCode<API::LuaState>{1, 0, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "LT      ", lt<API::LuaState>},
    FulOpCode<API::LuaState>{1, 0, (Byte)OpArgMask::OpArgK,
                             (Byte)OpArgMask::OpArgK, (Byte)OpMode::IABC,
                             "LE      ", le<API::LuaState>},
    FulOpCode<API::LuaState>{1, 0, (Byte)OpArgMask::OpArgN,
                             (Byte)OpArgMask::OpArgU, (Byte)OpMode::IABC,
                             "TEST    ", test<API::LuaState>},
    FulOpCode<API::LuaState>{1, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgU, (Byte)OpMode::IABC,
                             "TESTSET ", test_set<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgU, (Byte)OpMode::IABC,
                             "CALL    ", call<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgU, (Byte)OpMode::IABC,
                             "TAILCALL", tail_call<API::LuaState>},
    FulOpCode<API::LuaState>{0, 0, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABC,
                             "RETURN  ", vm_return<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IAsBx,
                             "FORLOOP ", for_loop<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IAsBx,
                             "FORPREP ", for_prep<API::LuaState>},
    FulOpCode<API::LuaState>{0, 0, (Byte)OpArgMask::OpArgN,
                             (Byte)OpArgMask::OpArgU, (Byte)OpMode::IABC,
                             "TFORCALL"},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgR,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IAsBx,
                             "TFORLOOP"},
    FulOpCode<API::LuaState>{0, 0, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgU, (Byte)OpMode::IABC,
                             "SETLIST ", set_list<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABx,
                             "CLOSURE ", closure<API::LuaState>},
    FulOpCode<API::LuaState>{0, 1, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgN, (Byte)OpMode::IABC,
                             "VARARG  ", vararg<API::LuaState>},
    FulOpCode<API::LuaState>{0, 0, (Byte)OpArgMask::OpArgU,
                             (Byte)OpArgMask::OpArgU, (Byte)OpMode::IAx,
                             "EXTRAARG"},
};

} // namespace VM
#endif
