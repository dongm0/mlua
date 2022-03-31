#ifndef EXP_H_
#define EXP_H_
#include <string>
#include <vector>
struct Block;

class Exp {
public:
  virtual int ty() { return 0; }
  virtual ~Exp() {}
};

class NilExp : public Exp {
public:
  int line;
  NilExp(int l) : line(l) {}
};
class TrueExp : public Exp {
public:
  int line;
  TrueExp(int l) : line(l) {}
};
class FalseExp : public Exp {
public:
  int line;
  FalseExp(int l) : line(l) {}
};
class VarargExp : public Exp {
public:
  int line;
  VarargExp(int l) : line(l) {}
};
class IntegerExp : public Exp {
public:
  int line;
  int64_t val;
  IntegerExp(int l, int64_t v) : line(l), val(v) {}
};
class FloatExp : public Exp {
public:
  int line;
  double val;
  FloatExp(int l, int64_t v) : line(l), val(v) {}
};
class StringExp : public Exp {
public:
  int line;
  std::string str;
  StringExp(int l, std::string &v) : line(l), str(v) {}
  StringExp(int l, std::string &&v) : line(l), str(v) {}
};
class NameExp : public Exp {
public:
  int line;
  std::string name;
  NameExp(int l, std::string &v) : line(l), name(v) {}
  NameExp(int l, std::string &&v) : line(l), name(v) {}
};
class UnopExp : public Exp {
public:
  int line;
  int op;
  Exp *exp;
  UnopExp(int l, int o, Exp *e) : line(l), op(o), exp(e) {}
};
class BinopExp : public Exp {
public:
  int line;
  int op;
  Exp *exp1;
  Exp *exp2;
  BinopExp(int l, int o, Exp *e1, Exp *e2)
      : line(l), op(o), exp1(e1), exp2(e2) {}
};
class ConcatExp : public Exp {
public:
  int line;
  std::vector<Exp *> exps;
  ConcatExp(int l, std::vector<Exp *> &v) : line(l), exps(v) {}
};

class TableConstructExp : public Exp {
public:
  int line;
  int last_line;
  std::vector<Exp *> key_exps;
  std::vector<Exp *> val_exps;
  TableConstructExp(int l, int ll, std::vector<Exp *> &k, std::vector<Exp *> &v)
      : line(l), last_line(ll), key_exps(k), val_exps(v) {}
};

class FunDefExp : public Exp {
public:
  int line;
  int last_line;
  std::vector<std::string> par_list;
  bool is_vararg;
  Block *block;
  FunDefExp(int l, int ll, std::vector<std::string> &pl, bool b, Block *bl)
      : line(l), last_line(ll), par_list(pl), is_vararg(b), block(bl) {}
};

class ParensExp : public Exp {
public:
  Exp *exp;
  ParensExp(Exp *e) : exp(e) {}
};

class TableAccessExp : public Exp {
public:
  int last_line;
  Exp *prefix_exp;
  Exp *key_exp;
  TableAccessExp(int ll, Exp *pfx, Exp *e)
      : last_line(ll), prefix_exp(pfx), key_exp(e) {}
};

class FuncCallExp : public Exp {
public:
  int line;
  int last_line;
  Exp *prefix_exp;
  StringExp *name_exp;
  std::vector<Exp *> args;
  FuncCallExp(int l, int ll, Exp *pfx, StringExp *name, std::vector<Exp *> a)
      : line(l), last_line(ll), prefix_exp(pfx), name_exp(name), args(a) {}
};

#endif