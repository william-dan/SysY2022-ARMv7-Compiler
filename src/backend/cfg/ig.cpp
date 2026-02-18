#include "ig.hpp"
#include <cassert>

namespace cfg{

IG::IG(FlowGraph* fg){
    bool is_move = false;
    utils::TempSet* use = nullptr;
    for(auto& n: *fg->mynodes){
        for(auto& u: *fg->use(n)) lookup(u);
        for(auto& d: *fg->def(n)) lookup(d);
        if(is_move = fg->isMove(n)){
            use = fg->use(n);
            assert(use && use->size() == 1);
        }else{
            use = nullptr;
        }

        for(auto& def: *fg->def(n)){
            for(auto& out: *fg->FG_Out(n)){
                if((!(is_move && *out == **use->begin())) 
                && (*out != *def)){
                    enter(def, out);
                }
            }
        }
    }
    // show();
    //有向图到无向图
    for(auto& n: *mynodes){
        n->succs = NodeSet_union(n->succs, n->preds);
        n->preds = new NodeSet(*n->succs);
    }
    // show();
    //fp, sp不参与ig
    auto fp_n  = lookup(utils::Temp::new_Temp(utils::TempType::INT, new string("fp")));
    auto sp_n  = lookup(utils::Temp::new_Temp(utils::TempType::INT, new string("sp")));
    while(!fp_n->succs->empty()){
        auto succ = *fp_n->succs->begin();
        rmEdge(fp_n, succ);
        rmEdge(succ, fp_n);
    }
    while(!sp_n->succs->empty()){
        auto succ = *sp_n->succs->begin();
        rmEdge(sp_n, succ);
        rmEdge(succ, sp_n);
    }

    // cout << "init mov" << endl;
    //初始化mov
    for(auto& n: *fg->mynodes){
        // if(n == fp_n || n == sp_n) continue;
        if(fg->isMove(n)){
            auto def = fg->def(n);
            use = fg->use(n);
            assert(def->size() == 1 && use->size() == 1);
            assert(t2n.count(*def->begin()));
            assert(t2n.count(*use->begin()));
            auto dn = t2n[*def->begin()];
            auto sn = t2n[*use->begin()];
            if(dn == sp_n || dn == fp_n || sn == sp_n || sn == fp_n) continue;
            if(goesTo(sn,dn)) continue;
            addEdge_move(dn, sn);
        }
    }
    

}

IG::~IG(){
    for(auto& n: *mynodes){
        auto info = std::any_cast<IGInfo*>(n->info);
        delete info;
    }
}


Node* IG::lookup(utils::Temp* t){
    if(t2n.count(t) > 0){
        return t2n[t];
    }else{
        auto nnode = new Node(this, new IGInfo(new utils::TempList{t}));
        t2n[t] = nnode;
        return nnode;
    }
}


void IG::enter(utils::Temp* t1, utils::Temp* t2){
    Node* n1 = lookup(t1);
    Node* n2 = lookup(t2);
    addEdge(n1, n2);
}
void IG::show(){
    cout << "ig has " << nodecount << " nodes" << endl;
    for(auto& n: *mynodes){
        cout << *std::any_cast<IGInfo*>(n->info)->temps->front()->id << ": ";
        // cout << "succ: ";
        for(auto& succ: *n->succs){
            cout << *std::any_cast<IGInfo*>(succ->info)->temps->front()->id << " ";
        }
        cout << endl;
        // cout << "pred: ";
        // for(auto& pred: *n->preds){
        //     cout << *std::any_cast<utils::Temp*>(pred->info)->id << " ";
        // }
        // cout << endl;
    }
}

void IG::addEdge_hide(Node* from, Node* to){
    auto info1 = any_cast<IGInfo*>(from->info);
    info1->hide_edges->emplace(to);
}
void IG::rmEdge_hide(Node* from, Node* to){
    auto info1 = any_cast<IGInfo*>(from->info);
    info1->hide_edges->erase(to);
}
bool IG::isRegister(Node* n){
    auto info = any_cast<IGInfo*>(n->info);
    assert(info->temps->size() > 0);
    return info->temps->front()->isRegister();
}
void IG::hideEdge(Node* n){
    auto info = any_cast<IGInfo*>(n->info);
    for(auto& succ: *n->succs){
        succ->preds->erase(n);
        succ->succs->erase(n);
    }
    info->hide_edges = n->succs;
    n->succs = new NodeSet();
    n->preds = new NodeSet();

}

void IG::addEdge_move(Node* n1, Node* n2){
    auto info1 = any_cast<IGInfo*>(n1->info);
    auto info2 = any_cast<IGInfo*>(n2->info);
    info1->mov_targets->emplace(n2);
    info2->mov_targets->emplace(n1);
}
void IG::rmEdge_move(Node* n1, Node* n2){
    auto info1 = any_cast<IGInfo*>(n1->info);
    auto info2 = any_cast<IGInfo*>(n2->info);
    info1->mov_targets->erase(n2);
    info2->mov_targets->erase(n1);
}



};