#ifndef STAT_H_
#define STAT_H_

struct Block;

class Stat {
public:
  virtual int ty() { return 0; }
  virtual ~Stat() {}
};
class EmptyStat : public Stat {
public:
};
class BreakStat : public Stat {
public:
  int line;
  BreakStat(int l) : line(l) {}
};
class LabelStat : public Stat {
public:
  std::string name;
  LabelStat(std::string &str) : name(str) {}
  LabelStat(std::string &&str) : name(str) {}
};
class GotoStat : public Stat {
public:
  std::string name;
  GotoStat(std::string &str) : name(str) {}
  GotoStat(std::string &&str) : name(str) {}
};
class DoStat : public Stat {
public:
  Block *block;
  DoStat(Block *b) : block(b) {}
};
class FuncCallStat : public Stat {
public:
  int line;
  int last_line;
  Exp *prefix_exp;
  StringExp *name_exp;
  std::vector<Exp *> args;
  FuncCallStat(int l, int ll, Exp *pfx, StringExp *name, std::vector<Exp *> a)
      : line(l), last_line(ll), prefix_exp(pfx), name_exp(name), args(a) {}
};
class WhileStat : public Stat {
public:
  Exp *exp;
  Block *block;
  WhileStat(Exp *e, Block *b) : exp(e), block(b) {}
};
class RepeatStat : public Stat {
public:
  Exp *exp;
  Block *block;
  RepeatStat(Exp *e, Block *b) : exp(e), block(b) {}
};
class IfStat : public Stat {
public:
  std::vector<Exp *> exps;
  std::vector<Block *> blocks;
  IfStat(std::vector<Exp *> &es, std::vector<Block *> &bs)
      : exps(es), blocks(bs) {}
};
class ForNumStat : public Stat {
public:
  int line_of_for;
  int line_of_do;
  std::string var_name;
  Exp *init_exp;
  Exp *limit_exp;
  Exp *step_exp;
  Block *block;
  ForNumStat(int lof, int lod, std::string &vname, Exp *init_e, Exp *limit_e,
             Exp *step_e, Block *b)
      : line_of_for(lof), line_of_do(lod), var_name(vname), init_exp(init_e),
        limit_exp(limit_e), step_exp(step_e), block(b) {}
  ForNumStat(int lof, int lod, std::string &&vname, Exp *init_e, Exp *limit_e,
             Exp *step_e, Block *b)
      : line_of_for(lof), line_of_do(lod), var_name(vname), init_exp(init_e),
        limit_exp(limit_e), step_exp(step_e), block(b) {}
};
class ForInStat : public Stat {
public:
  int line_of_do;
  std::vector<std::string> name_list;
  std::vector<Exp *> exp_list;
  Block *block;
  ForInStat(int lod, std::vector<std::string> &nl, std::vector<Exp *> &el,
            Block *b)
      : line_of_do(lod), name_list(nl), exp_list(el), block(b) {}
};
class LocalVarDecStat : public Stat {
public:
  int last_line;
  std::vector<std::string> name_list;
  std::vector<Exp *> exp_list;
  LocalVarDecStat(int ll, std::vector<std::string> &nl, std::vector<Exp *> &el)
      : last_line(ll), name_list(nl), exp_list(el) {}
};
class AssignStat : public Stat {
public:
  int last_line;
  std::vector<Exp *> var_list;
  std::vector<Exp *> exp_list;
  AssignStat(int ll, std::vector<Exp *> &vl, std::vector<Exp *> &el)
      : last_line(ll), var_list(vl), exp_list(el) {}
};
class LocalFuncDefStat : public Stat {
public:
  std::string name;
  Exp *func_def_exp;
  LocalFuncDefStat(std::string &n, Exp *f) : name(n), func_def_exp(f) {}
  LocalFuncDefStat(std::string &&n, Exp *f) : name(n), func_def_exp(f) {}
};

#endif