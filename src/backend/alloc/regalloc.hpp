#include "../../structure/assem/assem.hpp"
#include "../cfg/ig.hpp"
#include "color.hpp"
#include "../optimizer/loop.hpp"
#include <set>
#include <unordered_set>
#include <stack>
namespace alloc{


class Allocator{
public:
    Allocator(bool is_recursive): is_recursive(is_recursive){}
    assem::InstrList* reg_alloc(assem::InstrList* il);
protected:
    assem::InstrList* real_spill(assem::InstrList* instrlist, utils::TempType temptype, ColorResult& result);
    assem::InstrList* rename_temp(cfg::IG* ig, assem::InstrList* instrlist, utils::TempType temptype, ColorResult& result);
    utils::TempType temptype;
private:
    bool is_recursive;
    // std::unordered_map<assem::Instr*, std::pair<assem::InstrList::iterator, assem::InstrList::iterator>> ipos;
};
}//namespace alloc