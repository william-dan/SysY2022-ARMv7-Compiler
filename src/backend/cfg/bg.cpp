#include <algorithm>
#include <cassert>
#include <stack>
#include <iterator>
#include "bg.hpp"


namespace cfg {
using namespace std;
using utils::LabelList;
using mir::StmType;
using mir::Jump;
using mir::Cjump;

static unordered_map<utils::Label*, mir::StmList*> env;

BlockGraph::BlockGraph(mir::Block* block): Graph(){
    
    
    for(auto& stmList: *block->stmLists){
        assert(stmList->size() > 0 &&
            stmList->front()->kind == StmType::LABEL);
        env[static_cast<mir::Label*>(stmList->front())->label] = stmList;
        lookup(stmList);
    }
    env[block->label] = new mir::StmList{
        new mir::Label(block->label)};
    exit = lookup(env[block->label]);
    entry = lookup(block->stmLists->front());

    for(auto& stmList: *block->stmLists){
        auto lastStm = stmList->back();
        LabelList* ll;
        switch (lastStm->kind)
        {
        case StmType::JUMP:
        {
            Jump* j = static_cast<Jump*>(lastStm);
            ll = new LabelList{j->label};
        }
        break;
        case StmType::CJUMP:
        {
            Cjump* c = static_cast<Cjump*>(lastStm);
            ll = new LabelList{c->true_label, c->false_label};
        }
        break;
        case StmType::RETURN:
        {
            mir::Return* c = static_cast<mir::Return*>(lastStm);
            ll = new LabelList();
            enter(stmList, env[block->label]);
            
        }
        break;
        default:
        assert(0);
        break;
        }
        
        for(auto& l: *ll){
            if(env.count(l)){
                enter(stmList, env[l]);
            }  
        }
    }
    // show();
    deadnode_eliminate();
}

void BlockGraph::dfs_bg(Node* n){
    if(done.count(n)) return;
    done.emplace(n);
    for(auto& succ: *n->succs){
        dfs_bg(succ);
    }
}

void BlockGraph::deadnode_eliminate(){
    dfs_bg(this->entry);
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


Node* BlockGraph::lookup(mir::StmList* b){
    for(auto& node: *mynodes){
        if(any_cast<mir::StmList*>(node->info) == b){
            return node;
        }
    }
    any info = b;
    return new Node(this, info);
}

void BlockGraph::enter(mir::StmList* b1, mir::StmList* b2){
    auto n1 = lookup(b1);
    auto n2 = lookup(b2);
    addEdge(n1, n2);
}

mir::StmList* BlockGraph::get_info(Node* n){
    return any_cast<mir::StmList*>(n->info);
}


void BlockGraph::show(){
    cout << "bg has " << nodecount << " nodes" << endl;
    for(auto& n: *mynodes){
        cout << n->mykey << ": ";
        for(auto& succ: *n->succs){
            cout << succ->mykey << " ";
        }
        cout << endl;
    }
    cout << "bg code:" << endl;
    for(auto& n: *mynodes){
        cout << n->mykey << ": ";
        for(auto& s: *std::any_cast<mir::StmList*>(n->info)){
            s->print();
        }
    }
}

mir::Block* BlockGraph::toBlock(){
    // cerr << "before toBlock" << endl;
    auto sll = new mir::StmListList();
    auto bl = new mir::Block();
    for(auto& n: *mynodes){
        auto sl = get_info(n);
        auto label = static_cast<mir::Label*>(sl->front())->label;
        if(sl->size() == 1){
            bl->label = label;
        }else{
            sll->emplace_back(sl);
        }
    }
    bl->stmLists = sll;
    // cerr << "after toBlock" << endl;
    return bl;
}



void BlockGraph::Liveness(){
    for(auto& n: *mynodes){
        auto sl = get_info(n);
        auto uses = new utils::TempSet();
        auto defs = new utils::TempSet();

        for(auto& s: *sl){
            for(auto& use: s->get_use()){
                uses->emplace(use);
            }
            for(auto& def: s->get_def()){
                defs->emplace(def);
            }
        }
        liveness_info[n] = new BGinfo(uses, defs);

    }
    stack<Node*> workset;
    for(auto& n: *mynodes){
        workset.emplace(n);
    }
    while(!workset.empty()){
        auto n = workset.top();
        workset.pop();
        auto newOut = new utils::TempSet();
        auto newIn = new utils::TempSet();
        utils::TempSet tempset;
        for(auto& it: *n->succs){
            for(auto& in: *liveness_info[it]->in) newOut->emplace(in);
        }
        set_difference(
            newOut->begin(), newOut->end(),
            liveness_info[n]->defs->begin(), liveness_info[n]->defs->end(),
            inserter(tempset, tempset.begin())
        );
        set_union(
            liveness_info[n]->uses->begin(), liveness_info[n]->uses->end(),
            tempset.begin(), tempset.end(),
            inserter(*newIn, newIn->begin())
        );
        if(*liveness_info[n]->in != *newIn){
            assert(newIn->size() > liveness_info[n]->in->size());
            for(auto& it: *n->preds){
                // assert(it->mykey != n->mykey);
                workset.emplace(it);
            }
        }
        delete liveness_info[n]->in;
        delete liveness_info[n]->out;
        liveness_info[n]->in = newIn;
        liveness_info[n]->out = newOut;
    }
    
}
void BlockGraph::showLiveness(){
    for(auto& n: *mynodes){
        cout << "-----------------------------" << endl;
        utils::TempSet* out = liveness_info[n]->out;
        utils::TempSet* in = liveness_info[n]->in;
        cout << n->mykey << endl;
        cout << "bg_out: " << endl;
        for(auto& o: *out){
            cout << *o->id << " ";
        }
        cout << endl;
        cout << "bg_in: " << endl;
        for(auto& i: *in){
            cout << *i->id << " ";
        }
        cout << endl;
    }
}
void BlockGraph::clearLiveness(){
    for(auto& pr: liveness_info){
        delete pr.second;
    }
    liveness_info.clear();
}


BGinfo::BGinfo(utils::TempSet* uses, utils::TempSet* defs):
uses(uses), defs(defs){
    in = new utils::TempSet();
    out = new utils::TempSet();
}




void BlockGraph::cut_edge(Node* old_n, Node* new_n){
    cout << "old_node: " << old_n->mykey << endl;
    cout << "new_node: " << new_n->mykey << endl;
    while(!new_n->preds->empty()){
        auto top = *new_n->preds->begin();
        rmEdge(top, new_n);
    }
    auto ol = get_info(old_n)->front();
    auto nl = get_info(new_n)->front();
    assert(ol->kind == mir::StmType::LABEL);
    assert(nl->kind == mir::StmType::LABEL);
    auto oldlabel = static_cast<mir::Label*>(ol)->label;
    auto newlabel = static_cast<mir::Label*>(nl)->label;

    for(auto& pred: *old_n->preds){
        auto p_j = get_info(pred)->back();
        if(p_j->kind == mir::StmType::JUMP){
            auto j = static_cast<mir::Jump*>(p_j);
            j->label = newlabel;
        }else if(p_j->kind == mir::StmType::CJUMP){
            auto cj = static_cast<mir::Cjump*>(p_j);
            if(cj->true_label == oldlabel){
                cj->true_label = newlabel;
            }
            if(cj->false_label == oldlabel){
                cj->false_label = newlabel;
            }
        }else{
            assert(0);
        }
    }
    while(!old_n->preds->empty()){
        auto top = *old_n->preds->begin();
        addEdge(top, new_n);
        rmEdge(top, old_n);
    }
}

Node* BlockGraph::node_deepcopy(Node* proto){
    auto sl = new mir::StmList();
    auto n = new Node(this, sl);
    for(auto& s: *get_info(proto)){
        sl->emplace_back(s->stm_deepcopy());
    }
    auto label_stm = static_cast<mir::Label*>(get_info(n)->front());
    label_stm->label = new utils::Label();
    return n;
}

utils::Label* BlockGraph::get_node_label(Node* n){
    auto sl = get_info(n);
    assert(sl->front()->kind == mir::StmType::LABEL);
    return static_cast<mir::Label*>(sl->front())->label;
}

void BlockGraph::detach_cond(){
    for(auto& n: *mynodes){
        auto sl = get_info(n);
        if(sl->back()->kind == StmType::CJUMP){
            auto cj = static_cast<Cjump*>(sl->back());
            if(cj->left->kind == mir::ExpType::CONST && cj->right->kind == mir::ExpType::CONST){
                auto lconst = static_cast<mir::Const*>(cj->left);
                auto rconst = static_cast<mir::Const*>(cj->right);
                auto result = false;
                switch (cj->op)
                {
                case mir::RelOp::EQ:
                {
                    if(lconst->type == utils::TempType::FLOAT){
                        result = (lconst->n.f == rconst->n.f);
                    }else{
                        result = (lconst->n.i == rconst->n.i);
                    }
                }
                break;
                case mir::RelOp::GE:
                {
                    if(lconst->type == utils::TempType::FLOAT){
                        result = (lconst->n.f >= rconst->n.f);
                    }else{
                        result = (lconst->n.i >= rconst->n.i);
                    }
                }
                break;
                case mir::RelOp::GT:
                {
                    if(lconst->type == utils::TempType::FLOAT){
                        result = (lconst->n.f > rconst->n.f);
                    }else{
                        result = (lconst->n.i > rconst->n.i);
                    }
                }
                break;
                case mir::RelOp::LE:
                {
                    if(lconst->type == utils::TempType::FLOAT){
                        result = (lconst->n.f <= rconst->n.f);
                    }else{
                        result = (lconst->n.i <= rconst->n.i);
                    }
                }
                break;
                case mir::RelOp::LT:
                {
                    if(lconst->type == utils::TempType::FLOAT){
                        result = (lconst->n.f < rconst->n.f);

                    }else{
                        result = (lconst->n.i < rconst->n.i);
                        
                    }
                }
                break;
                case mir::RelOp::NE:
                {
                    if(lconst->type == utils::TempType::FLOAT){
                        result = (lconst->n.f != rconst->n.f);

                    }else{
                        result = (lconst->n.i != rconst->n.i);
                        
                    }
                }
                break;
                default:
                assert(0);
                break;
                }
                auto succs = n->succs;
                assert(succs->size() == 2);
                auto it = succs->begin();
                Node *fn, *tn;
                auto first = *it++;
                auto second = *it;
                auto fsl =  get_info(first);

                assert(fsl->front()->kind == mir::StmType::LABEL);
                if(static_cast<mir::Label*>(fsl->front())->label == cj->false_label){
                    fn = first;
                    tn = second;
                }else{
                    fn = second;
                    tn = first;
                }
                
                if(result){
                    rmEdge(n, fn);
                    sl->pop_back();
                    sl->emplace_back(new mir::Jump(cj->true_label));
                }else{
                    rmEdge(n, tn);
                    sl->pop_back();
                    sl->emplace_back(new mir::Jump(cj->false_label));
                }
                
            }
        }
    }
}



void BlockGraph::realmerge(Node* a, Node* b){
    auto asl = get_info(a);
    auto bsl = get_info(b);
    asl->pop_back();
    bsl->pop_front();
    rmEdge(a, b);
    for(auto& ap: *a->preds){
        addEdge(ap, b);
    }
    while(!a->preds->empty()){
        auto bg = *a->preds->begin();
        rmEdge(bg, a);
    }
    for(auto it = asl->rbegin(); it != asl->rend(); it++){
        bsl->emplace_front(*it);
    }
    mynodes->remove(a);
}


void BlockGraph::merge_node(){
  bool changed = true;
  while(changed){
    changed = false;
    for(auto node: *mynodes){
        if(node == entry || node == exit) continue;
        if(node->preds->size() == 1 && (*node->preds->begin())->succs->size() == 1 && 
        (*node->preds->begin()) != entry){
            realmerge(*node->preds->begin(), node);
            changed = true;
            break;
        }
      }
    }
  }



};