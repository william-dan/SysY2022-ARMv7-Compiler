#include <iostream>
#include <unordered_set>
#include "graph.hpp"
#include "../../structure/assem/assem.hpp"

namespace cfg{


class A_BlockGraph: public Graph{
public:
    A_BlockGraph(assem::InstrList* il);
    ~A_BlockGraph();
    void show();
    void dfs_abg(Node* n);
    void deadnode_eliminate();
    std::unordered_set<Node*> done;
};


};