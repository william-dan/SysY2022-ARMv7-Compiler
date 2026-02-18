#include "assem_bg.hpp"
#include <unordered_set>


namespace cfg{
using namespace std;

A_BlockGraph::A_BlockGraph(assem::InstrList* il){


    unordered_map<utils::Label*, Node*> labelTable;

    for(auto& i: *il){
        if(i->type == assem::AssemType::Label){
            auto curr = new Node(this, new unordered_set<utils::Temp*>());
            auto li = static_cast<assem::LabelInstr*>(i);
            labelTable[li->label] = curr;
            if(!entry) entry = curr;
        }
    }

    Node* curr = nullptr;
    bool jumped = false;
    unordered_set<utils::Temp*>* ts = nullptr;
    for(auto& i: *il){
        if(i->type == assem::AssemType::Label){
            auto li = static_cast<assem::LabelInstr*>(i);
            auto curr_temp = labelTable[li->label];
            ts = any_cast<unordered_set<utils::Temp*>*>(curr_temp->info);
            if(!jumped && curr){
                addEdge(curr, curr_temp);
            }
            curr = curr_temp;
            jumped = false;
        }else if(i->type == assem::AssemType::Oper && 
        static_cast<assem::OperInstr*>(i)->jumps->size()){
            assert(curr && ts);
            auto j = static_cast<assem::OperInstr*>(i);
            for(auto& target: *j->jumps){
                addEdge(curr, labelTable[target]);
            }
            jumped = true;
        }else if(i->assem.find("bx") != string::npos){
            jumped = true;
        }else{
            if(!curr || !ts) continue;
            auto defs = i->get_def();
            auto uses = i->get_use();
            for(auto& def: *defs){
                ts->emplace(def);
            }
            for(auto& use: *uses){
                ts->emplace(use);
            }
        }

    }
    deadnode_eliminate();

}
A_BlockGraph::~A_BlockGraph(){
    for(auto& n: *mynodes){
        auto info = any_cast<unordered_set<utils::Temp*>*>(n->info);
        delete info;
    }
}

void A_BlockGraph::show(){
    cout << "a_bg has " << nodecount << " nodes" << endl;
    for(auto& n: *mynodes){
        cout << n->mykey << ": ";
        for(auto& succ: *n->succs){
            cout << succ->mykey << " ";
        }
        cout << endl;
    }
}

void A_BlockGraph::dfs_abg(Node* n){
    if(done.count(n)) return;
    done.emplace(n);
    for(auto& succ: *n->succs){
        dfs_abg(succ);
    }
}

void A_BlockGraph::deadnode_eliminate(){
    dfs_abg(entry);
    for(auto it = mynodes->begin(); it != mynodes->end(); ){
        auto n = *it;
        if(!done.count(n)){
            while(!n->preds->empty()){
                auto top = *n->preds->begin();
                rmEdge(top, n);
            }
            while(!n->succs->empty()){
                auto top = *n->succs->begin();
                rmEdge(n, top);
            }
            it = mynodes->erase(it);
        }else{
            it++;
        }
    }
    done.clear();
}
    

}