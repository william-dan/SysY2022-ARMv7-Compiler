
#include "../ssa/build_ssa.hpp"

namespace ssa{

//simple constant propagation
class ControlDependenceGraph: public cfg::Graph{
public:
    ControlDependenceGraph(NodeList* nodelist, Node* exitnode)
    :nodelist(nodelist)
    ,exitnode(exitnode){
        build();
    }
    void build();
    void show();

    NodeList* nodelist;
    Node* entrynode;
    Node* exitnode;
    std::unordered_map<Node*, Node*> nodemap;   
    std::unordered_map<Node*, Node*> cdg_to_bg;   
};

void test_cdg();
}//namespace ssa