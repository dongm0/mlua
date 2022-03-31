#ifndef BLOCK_H_
#define BLOCK_H_
#include "Exp.h"
#include "Stat.h"
#include <string>
#include <vector>

struct Block {
  int last_line;
  std::vector<Stat *> stats;
  std::vector<Exp *> ret_exps;
};

#endif