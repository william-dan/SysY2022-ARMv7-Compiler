#include "flowgraph.hpp"
#include <cassert>
#include <algorithm>
#include <iterator>

namespace cfg{

FlowGraph::FlowGraph(): Graph(){}

FlowGraph::FlowGraph(InstrList* il, utils::TempType temptype): Graph(){
    //(I) Iterate over the entire instruction list
    ITType prev_type = ITType::JUMP;
    Node* prev;
    NodeList jumpList;
    for(auto& i: *il){
        Node* curr = new Node(this, nullptr);
        TempSet *defs, *uses;
        ITType type = ITType::COMMON;
        
        // i->print();
        switch (i->type)
        {
        case assem::AssemType::Oper:
        {
            assem::OperInstr* o = static_cast<assem::OperInstr*>(i);
            if(!o->jumps->empty()){
                type = ITType::JUMP;
                jumpList.emplace_back(curr);
            }
            auto defslist = o->get_def(temptype);
            auto useslist = o->get_use(temptype);
            defs = new TempSet(defslist->begin(), defslist->end());
            uses = new TempSet(useslist->begin(), useslist->end());
        }
        break;
        case assem::AssemType::Label:
        {
            assem::LabelInstr* l = static_cast<assem::LabelInstr*>(i);
            labelTable[l->label] = curr;
            defs = new TempSet();
            uses = new TempSet();
        }
        break;
        case assem::AssemType::Move:
        {
            assem::MoveInstr* m = static_cast<assem::MoveInstr*>(i);
            auto defslist = m->get_def(temptype);
            auto useslist = m->get_use(temptype);
            defs = new TempSet(defslist->begin(), defslist->end());
            uses = new TempSet(useslist->begin(), useslist->end());
            if(defslist->size() == 1 && useslist->size() == 1){
                type = ITType::MOVE;
            }
        }
        break;
        default:
            assert(0);
        break;
        }

        any info = new FGinfo(i, uses, defs, type == ITType::MOVE);
        curr->info = info;

        if(prev_type != ITType::JUMP){
            addEdge(prev, curr);
        }

        prev = curr;
        prev_type = type;

    }
    // cout << "flowgraph half pass" << endl;
    for(auto& node: jumpList){
        FGinfo* info = any_cast<FGinfo*>(node->info);
        assert(info->instr->type == assem::AssemType::Oper);
        assem::OperInstr* o = static_cast<assem::OperInstr*>(info->instr);
        // o->print();
        for(auto& l: *o->jumps){
            addEdge(node, labelTable[l]);
        }
    }
    // cout << "flowgraph tot pass" << endl;
}



TempSet* FlowGraph::def(Node* n){
    FGinfo* info = any_cast<FGinfo*>(n->info);
    return info->defs;
}
TempSet* FlowGraph::use(Node* n){
    FGinfo* info = any_cast<FGinfo*>(n->info);
    return info->uses;
}
bool FlowGraph::isMove(Node* n){
    FGinfo* info = any_cast<FGinfo*>(n->info);
    return info->isMove;
}

void FlowGraph::show(){
    cout << "fg has " << nodecount << " nodes" << endl;
    for(auto& n: *mynodes){
        cout << n->mykey << ": ";
        for(auto& succ: *n->succs){
            cout << succ->mykey << " ";
        }
        cout << endl;
    }
}


void FlowGraph::Liveness(){

    std::stack<Node*> workset;
    for(auto& n: *mynodes){
        workset.emplace(n);
        auto info = any_cast<FGinfo*>(n->info);
        info->in = new TempSet();
        info->out = new TempSet();
    }
    while(!workset.empty()){
        auto n = workset.top();
        workset.pop();
        auto newOut = new TempSet();
        auto newIn = new TempSet();
        TempSet tempset;
        auto info = any_cast<FGinfo*>(n->info);
        auto use = info->uses;
        auto def = info->defs;
        auto n_in = info->in;
        auto n_out = info->out;
        for(auto& it: *n->succs){
            auto succ = any_cast<FGinfo*>(it->info)->in;
            for(auto& in: *succ) newOut->emplace(in);
        }
        std::set_difference(
            newOut->begin(), newOut->end(),
            def->begin(), def->end(),
            std::inserter(tempset, tempset.begin())
        );
        std::set_union(
            use->begin(), use->end(),
            tempset.begin(), tempset.end(),
            std::inserter(*newIn, newIn->begin())
        );
        if(*n_in != *newIn){
            assert(newIn->size() > n_in->size());
            // assert(newIn->size() <= inTable.size());
            for(auto& it: *n->preds){
                // assert(it->mykey != n->mykey);
                workset.emplace(it);
            }
        }
        delete n_in;
        delete n_out;
        info->in = newIn;
        info->out = newOut;

    }
}
void FlowGraph::showLiveness(){
    for(auto& n: *mynodes){
        cout << "-----------------------------" << endl;
        TempSet* out = FG_Out(n);
        TempSet* in = FG_In(n);
        cout << n->mykey << endl;
        cout << "fg_out: " << endl;
        for(auto& o: *out){
            cout << *o->id << " ";
        }
        cout << endl;
        cout << "fg_in: " << endl;
        for(auto& i: *in){
            cout << *i->id << " ";
        }
        cout << endl;
    }
}
utils::TempSet* FlowGraph::FG_Out(Node* n){
    return any_cast<FGinfo*>(n->info)->out;
}
utils::TempSet* FlowGraph::FG_In(Node* n){
    return any_cast<FGinfo*>(n->info)->in;
}


FGinfo::FGinfo(Instr* instr, TempSet* uses, TempSet* defs, bool isMove):
    instr(instr), uses(uses), defs(defs), isMove(isMove){}

FlowGraph::~FlowGraph(){
    for(auto& n: *mynodes){
        auto info = std::any_cast<FGinfo*>(n->info);
        delete info;
    }
}



};