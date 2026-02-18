#pragma once
#include "../ssa/build_ssa.hpp"
#include "../../utils/temp.hpp"
#include <unordered_set>
#include <set>
#include <stack>
namespace ssa{

class LoopInfo;
class Loop;

class Loop {
public:
    Loop(NodeList* ir);
    void Unroll();
    void compute_temp_score();
    

    void show_tree();
    void show_n2dep();
    void show_LoopInfo(LoopInfo& info);

    NodeList* ir;
    std::unordered_map<Node*, std::unordered_set<Node*>> loops;
    std::unordered_map<Node*, std::unordered_set<Node*>> tree;
    std::unordered_map<Node*, int> n2dep;
    std::unordered_map<utils::Temp*, int> temp_score;


    std::unordered_map<Node*, std::unordered_set<Node*>> cond;
    std::unordered_set<Node*> done;
    std::unordered_map<Node*, Node*> parent;
private:
    void get_parent(Node* n);
    void build_tree();
    void get_dep(Node* curr, int dep);
    
    int unroll_times = 5;
    LoopInfo analyse(Node* h);
    Node* add_preheader(LoopInfo& info);
    void loopUnroll(LoopInfo& info);
    mir::Exp* find_circle(utils::Temp* t, LoopInfo& info);
    Node* find_preheader(Node* h);
    Node* find_single_exit(Node* h);
    Node* find_final(Node* h);
};


// step = 1, 
class LoopInfo{
public:
    Node* head;
    bool canUnroll;
    bool preheader_added;
    mir::Exp *step, *times;
    utils::Temp* ini_iter;
    std::unordered_set<utils::Temp*> induce_temp;
    std::unordered_set<utils::Temp*> var, invar;
    std::unordered_map<utils::Temp*, std::unordered_set<utils::Temp*>> def_use;
    std::unordered_map<utils::Temp*, mir::Exp*> def_exp;

    LoopInfo():
    head(nullptr), canUnroll(false), 
    preheader_added(false), step(nullptr),
    times(nullptr), ini_iter(nullptr){}
};




};