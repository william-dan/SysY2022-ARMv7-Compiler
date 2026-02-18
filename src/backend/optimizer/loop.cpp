#include "loop.hpp"
#include "../cfg/assem_bg.hpp"
#include <queue>
#include <string>

#include <iterator>

namespace ssa{
using namespace std;

void Loop::get_parent(Node* n){
    if(done.count(n)) return;
    done.emplace(n);
    for(auto& succ: *n->succs){
        get_parent(succ);
    }
    if(loops.count(n)){
        for(auto& node: loops[n]){
            if(node == n) continue;
            if(!parent.count(node)) parent[node] = n;
        }
    }
}

void Loop::build_tree(){
    for(auto& n: *ir){
        if(!parent.count(n)){
            tree[nullptr].emplace(n);
        }else{
            tree[parent[n]].emplace(n);
        }
    }
}

void Loop::get_dep(Node* curr, int dep){
    n2dep[curr] = dep;
    for(auto& child: tree[curr]){
        get_dep(child, dep + 1);
    }
    if(!tree[curr].empty()){
        n2dep[curr] = dep+1;
    }
}

//only a_bg
void Loop::compute_temp_score(){
    auto bg = static_cast<A_BlockGraph*>(ir->front()->mygraph);
    for(auto& n: *bg->mynodes){
        auto dep = n2dep[n];
        int score = 0;
        if(dep == 0) score = 1;
        else score = dep * 10;
        auto info = any_cast<unordered_set<utils::Temp*>*>(n->info);
        for(auto& t: *info){
            if(temp_score.count(t)){
                temp_score[t] += score;
            }else{
                temp_score[t] = score;
            }
        }
    }
}


// repeat:
// a ? begin: final
// begin:

// until:
// a ? begin: final

// final:

Loop::Loop(NodeList* ir):
ir(ir)
{
    assert(!ir->empty());
   
    // cout << "before domi" << endl;
    auto dtree = DominatorTree(ir);
    //找所有的回边，用(h, set)表示
    for(auto& n: *ir){
        for(auto& succ: *n->succs){
            auto curr = n;
            while(curr){
                if(succ == curr){
                    loops[succ].emplace(n);
                    cond[succ].emplace(n);
                    break;
                }
                curr = dtree.idom[curr];
            }
            
        }
    }

    //修补
    for(auto& loop: loops) loop.second.emplace(loop.first);
    //找自然循环
    for(auto& loop: loops){
        auto h = loop.first;
        queue<Node*> q;
        for(auto& n: loop.second){
            if(n == h) continue;
            q.emplace(n);
        }
        while(!q.empty()){
            auto n = q.front();
            q.pop();
            for(auto& pred: *n->preds){
                if(loop.second.count(pred) || pred == h) continue;
                loop.second.emplace(pred);
                q.emplace(pred);
            }
        }
    }
    //建立循环嵌套树
    get_parent(ir->front()->mygraph->entry);
    build_tree();
    get_dep(nullptr, 0);

    // show_tree();
    // show_n2dep();
    
  
}

// only bg
void Loop::Unroll(){
    for(auto& loop: loops){
        if(loop.second.size() > 2) continue;
        auto info = analyse(loop.first);
        // show_LoopInfo(info);
        if(info.canUnroll){
            add_preheader(info);
            loopUnroll(info);
        }
    }
}



// precondition:
// a = b op c
// a = b
// a = mem()
// a = Name()
// a = call()
// a = phi()

LoopInfo Loop::analyse(Node* h){
    auto bg = static_cast<BlockGraph*>(h->mygraph);
    //初始化loop info
    LoopInfo info;
    info.head = h;
    // assert(mfg);
    //拿到loop
    auto loop = loops[h];
    //复杂loop不做
    if(loop.size() > 2) return info;
    //提取def, use

    for(auto& n: loop){
        for(auto& s: *bg->get_info(n)){
            if(s->kind == mir::StmType::MOVE){
                auto mov = static_cast<mir::Move*>(s);
                if(mov->dst->kind != mir::ExpType::TEMP) continue;
                auto curr_temp = static_cast<mir::Temp*>(mov->dst)->temp;
                info.def_exp[curr_temp] = mov->src;
                switch (mov->src->kind)
                {
                case mir::ExpType::BINOP:
                {
                    auto mbop = static_cast<mir::Binop*>(mov->src);
                    info.def_use.emplace(make_pair(curr_temp, unordered_set<utils::Temp*>()));
                    if(mbop->left->kind == mir::ExpType::CONST){
                        
                    }else if(mbop->left->kind == mir::ExpType::TEMP){
                        auto temp = static_cast<mir::Temp*>(mbop->left)->temp;
                        info.def_use[curr_temp].emplace(temp);
                    }else{
                        info.def_use[curr_temp].emplace(nullptr);
                    }

                    if(mbop->right->kind == mir::ExpType::CONST){
                        
                    }else if(mbop->right->kind == mir::ExpType::TEMP){
                        auto temp = static_cast<mir::Temp*>(mbop->right)->temp;
                        info.def_use[curr_temp].emplace(temp);
                    }else{
                        info.def_use[curr_temp].emplace(nullptr);
                    }
                }
                break;
                case mir::ExpType::CONST:
                {
                    auto mc = static_cast<mir::Const*>(mov->src);
                    info.def_use.emplace(make_pair(curr_temp, std::unordered_set<utils::Temp*>{}));
                }
                break;
                case mir::ExpType::TEMP:
                {
                    auto mtemp = static_cast<mir::Temp*>(mov->src);
                    info.def_use[curr_temp].emplace(mtemp->temp);
                }
                break;
                case mir::ExpType::PHI:
                {
                    assert(0);
                    auto mphi = static_cast<mir::Phi*>(mov->src);
                    for(auto& t: *mphi->tl){
                        info.def_use[curr_temp].emplace(t);
                    }
                }
                break;
                default:
                {
                    info.def_use[curr_temp].emplace(nullptr);
                }
                break;
                }
            }
        }
    }
    //找循环不变量

    //找循环里所有定义过的temp
    for(auto& du: info.def_use){
        info.var.emplace(du.first);
    }
    //找use，如果没有定义过，那么就是循环外的
    bool change = true;
    int iteration_n = 0;
    while(change){
        change = false;
        for(auto it = info.var.begin(); it != info.var.end();){
            bool is_invar = true;
            auto d = *it;
            auto us = info.def_use[d];
            if(us.count(nullptr)){
                it++;
                continue;
            }
            for(auto& u: us){
                if(info.var.count(u)){
                    is_invar = false;
                    break;
                }
            }
            if(is_invar){
                info.invar.emplace(d);
                info.var.erase(it++);
                change = true;
            }else{
                it++;
            }
        }
        iteration_n++;
    }
    // cout << "iteration_n: " << iteration_n << endl;
    //找基本归纳变量
    if(cond[h].size() == 1){
        auto cond_h = *cond[h].begin();
        auto last_stm = bg->get_info(cond_h)->back();
        auto cj = static_cast<mir::Cjump*>(last_stm);
        if(cj->left->kind == mir::ExpType::TEMP && cj->right->kind == mir::ExpType::CONST){
            auto ltemp = static_cast<mir::Temp*>(cj->left);
            auto rconst = static_cast<mir::Const*>(cj->right);
            info.times = rconst;
            auto result = find_circle(ltemp->temp, info);
            if(result){
                info.step = result;
            }
        }else if(cj->left->kind == mir::ExpType::CONST && cj->right->kind == mir::ExpType::TEMP){
            auto lconst = static_cast<mir::Const*>(cj->left);
            auto rtemp = static_cast<mir::Temp*>(cj->right);
            info.times = lconst;
            auto result = find_circle(rtemp->temp, info);
            if(result){
                info.step = result;
            }
        }else if(cj->left->kind == mir::ExpType::TEMP && cj->right->kind == mir::ExpType::TEMP){
            auto ltemp = static_cast<mir::Temp*>(cj->left);
            auto rtemp = static_cast<mir::Temp*>(cj->right);

            if(!(info.var.count(ltemp->temp)) && info.var.count(rtemp->temp)){
                auto result = find_circle(rtemp->temp, info);
                if(result){
                    info.step = result;
                }
                info.times = ltemp;
            }else if(info.var.count(ltemp->temp) && !(info.var.count(rtemp->temp))){
                auto result = find_circle(ltemp->temp, info);
                if(result){
                    info.step = result;
                }
                info.times = rtemp;
            }
        }
    }
    int min_id = -1;
    utils::Temp* min_tmp = nullptr;
    for(auto& n: info.induce_temp){
        int curr_id = stoi(*n->id);
        if(min_id == -1){
            min_id = curr_id;
            min_tmp = n;
        }else if(curr_id < min_id){
            min_id = curr_id;
            min_tmp = n;
        }
    }
    assert(min_tmp || info.induce_temp.empty());
    info.ini_iter = min_tmp;
    //找导出归纳变量
    
    

    // special case
  

    if(info.step && info.times && info.induce_temp.size())
        info.canUnroll = true;

    return info;
}


Node* Loop::add_preheader(LoopInfo& info){
    assert(!info.preheader_added);
    info.preheader_added = true;
    auto bg = static_cast<BlockGraph*>(info.head->mygraph);
    auto newsl = new mir::StmList();
    auto newnode = new Node(bg, newsl);
    auto h_label = static_cast<mir::Label*>(bg->get_info(info.head)->front())->label;
    auto newlabel = new utils::Label();
    newsl->emplace_back(new mir::Label(newlabel));
    newsl->emplace_back(new mir::Jump(h_label));
    
    auto copy_preds = NodeSet(*info.head->preds);
    for(auto& pred: copy_preds){
        if(cond[info.head].count(pred)) continue;
        auto p_j = bg->get_info(pred)->back();
        if(p_j->kind == mir::StmType::JUMP){
            auto j = static_cast<mir::Jump*>(p_j);
            j->label = newlabel;
        }else if(p_j->kind == mir::StmType::CJUMP){
            auto cj = static_cast<mir::Cjump*>(p_j);
            if(cj->true_label == h_label){
                cj->true_label = newlabel;
            }
            if(cj->false_label == h_label){
                cj->false_label = newlabel;
            }
        }else{
            assert(0);
        }
        bg->addEdge(pred, newnode);
        bg->rmEdge(pred, info.head);
    }
    bg->addEdge(newnode, info.head);
    return newnode;
}


void Loop::loopUnroll(LoopInfo& info){
    assert(info.canUnroll && info.preheader_added);
    if(!info.ini_iter) return;
    assert(info.times && info.step && info.ini_iter);
    auto bg = static_cast<BlockGraph*>(info.head->mygraph);

    auto first_cond = find_single_exit(info.head);
    auto first_cjump = static_cast<mir::Cjump*>(bg->get_info(first_cond)->back());
    if(first_cjump->op != mir::RelOp::GE
    && first_cjump->op != mir::RelOp::GT
    && first_cjump->op != mir::RelOp::LE
    && first_cjump->op != mir::RelOp::LT) return;
    
    

    //计算 i < times - step * unroll_times
    auto first_end = new utils::Temp(utils::TempType::INT);
    auto multi_step = new utils::Temp(utils::TempType::INT);
    auto multi_step_assign = new mir::Move(
        new mir::Temp(multi_step),
        new mir::Binop(mir::BinOp::MUL,
            info.step->exp_deepcopy(),
            new mir::Const(unroll_times)
        )
    );
    auto first_end_assign = new mir::Move(
        new mir::Temp(first_end),
        new mir::Binop(mir::BinOp::MINUS, 
            info.times->exp_deepcopy(),
            new mir::Temp(multi_step)
        )   
    );
    //新的block，处理i > times - step * unroll_times情况，连接到bg里
    auto second_head = bg->node_deepcopy(info.head);
    auto second_cond = bg->node_deepcopy(first_cond);
    
    first_cjump->false_label = bg->get_node_label(second_head);
    auto second_jump = static_cast<mir::Jump*>(bg->get_info(second_head)->back());
    second_jump->label = bg->get_node_label(second_cond);
    auto second_cjump = static_cast<mir::Cjump*>(bg->get_info(second_cond)->back());
    second_cjump->true_label = bg->get_node_label(second_head);
    auto final_node = find_final(info.head);
    bg->rmEdge(first_cond, final_node);
    bg->addEdge(first_cond, second_head);
    bg->addEdge(second_head, second_cond);
    bg->addEdge(second_cond, second_head);
    bg->addEdge(second_cond, final_node);


    //把计算的i < times - step * unroll_times放进前置节点
    auto preheader = find_preheader(info.head);
    auto pre_place_block = bg->get_info(preheader);
    pre_place_block->pop_back();
    auto pre_place_it = pre_place_block->end();
    
    for(auto& s: *bg->get_info(second_cond)){
        if(s->kind == mir::StmType::LABEL) continue;
        pre_place_block->emplace(pre_place_it, s->stm_deepcopy());
    }
    pre_place_it--;
    pre_place_block->emplace(pre_place_it, multi_step_assign);
    pre_place_block->emplace(pre_place_it, first_end_assign);

    //修改header节点的cjump
    if(first_cjump->left->kind == mir::ExpType::TEMP &&
    first_cjump->right->kind == mir::ExpType::CONST){
        first_cjump->right = new mir::Temp(first_end);
    }else if(first_cjump->left->kind == mir::ExpType::CONST &&
    first_cjump->right->kind == mir::ExpType::TEMP){
        first_cjump->left = new mir::Temp(first_end);
    }else if(first_cjump->left->kind == mir::ExpType::TEMP &&
    first_cjump->right->kind == mir::ExpType::TEMP){
        if(info.induce_temp.count(static_cast<mir::Temp*>(first_cjump->left)->temp)){
            first_cjump->right = new mir::Temp(first_end);
        }else{
            first_cjump->left = new mir::Temp(first_end);
        }
    }else{
        assert(0);
    }
    //复制cjump，用来给preheader
    auto first_cjump_copy = new mir::Cjump(*first_cjump);
    if(first_cjump_copy->left->kind == mir::ExpType::TEMP &&
    first_cjump_copy->right->kind == mir::ExpType::CONST){
        first_cjump_copy->left = new mir::Temp(info.ini_iter);
    }else if(first_cjump_copy->left->kind == mir::ExpType::CONST &&
    first_cjump_copy->right->kind == mir::ExpType::TEMP){
        first_cjump_copy->right = new mir::Temp(info.ini_iter);
    }else if(first_cjump_copy->left->kind == mir::ExpType::TEMP &&
    first_cjump_copy->right->kind == mir::ExpType::TEMP){
        if(info.induce_temp.count(static_cast<mir::Temp*>(first_cjump_copy->left)->temp)){
            first_cjump_copy->left = new mir::Temp(info.ini_iter);
        }else{
            first_cjump_copy->right = new mir::Temp(info.ini_iter);
        }
    }else{
        assert(0);
    }

    pre_place_block->emplace(pre_place_it, first_cjump_copy);
    pre_place_block->pop_back();
    bg->addEdge(preheader, second_head);


    //复制循环体unroll_times遍
    auto copy_begin = bg->get_info(info.head)->begin();
    copy_begin++;
    auto copy_end = bg->get_info(info.head)->end();
    copy_end--;
    auto copy_place_block = bg->get_info(info.head);
    mir::StmList proto;
    for(auto it = copy_begin; it != copy_end; it++){
        proto.emplace_back((*it));
    }
    for(int i = 0; i < unroll_times - 1; i++){
        for(auto& s: proto){
            copy_place_block->emplace(copy_end, s->stm_deepcopy());
        }
    }
    // cerr << "unroll" << endl;

}


mir::Exp* Loop::find_circle(utils::Temp* t, LoopInfo& info){
    mir::Exp* exp = nullptr;
    info.induce_temp.emplace(t);
    while(info.def_exp.count(t) && info.def_exp[t]->kind == mir::ExpType::TEMP){
        t = *info.def_use[t].begin();
        info.induce_temp.emplace(t);
    }
    if(!info.def_exp.count(t)){
        info.induce_temp.clear();
        return nullptr;
    }
    auto e = info.def_exp[t];
    if(e->kind != mir::ExpType::BINOP){
        info.induce_temp.clear();
        return nullptr;
    }
    auto binop = static_cast<mir::Binop*>(e);
    if(binop->op != mir::BinOp::PLUS && binop->op != mir::BinOp::MINUS) return nullptr;
    if(binop->left->kind == mir::ExpType::TEMP
    && binop->right->kind == mir::ExpType::CONST){
        if(binop->op == mir::BinOp::MINUS){
            exp = new mir::Const(-static_cast<mir::Const*>(binop->right)->n.i);
        }else{
            exp = binop->right;
        }
        t = static_cast<mir::Temp*>(binop->left)->temp;
        info.induce_temp.emplace(t);
    }else if(binop->left->kind == mir::ExpType::CONST
    && binop->right->kind == mir::ExpType::TEMP){
        if(binop->op == mir::BinOp::MINUS){
            exp = new mir::Const(-static_cast<mir::Const*>(binop->left)->n.i);
        }else{
            exp = binop->left;
        }
        t = static_cast<mir::Temp*>(binop->right)->temp;
        info.induce_temp.emplace(t);
    }else{
        info.induce_temp.clear();
        return nullptr;
    }
    while(info.def_exp.count(t) && info.def_exp[t]->kind == mir::ExpType::TEMP && !info.induce_temp.count(t)){
        t = *info.def_use[t].begin();
        // cout << t->to_string() << endl;
        info.induce_temp.emplace(t);
    }
    if(info.induce_temp.count(t)) return exp;
    info.induce_temp.clear();
    return nullptr;
}



Node* Loop::find_preheader(Node* h){
    vector<Node*> hds;
    for(auto& pred: *h->preds){
        if(cond[h].count(pred)) continue;
        hds.emplace_back(pred);
    }
    assert(hds.size() == 1);
    auto enter = hds.front();
    return enter;
}

Node* Loop::find_single_exit(Node* h){
    assert(cond[h].size() == 1);
    return *cond[h].begin();
}


Node* Loop::find_final(Node* h){
    auto exit = find_single_exit(h);
    vector<Node*> hds;
    for(auto& succ: *exit->succs){
        if(succ == h) continue;;
        hds.emplace_back(succ);
    }
    assert(hds.size() == 1);
    return hds.front();
}



void Loop::show_tree(){
    cout << "loop tree has " << tree.size() << " head node" << endl;
    for(auto& pr: tree){
        if(!pr.first) cout << "-1: " << endl;
        else cout << pr.first->mykey << ": " << endl;
        for(auto& n: pr.second){
            cout << n->mykey << " ";
        }
        cout << endl;
    }

}

void Loop::show_n2dep(){
    cout << "n2dep has " << n2dep.size() << " nodes" << endl;
    for(auto& pr: n2dep){
        if(!pr.first) cout << "-1: ";
        else cout << pr.first->mykey << ": ";
        cout << pr.second << endl;
    }

}


void Loop::show_LoopInfo(LoopInfo& info){
    cout << "head: " << info.head->mykey << endl;
    cout << "canUnroll: " << info.canUnroll << endl;
    
    cout << "preheader_added: " << info.preheader_added << endl;

    

    cout << "step: ";
    if(info.step){
        info.step->print();
    }
    cout << endl;

    cout << "times: ";
    if(info.times){
        info.times->print();
    }
    cout << endl;

    cout << "ini_iter: ";
    if(info.ini_iter){
        cout << info.ini_iter->to_string();
    }
    cout << endl;

    cout << "induce_temp: ";
    for(auto& t: info.induce_temp){
        cout << t->to_string() << " ";
    }
    cout << endl;

   

    cout << "var: ";
    for(auto& t: info.var){
        cout << t->to_string() << " ";
    }
    cout << endl;
    
    cout << "invar: ";
    for(auto& t: info.invar){
        cout << t->to_string() << " ";
    }
    cout << endl;

}

};