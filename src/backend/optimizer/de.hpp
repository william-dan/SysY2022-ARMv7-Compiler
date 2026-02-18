#include "../ssa/build_ssa.hpp"
#include "cdg.hpp"
namespace ssa{
using namespace std;
//simple constant propagation
class DeadcodeElimination{
public:
    DeadcodeElimination(NodeList* nodelist):nodelist(nodelist){
        auto graph = nodelist->front()->mygraph;
        // cout << "cdg" << endl;
        cdg = new ControlDependenceGraph(nodelist, static_cast<cfg::BlockGraph*>(graph)->exit);
        // cout << "mask" << endl;
        mask();
        // cout << "eliminate" << endl;
        eliminate();
    }
    void eliminate();
    void mask();
    bool has_side_effect(mir::Stm* stm);
    Node* next_active_node(Node* node, Node* pred);
    NodeList* nodelist;
    ControlDependenceGraph* cdg;
    std::unordered_map<utils::Temp*, mir::Stm*> def_stm;
    std::unordered_map<mir::Stm*, Node*> stm_to_block;
    std::set<mir::Stm*> active_stm;
    std::set<Node*> active_block;
    std::unordered_map<Node*, Node*> node_parent;
    std::unordered_map<Node*, NodeSet> new_succs;
    std::unordered_map<Node*, std::vector<std::pair<Node*, Node*>>> origin_pred_to_new_pred;
    std::set<Node*> visited;
};

}//namespace ssa