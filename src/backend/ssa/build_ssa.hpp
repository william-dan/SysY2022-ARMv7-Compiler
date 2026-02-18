#pragma once
#include "domi_tree.hpp"
#include "../../utils/temp.hpp"
#include "../cfg/bg.hpp"
#include <stack>
namespace ssa{
using TempDefTable = std::unordered_map<Node*, std::set<utils::Temp*>>;
using TempSites = std::unordered_map<utils::Temp*, NodeList>;
using TempStack = std::unordered_map<utils::Temp*, std::stack<utils::Temp*>>;
class Ssa{
    public:
    Ssa(NodeList* nodelist, utils::TempList* fun_args)
    :nodelist(nodelist)
    ,fun_args(fun_args){
        domi_tree = new DominatorTree(nodelist);
        for(auto& node: *nodelist){
            get_origin_def(node);
        }
        place_phi();
        auto node = nodelist->front();
        auto graph = static_cast<cfg::BlockGraph*>(node->mygraph);
        rename();
    }
    utils::TempList* get_origin_def(Node* n);
    void place_phi();
    void rename();
    void rename_single_node(Node* n);
    void back_to_ir();
    void remove_empty();
    utils::TempList* fun_args;
    NodeList* nodelist;
    DominatorTree* domi_tree;
    TempDefTable origin_defs;
    TempSites defsites;
    std::set<utils::Temp*> usesites;
    TempStack stacks;
    std::set<Node*> done;
};

mir::Exp* get_def(mir::Stm* stm);
mir::ExpList get_uses(mir::Stm* stm);   
bool is_a_phi(mir::Stm* stm);
}