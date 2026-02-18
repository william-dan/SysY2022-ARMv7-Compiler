#include "../ssa/build_ssa.hpp"
#include "fi.hpp"
#include <unordered_map>

namespace ssa{
using namespace std;
//1. 哈希所有的右值表达式，还要对全局变量特别哈希
//  a. 从前往后做，对于前面已经变化过的，后面要先对右值展开再做
//  b. 

class GlobalValueNumber{
public:
GlobalValueNumber(
    cfg::BlockGraph* ssa_ir, 
    unordered_map<string, mir::Func_Info*>* func_infos);

private:
void dfs_dtree(Node* curr);
void findavail(mir::StmList::iterator& it, mir::StmList* sl);

utils::Temp* find_root(utils::Temp* curr);
void union_temp(utils::Temp* t1, utils::Temp* t2);


void dfs_dtree_for_mem(Node* curr);
void mem2temp(mir::StmList::iterator& it, mir::StmList* sl);


DominatorTree* dtree;
cfg::BlockGraph* ssa_ir;

unordered_map<string, mir::Func_Info*>* func_infos;

std::unordered_map<utils::Temp*, int> equal_temp_h;
std::unordered_map<utils::Temp*, utils::Temp*> equal_temp;
std::stack<utils::Temp*> equal_stk;

std::unordered_map<string, utils::Temp*> avail;
std::stack<string> avail_stk;

std::unordered_map<string, mir::Exp*> avail_mem;
std::stack<string> avail_mem_stk;
// std::stack<utils::Temp*> mem_temp_stk;

};





}