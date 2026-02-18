#pragma once
#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>
#include "../cfg/graph.hpp"
#include <algorithm>
namespace ssa {
using namespace cfg;

class DominatorTree{
public:
    DominatorTree(NodeList* nodelist)
    :nodelist(nodelist){
        v.resize(nodelist->size());
        for(auto node: *nodelist){
            ancestor[node] = NULL;
            parent[node] = NULL;
            idom[node] = NULL;
            samedom[node] = NULL;
            semi[node] = NULL;
            best[node] = NULL;
        }
        dfnum[NULL] = -1;
        build();
     
        for(auto n: *nodelist){
            dominance_frontier[n] = std::set<Node*>();
            ancestor[n] = n;
        }
        get_dominance_frontier(nodelist->front());
    }

    void build();
    void dfs(Node* p, Node* n);
    void link(Node* p, Node* n);
    Node* awls(Node* n);
    Node* find_root(Node* n);
    void get_dominance_frontier(Node* n);
    void compute_depth(Node* n, int depth);
    
    NodeList* nodelist;
    std::unordered_map<Node*, Node*> parent;
    std::unordered_map<Node*, Node*> ancestor;
    std::unordered_map<Node*, Node*> idom;
    std::unordered_map<Node*, Node*> samedom;
    std::unordered_map<Node*, Node*> semi;
    std::unordered_map<Node*, Node*> best;
    std::unordered_map<Node*, std::set<Node*>> bucket;
    std::unordered_map<Node*, std::set<Node*>> children;
    std::unordered_map<Node*, std::set<Node*>> dominance_frontier;
    std::vector<Node*> v;
    std::unordered_map<Node*, int> dfnum;
    std::unordered_map<Node*, int> domi_depth;
    int count;

};

}