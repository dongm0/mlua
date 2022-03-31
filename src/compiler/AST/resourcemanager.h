#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <string>
#include <vector>

struct Block;
class Stat;
class EmptyStat;
class BreakStat;
class LabelStat;
class GotoStat;
class DoStat;
class WhileStat;
class RepeatStat;
class IfStat;
class ForNumStat;
class ForInStat;
class LocalVarDecStat;
class AssignStat;
class LocalFuncDefStat;
class FuncCallStat;
class Exp;
class NilExp;
class TrueExp;
class FalseExp;
class VarargExp;
class IntegerExp;
class FloatExp;
class StringExp;
class NameExp;
class UnopExp;
class BinopExp;
class ConcatExp;
class TableConstructExp;
class FunDefExp;
class ParensExp;
class TableAccessExp;
class FuncCallExp;

class ResourceManager {
public:
  ResourceManager(ResourceManager &) = delete;
  ~ResourceManager();
  static ResourceManager &instance();

private:
  ResourceManager();
  std::vector<Block *> _blocks;
  std::vector<Stat *> _stats;
  std::vector<Exp *> _exps;

public:
  Block *new_block();
  template <typename Derived, typename... Args> Stat *new_stat(Args &&...args) {
    Stat *buf = new Derived(std::forward<Args>(args));
    _stats.push_back(buf);
    return
  }
  template <typename Derived, typename... Args> Exp *new_exp(Args &&...args) {
    Exp *buf = new Derived(std::forward<Args>(args));
    _exps.push_back(buf);
    return buf;
  }
};

#endif