#include "../ssa/build_ssa.hpp"
#include "loop.hpp"
namespace ssa{

class GlobalCodeMotion
{
public:
    NodeList* nodelist;
    unordered_map<Node*, int> domi_depth;
    unordered_map<Node*, int> loop_depth;
    std::unordered_map<utils::Temp*, mir::Stm*> def_stm;
    std::unordered_map<utils::Temp*, mir::StmList> use_stms;
    std::set<mir::Stm*> visited;
    std::set<mir::Stm*> array_init;
    std::unordered_map<mir::Stm*, Node*> stm_to_block;
    DominatorTree* domi_tree;
public:
    GlobalCodeMotion(NodeList* nodelist);
    bool has_side_effect(mir::Stm* stm);
    Node* schedule_early(mir::Stm* stm);
    Node* schedule_late(mir::Stm* stm);
    Node* lca(Node* a, Node* b);
    ~GlobalCodeMotion(){
    };
};

}