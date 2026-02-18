#pragma once
#include <iostream>
#include "graph.hpp"
#include "../../structure/mir/mir.hpp"
#include <unordered_set>

namespace cfg {
using std::list;

class BlockGraph;
class BGinfo;

class BlockGraph: public Graph {
public:
    BlockGraph(mir::Block* block);
    mir::StmList* get_info(Node* n);
    void show();
    mir::Block* toBlock();
    void deadnode_eliminate();


    void Liveness();
    void showLiveness();
    void clearLiveness();
    unordered_map<Node*, BGinfo*> liveness_info;


    void cut_edge(Node* old_n, Node* new_n);
    Node* node_deepcopy(Node* proto);
    utils::Label* get_node_label(Node* n);

    void merge_node();
    void realmerge(Node* a, Node* b);
    void detach_cond();
private:
    void dfs_bg(Node* n);
    Node* lookup(mir::StmList* b);
    void enter(mir::StmList* b1, mir::StmList* b2);

    std::unordered_set<Node*> done;
};

class BGinfo {
public:
    utils::TempSet *uses, *defs;
    utils::TempSet *in, *out;
    BGinfo(){}
    BGinfo(utils::TempSet* uses, utils::TempSet* defs);
    ~BGinfo(){
        delete uses;
        delete defs;
        delete in;
        delete out;
    }
};


};