#pragma once
#include <iostream>
#include "../../structure/assem/assem.hpp"
#include "../cfg/ig.hpp"
#include "../optimizer/loop.hpp"
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stack>
namespace alloc{

struct ColorResult{
    std::unordered_map<utils::Temp*, int>& color_table;
    std::unordered_map<utils::Temp*, int>& spill_table;
    ColorResult(
        std::unordered_map<utils::Temp*, int>& color_table,
        std::unordered_map<utils::Temp*, int>& spill_table
    ): color_table(color_table), spill_table(spill_table){}
};

class Color{
public:
    cfg::IG* ig;
    utils::TempType temptype;
    bool ignore_mov;
    int k;
    static int real_spill_n;
    std::vector<int> reg;
    const std::vector<int> int_reg = {0,1,2,3,14,4,5,6,7,8,9,10,12};
    const std::vector<int> float_reg = 
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

    Color(cfg::IG* ig, utils::TempType temptype, bool ignore_mov);
    ~Color();
    ColorResult color();
private:
    void print_stage(std::string name, cfg::IGInfo* info);

    void make_worklist();
    void change_worklist(cfg::Node* n);
    void simplify();
    void coalesce();
    void freeze();
    void spill();
    void assignColor();

    void freeze_move(cfg::Node* n);

    bool Brigg_strategy(cfg::Node* x, cfg::Node* y);
    bool George_strategy(cfg::Node* x, cfg::Node* y);
    void reg_coal(cfg::Node* x, cfg::Node* y);
    double compute_potential_spill_priority(cfg::Node* n);
    
    std::stack<cfg::Node*> select_stack;
    
    std::unordered_map<utils::Temp*, int> color_table;
    std::unordered_map<utils::Temp*, int> spill_table;

    
    
    cfg::NodeSet simplify_worklist;
    cfg::NodeSet coalesce_worklist;
    cfg::NodeSet freeze_worklist;
    cfg::NodeSet spill_worklist;
};


};