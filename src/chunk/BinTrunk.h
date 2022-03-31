#ifndef BINTRUNK_H_
#define BINTRUNK_H_

#include <memory>
#include <mlua.h>
#include <string>
#include <vector>

#define LUA_SIGNATURE "\x1bLua"
#define LUAC_VERSION 0x53
#define LUAC_FORMAT 0
#define LUAC_DATA "\x19\x93\r\n\x1a\n"
#define CINT_SIZE sizeof(CInt)
#define CSIZE_SIZE sizeof(CSize)
#define INSTRUCTION_SIZE 4
#define LUA_INTEGER_SIZE sizeof(LInt)
#define LUA_NUMBER_SIZE sizeof(LNumber)
#define LUAC_INT 0x5678
#define LUAC_NUM 370.5

const Byte TAG_NIL = 0x00;
const Byte TAG_BOOLEAN = 0x01;
const Byte TAG_NUMBER = 0x03;
const Byte TAG_INTEGER = 0x13;
const Byte TAG_SHORT_STR = 0x04;
const Byte TAG_LONG_STR = 0x14;

namespace API {
class LuaValue;
}

namespace BinTrunk {
struct Upvalue;
struct Prototype;
struct LocVar;

struct Header {
  Byte signature[4];
  Byte version;
  Byte format;
  Byte luac_data;
  Byte cint_size;
  Byte csize_size;
  Byte instruction_size;
  Byte lua_integer_size;
  Byte lua_number_size;
  LInt luac_int;
  LNumber luac_num;
};

struct BinaryTrunk {
  Header header;
  Byte sizeup_values;
  std::shared_ptr<Prototype> main_func;
};

struct Upvalue {
  Byte instack;
  Byte idx;
};

struct LocVar {
  std::string var_name;
  CInt start_pc;
  CInt end_pc;
};

struct Prototype {
  std::string source;
  CInt line_defined;
  CInt last_line_defined;
  Byte num_params;
  Byte is_var_arg;
  Byte max_stack_size;
  std::vector<CInt> code;
  std::vector<API::LuaValue> constants;
  std::vector<Upvalue> upvalues;
  std::vector<std::shared_ptr<Prototype>> protos;
  std::vector<CInt> line_info;
  std::vector<LocVar> loc_vars;
  std::vector<std::string> upvalue_names;
};

struct Reader {
  Byte *data;

  Reader(Byte *);

  Byte read_byte();
  CInt read_cint();
  CSize read_csize();
  LInt read_lint();
  LNumber read_lnumber();
  std::string read_string();
  const Byte *read_bytes(CSize n);

  void check_header();

  std::shared_ptr<Prototype> read_proto(std::string);
  API::LuaValue read_constant();
  std::vector<API::LuaValue> read_constants();
  std::vector<Upvalue> read_upvalues();
  std::vector<std::shared_ptr<Prototype>> read_protos(std::string);
  std::vector<LocVar> read_loc_vars();
  std::vector<std::string> read_upvalue_names();
  std::vector<CInt> read_code();
  std::vector<CInt> read_line_info();
};

std::shared_ptr<Prototype> undump(Byte *data);
} // namespace BinTrunk

#endif