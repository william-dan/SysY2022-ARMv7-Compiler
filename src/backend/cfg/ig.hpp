#pragma once
#include <iostream>
#include <unordered_set>
#include "graph.hpp"
#include "flowgraph.hpp"
#include "../../utils/temp.hpp"

namespace cfg{


class IG: public Graph {
public:
    std::unordered_map<utils::Temp*, Node*> t2n;
    IG(FlowGraph* fg);
    ~IG();
    void addEdge_hide(Node* from, Node* to);
    void rmEdge_hide(Node* from, Node* to);
    void addEdge_move(Node* n1, Node* n2);
    void rmEdge_move(Node* n1, Node* n2);
    void hideEdge(Node* n);
    bool isRegister(Node* n);

    Node* lookup(utils::Temp* t);
    void enter(utils::Temp* t1, utils::Temp* t2);
    void show();
};

class IGInfo{
public:
    utils::TempList* temps;
    // int color;
    NodeSet* hide_edges;
    NodeSet* mov_targets;
    // int spill_offset;
    IGInfo(utils::TempList* temps): temps(temps){
        mov_targets = new NodeSet();
        hide_edges = new NodeSet();
        // color = spill_offset = -1;
    }
    ~IGInfo(){
        delete temps;
        delete hide_edges;
        delete mov_targets;
    }
};



};