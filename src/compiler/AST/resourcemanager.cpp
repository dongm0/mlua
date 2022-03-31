#include "resourcemanager.h"
#include "Block.h"
#include "Exp.h"
#include "Stat.h"
#include <vector>

ResourceManager::~ResourceManager() {
  for (auto &x : _blocks)
    delete x;

  for (auto &x : _stats)
    delete x;

  for (auto &x : _exps)
    delete x;
}
ResourceManager::ResourceManager() {}
ResourceManager &ResourceManager::instance() {
  static ResourceManager m_instance;
  return m_instance;
}

Block *ResourceManager::new_block() {
  auto buf = new Block;
  _blocks.push_back(buf);
  return buf;
}