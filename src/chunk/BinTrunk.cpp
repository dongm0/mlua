#include "BinTrunk.h"
#include <api/core/LuaValue.h>
#include <cstring>
#include <stdexcept>

namespace {
template <typename T> static T __annomous_read_bin(Byte *&data) {
  auto ret = *reinterpret_cast<T *>(data);
  data += sizeof(T);
  return ret;
}

template <typename T> static T *__annomous_read_bin_ptr(Byte *&data, size_t n) {
  auto ret = reinterpret_cast<T *>(data);
  data += n * sizeof(T);
  return ret;
}
} // namespace

namespace BinTrunk {
Reader::Reader(Byte *d) { data = d; }

Byte Reader::read_byte() { return __annomous_read_bin<Byte>(data); }

CInt Reader::read_cint() { return __annomous_read_bin<CInt>(data); }

CSize Reader::read_csize() { return __annomous_read_bin<CSize>(data); }

LInt Reader::read_lint() { return __annomous_read_bin<LInt>(data); }

LNumber Reader::read_lnumber() { return __annomous_read_bin<LNumber>(data); }

std::string Reader::read_string() {
  size_t size = (size_t)this->read_byte();
  if (size == 0) {
    return std::string("");
  }

  if (size == 0xff) {
    size = (size_t)this->read_csize();
  }
  auto byte = this->read_bytes(size - 1);
  return std::string((char *)byte, size - 1);
}

const Byte *Reader::read_bytes(CSize n) {
  return __annomous_read_bin_ptr<Byte>(data, n);
}

void Reader::check_header() {
  if (std::string((char *)read_bytes(4), 4) != LUA_SIGNATURE) {
    throw std::runtime_error("not a precomplied chunk!");
  } else if (read_byte() != LUAC_VERSION) {
    throw std::runtime_error("version dismatch!");
  } else if (read_byte() != LUAC_FORMAT) {
    throw std::runtime_error("format mismatch!");
  } else if (std::string((char *)read_bytes(6), 6) != LUAC_DATA) {
    throw std::runtime_error("corrupted!");
  } else if (read_byte() != CINT_SIZE) {
    throw std::runtime_error("int size mismatch!");
  } else if (read_byte() != CSIZE_SIZE) {
    throw std::runtime_error("size_t size mismatch!");
  } else if (read_byte() != INSTRUCTION_SIZE) {
    throw std::runtime_error("instruction size mismatch!");
  } else if (read_byte() != LUA_INTEGER_SIZE) {
    throw std::runtime_error("lua_integer size mismatch!");
  } else if (read_byte() != LUA_NUMBER_SIZE) {
    throw std::runtime_error("lua number size mismatch!");
  } else if (read_lint() != LUAC_INT) {
    throw std::runtime_error("endianness mismatch!");
  } else if (read_lnumber() != LUAC_NUM) {
    throw std::runtime_error("float format mismatch!");
  }
}

std::shared_ptr<Prototype> Reader::read_proto(std::string parent_source) {
  auto src = read_string();
  if (src == "") {
    src = parent_source;
  }
  return std::make_shared<Prototype>(Prototype{
      src,
      read_cint(),
      read_cint(),
      read_byte(),
      read_byte(),
      read_byte(),
      read_code(),
      read_constants(),
      read_upvalues(),
      read_protos(src),
      read_line_info(),
      read_loc_vars(),
      read_upvalue_names(),
  });
}

std::vector<CInt> Reader::read_code() {
  auto len = read_cint();
  std::vector<CInt> res(len);
  for (size_t i = 0; i < len; ++i) {
    res[i] = read_cint();
  }
  return res;
}

std::vector<API::LuaValue> Reader::read_constants() {
  auto len = read_cint();
  std::vector<API::LuaValue> res(len);
  for (size_t i = 0; i < len; ++i) {
    res[i] = read_constant();
  }
  return res;
}

API::LuaValue Reader::read_constant() {
  auto tag = read_byte();
  API::LuaValue var;
  std::string _str;
  switch (tag) {
  case TAG_NIL:
    break;
  case TAG_BOOLEAN:
    var = API::LuaValue((read_byte() == 1));
    break;
  case TAG_INTEGER:
    var = API::LuaValue(read_lint());
    break;
  case TAG_NUMBER:
    var = API::LuaValue(read_lnumber());
    break;
  case TAG_SHORT_STR:
    var = API::LuaValue(std::move(read_string()));
    break;
  case TAG_LONG_STR:
    var = API::LuaValue(std::move(read_string()));
    break;
  default:
    throw std::runtime_error("corrupted!");
    break;
  }
  return std::move(var);
}

std::vector<Upvalue> Reader::read_upvalues() {
  auto len = read_cint();
  std::vector<Upvalue> res(len);
  for (size_t i = 0; i < len; ++i) {
    res[i] = Upvalue{read_byte(), read_byte()};
  }
  return res;
}

std::vector<std::shared_ptr<Prototype>>
Reader::read_protos(std::string parent_source) {
  auto len = read_cint();
  std::vector<std::shared_ptr<Prototype>> res(len);
  for (size_t i = 0; i < len; ++i) {
    res[i] = read_proto(parent_source);
  }
  return res;
}

std::vector<CInt> Reader::read_line_info() {
  auto len = read_cint();
  std::vector<CInt> res(len);
  for (size_t i = 0; i < len; ++i) {
    res[i] = read_cint();
  }
  return res;
}

std::vector<LocVar> Reader::read_loc_vars() {
  auto len = read_cint();
  std::vector<LocVar> res(len);
  for (size_t i = 0; i < len; ++i) {
    res[i] = LocVar{read_string(), read_cint(), read_cint()};
  }
  return res;
}

std::vector<std::string> Reader::read_upvalue_names() {
  auto len = read_cint();
  std::vector<std::string> res(len);
  for (size_t i = 0; i < len; ++i) {
    res[i] = read_string();
  }
  return res;
}

std::shared_ptr<Prototype> undump(Byte *data) {

  Reader reader(data);
  reader.check_header();
  reader.read_byte();
  return reader.read_proto("");
}
}