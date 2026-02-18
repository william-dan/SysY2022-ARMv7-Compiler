#include "color.hpp"
#include "../cfg/ig.hpp"
#include <cassert>
#include <string>

namespace alloc{
using namespace std;
using namespace utils;
using namespace cfg;

int Color::real_spill_n = 0;

Color::Color(cfg::IG* ig, utils::TempType temptype, bool ignore_mov):
ig(ig), temptype(temptype), ignore_mov(ignore_mov) {
    switch (temptype)
    {
    case utils::TempType::INT:
    {
        reg = int_reg;
    }
    break;
    case utils::TempType::FLOAT:
    {
        reg = float_reg;
    }
    break;
    default:
        assert(0);
    break;
    }
    k = reg.size();
}
    
Color::~Color(){
    while(!select_stack.empty()) select_stack.pop();
}

ColorResult Color::color(){
    make_worklist();
    while(
        !simplify_worklist.empty() || 
        !coalesce_worklist.empty() ||
        !freeze_worklist.empty() ||
        !spill_worklist.empty()
    ){
        if(!simplify_worklist.empty()){
            simplify();
        }else if(!coalesce_worklist.empty()){
            coalesce();
        }else if(!freeze_worklist.empty()){
            freeze();
        }else if(!spill_worklist.empty()){
            spill();
        }
    }
    assignColor();
    return ColorResult(color_table, spill_table);
}

void Color::make_worklist(){
    for(auto& n: *ig->mynodes){
        auto info = any_cast<IGInfo*>(n->info);
        if(ig->isRegister(n)) continue;
        if(n->degree() / 2 >= k){
            spill_worklist.emplace(n);
        }else if(!info->mov_targets->empty()){
            coalesce_worklist.emplace(n);
        }else{
            simplify_worklist.emplace(n);
        }
    }
}


void Color::change_worklist(cfg::Node* n){
    auto info = any_cast<IGInfo*>(n->info);
    // print_stage("change worklist", info);
    if(ig->isRegister(n)) return;
    if(simplify_worklist.count(n)){
        if(n->degree() / 2 >= k){
            simplify_worklist.erase(n);
            spill_worklist.emplace(n);
        }
    }else if(coalesce_worklist.count(n)){
        if(n->degree() / 2 >= k){
            coalesce_worklist.erase(n);
            spill_worklist.emplace(n);
        }else if(info->mov_targets->empty()){
            coalesce_worklist.erase(n);
            simplify_worklist.emplace(n);
        }
    }else if(freeze_worklist.count(n)){
        if(n->degree() / 2 >= k){
            freeze_worklist.erase(n);
            spill_worklist.emplace(n);
        }else if(info->mov_targets->empty()){
            freeze_worklist.erase(n);
            simplify_worklist.emplace(n);
        }
    }else if(spill_worklist.count(n)){
        if(n->degree() / 2 >= k){
            // do nothing
        }else if(info->mov_targets->empty()){
            spill_worklist.erase(n);
            simplify_worklist.emplace(n);
        }else{
            spill_worklist.erase(n);
            coalesce_worklist.emplace(n);
        }
    }else{
        assert(0);
    }
    // print_stage("change worklist finish", info);
}


void Color::simplify(){
    auto n = *simplify_worklist.begin();
    auto info = any_cast<IGInfo*>(n->info);
    assert(info->mov_targets->size() == 0);
    simplify_worklist.erase(n);
    assert(info->hide_edges->empty());
    // remove from ig
    ig->hideEdge(n);
    // print_stage("simplify", info);
    select_stack.emplace(n);
    for(auto& succ: *info->hide_edges){
        change_worklist(succ);
    }
}
void Color::coalesce(){
    auto n = *coalesce_worklist.begin();
    auto info = any_cast<IGInfo*>(n->info);
    coalesce_worklist.erase(n);
    // print_stage("coalesce", info);
    bool need_freeze = true;
    if(!ignore_mov){
        vector<Node*> mv_tmps;
        for(auto& mov_tmp: *info->mov_targets) mv_tmps.emplace_back(mov_tmp); 

        for(auto& mov_tmp: mv_tmps){
            assert(n != mov_tmp);
            if(ig->goesTo(n, mov_tmp)){
                ig->rmEdge_move(n, mov_tmp);
                change_worklist(mov_tmp);
                continue;
            }
            // assert(!ig->goesTo(n, mov_tmp));
            assert(any_cast<IGInfo*>(mov_tmp->info)->hide_edges->size() == 0);

            if(Brigg_strategy(n, mov_tmp)
            || George_strategy(n, mov_tmp)){
                reg_coal(mov_tmp, n);
                need_freeze = false;
                break;
            }
        }
    }
    if(need_freeze) freeze_worklist.emplace(n);
    
}
void Color::freeze_move(cfg::Node* n){
    auto info = any_cast<IGInfo*>(n->info);
    assert(!ig->isRegister(n));
    // print_stage("freeze", info);
    // assert(info->mov_targets->size());
    auto mv_adj = info->mov_targets;
    while(!mv_adj->empty()){
        auto f = *mv_adj->begin();
        ig->rmEdge_move(n, f);
        change_worklist(f);
    }
}

void Color::freeze(){
    auto n = *freeze_worklist.begin();
    freeze_move(n);
    freeze_worklist.erase(n);
    simplify_worklist.emplace(n);
    
}
void Color::spill(){
    auto n = *spill_worklist.begin();
    // print_stage("spill", any_cast<IGInfo*>(n->info));
    for(auto& node: spill_worklist){
        if(compute_potential_spill_priority(node) > compute_potential_spill_priority(n)){
            n = node;
        }
    }
    freeze_move(n);
    spill_worklist.erase(n);
    simplify_worklist.emplace(n);
}

void Color::assignColor(){
    for(auto& id: reg){
        auto t = utils::Temp::new_Temp(this->temptype, new string(to_string(id)));
        if(ig->t2n.count(t) == 0) continue;
        auto n = ig->t2n[t];
        auto info = any_cast<IGInfo*>(n->info);
        for(auto& temp: *info->temps){
            // cout << "color of " << *temp->id << " is " << id << endl;
            color_table[temp] = id;
        }
    }
    while(!select_stack.empty()){
        auto n = select_stack.top();
        select_stack.pop();
        vector<int> used_color;
        used_color.resize(50);
        auto info = any_cast<IGInfo*>(n->info);
        // print_stage("assign color", info);

        for(auto& adj: *info->hide_edges){
            auto adj_info = any_cast<IGInfo*>(adj->info);
            // print_stage("adj color", adj_info);
            if(spill_table.count(adj_info->temps->front())) continue;
            // if(!color_table.count(adj_info->temps->front())){
            //     cout << adj_info->temps->front()->to_string() << endl;
            // }
            assert(color_table.count(adj_info->temps->front()));
            used_color[color_table[adj_info->temps->front()]] = 1;
        }
        bool real_spill = true;
        for(auto& available_reg: reg){
            if(used_color[available_reg]) continue;
            real_spill = false;
            for(auto& temp: *info->temps) color_table[temp] = available_reg;
            break;
        }
        if(real_spill){
            for(auto& temp: *info->temps) spill_table[temp] = real_spill_n;
            real_spill_n++;
        }
    }
}

bool Color::Brigg_strategy(cfg::Node* x, cfg::Node* y){
    int more_than_k_neigh_n = 0;
    cfg::NodeSet succs_after_coal = cfg::NodeSet(*x->succs);
    for(auto& y_succ: *y->succs) succs_after_coal.emplace(y_succ);
    for(auto& succ: succs_after_coal){
        if(succ->degree() / 2 >= k){
            more_than_k_neigh_n++;
        }   
    }   
    return more_than_k_neigh_n < k;
}
bool Color::George_strategy(cfg::Node* x, cfg::Node* y){
    for(auto& x_succ: *x->succs){
        if(ig->goesTo(x_succ, y) && x_succ->degree() / 2 >= k){
            return false;
        }
    }
    return true;
}
// y coal to x
void Color::reg_coal(cfg::Node* x, cfg::Node* y){
    // cout << "reg coal: " << endl;
    // cout << "\t";
    // print_stage("coalee", any_cast<IGInfo*>(y->info));
    // cout << "\t";
    // print_stage("coaler", any_cast<IGInfo*>(x->info));

    assert(y->succs);
    assert(!ig->isRegister(y));
    for(auto& succ: *y->succs){
        if(succ == x) continue;
        ig->addEdge(succ, x);
        ig->addEdge(x, succ);
    }
    auto y_info = any_cast<IGInfo*>(y->info);
    auto x_info = any_cast<IGInfo*>(x->info);
    assert(y_info->hide_edges->size() == 0);
    for(auto& succ: *y_info->hide_edges){
        if(succ == x) continue;
        ig->addEdge_hide(x, succ);
    }
    for(auto& succ: *y_info->mov_targets){
        if(succ == x) continue;
        if(ig->goesTo(x, succ)) continue;
        ig->addEdge_move(x, succ);
    }
    while(!y->succs->empty()){
        auto bg = *y->succs->begin();
        ig->rmEdge(bg, y);
        ig->rmEdge(y, bg);
    }
    while(!y_info->hide_edges->empty()){
        auto bg = *y_info->hide_edges->begin();
        ig->rmEdge_hide(y, bg);
    }
    while(!y_info->mov_targets->empty()){
        auto bg = *y_info->mov_targets->begin();
        ig->rmEdge_move(y, bg);
    }
    for(auto& temp: *y_info->temps){
        x_info->temps->emplace_back(temp);
        ig->t2n[temp] = x;
    }
}




double Color::compute_potential_spill_priority(cfg::Node* n){
    
    return n->degree();
}

void Color::print_stage(std::string name, cfg::IGInfo* info){
    cout << name << ": ";
    for(auto& t: *info->temps){
        cout << *t->id << "\t";
    }
    cout << endl;
}


};