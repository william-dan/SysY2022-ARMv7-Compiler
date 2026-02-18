#include "gvn.hpp"

namespace ssa{

GlobalValueNumber::GlobalValueNumber(cfg::BlockGraph* ssa_ir,unordered_map<string, mir::Func_Info*>* func_infos): 
ssa_ir(ssa_ir), func_infos(func_infos)
{
    dtree = new DominatorTree(ssa_ir->mynodes);
    dfs_dtree(ssa_ir->entry);
    equal_temp_h.clear();
    assert(equal_temp.empty());
    assert(avail.empty());

    for(auto& n: *ssa_ir->mynodes){
        auto ssa_sl = ssa_ir->get_info(n);
        for(auto it = ssa_sl->begin(); it != ssa_sl->end(); it++){
            auto s = *it;
            if(s->kind == mir::StmType::MOVE){
                mem2temp(it, ssa_sl);
            }else if(s->kind == mir::StmType::EXP && static_cast<mir::ExpStm*>(s)->exp->kind == mir::ExpType::CALL){
                auto call = static_cast<mir::Call*>(static_cast<mir::ExpStm*>(s)->exp);
                if(func_infos->count(*call->name) && (*func_infos)[*call->name]->side_effect){
                    avail_mem.clear();
                }
            }
        }
        avail_mem.clear();
    }
    // dfs_dtree_for_mem(ssa_ir->entry);
    assert(avail_mem.empty());
    delete dtree;


}


void GlobalValueNumber::dfs_dtree(Node* curr){
    equal_stk.emplace(nullptr);
    // avail_mem_stk.emplace("");
    avail_stk.emplace("");
    auto ssa_sl = ssa_ir->get_info(curr);
    for(auto it = ssa_sl->begin(); it != ssa_sl->end(); it++){
        auto s = *it;
        if(s->kind == mir::StmType::MOVE){
            findavail(it, ssa_sl);
        }
        // else{
        //     auto use_refs = s->get_use_ref();
        //     for(auto& use_ref: use_refs){
        //         assert((*use_ref)->kind == mir::ExpType::TEMP);
        //         auto use_temp = static_cast<mir::Temp*>(*use_ref);
        //         use_temp->temp = find_root(use_temp->temp);
        //     }
        // }
    }
    assert(dtree->children.count(curr));
    for(auto& child: dtree->children[curr]){
        dfs_dtree(child);
    }
    while(equal_stk.top() != nullptr){
        auto top = equal_stk.top();
        equal_stk.pop();
        equal_temp.erase(top);
        equal_temp_h.erase(top);
    }
    equal_stk.pop();
    while(avail_stk.top() != ""){
        auto top = avail_stk.top();
        avail_stk.pop();
        avail.erase(top);
    }
    avail_stk.pop();
    // while(avail_mem_stk.top() != ""){
    //     auto top = avail_mem_stk.top();
    //     avail_mem_stk.pop();
    //     avail_mem.erase(top);
    // }
    // avail_mem_stk.pop();
}

utils::Temp* GlobalValueNumber::find_root(utils::Temp* curr){
    if(!equal_temp.count(curr)) return curr;
    equal_temp[curr] = find_root(equal_temp[curr]);
    return equal_temp[curr];
}
void GlobalValueNumber::union_temp(utils::Temp* t1, utils::Temp* t2){
    if(!equal_temp.count(t1)) equal_temp_h[t1] = 1;
    if(!equal_temp.count(t2)) equal_temp_h[t2] = 1;
    auto r1 = find_root(t1);
    auto r2 = find_root(t2);
    if(equal_temp_h[r1] > equal_temp_h[r2]){
        equal_temp[r2] = r1;
        equal_temp_h[r1] += equal_temp_h[r2];
    }else{
        equal_temp[r1] = r2;
        equal_temp_h[r2] += equal_temp_h[r1];
    }
}
//
//mem(a) = t1 + t2
//b = mem(a)
//
//

void GlobalValueNumber::findavail(mir::StmList::iterator& it, mir::StmList* sl){
    auto movstm = static_cast<mir::Move*>(*it);
    utils::Temp* ltemp = nullptr;
    auto oristm = movstm;

    if(movstm->dst->kind == mir::ExpType::MEM){
        auto mem = static_cast<mir::Mem*>(movstm->dst);
        if(mem->exp->kind == mir::ExpType::TEMP){
            auto mtemp = static_cast<mir::Temp*>(mem->exp);
            mtemp->temp = find_root(mtemp->temp);
        }
        if(
        movstm->src->kind == mir::ExpType::TEMP) return;
        ltemp = new utils::Temp(movstm->dst->get_type());
        auto newstm = new mir::Move(new mir::Temp(ltemp), movstm->src);
        movstm->src = new mir::Temp(ltemp);
        sl->emplace(it, newstm);
        movstm = newstm;
    }else{
        assert(movstm->dst->kind == mir::ExpType::TEMP);
        ltemp = static_cast<mir::Temp*>(movstm->dst)->temp;
    }
    auto e = movstm->src;
    switch (e->kind)
    {
    case mir::ExpType::MEM:
    {
        auto mem = static_cast<mir::Mem*>(e);
        // string idx;
        if(mem->exp->kind == mir::ExpType::TEMP){
            auto exp_temp = static_cast<mir::Temp*>(mem->exp);
            exp_temp->temp = find_root(exp_temp->temp);
        }
        // }else if(mem->exp->kind == mir::ExpType::NAME){
        //     idx = static_cast<mir::Name*>(mem->exp)->label->to_string();
        //     if(avail.count(idx)){
        //         idx = find_root(avail[idx])->to_string();
        //     }
        // }else{
        //     assert(0);
        // }
        // if(avail_mem.count(idx)){
        //     movstm->src = new mir::Temp(avail_mem[idx]);
        //     // equal_stk.emplace(ltemp);
        //     // union_temp(ltemp, avail_mem[idx]);
        // }else{
        //     avail_mem[idx] = find_root(ltemp);
        //     avail_mem_stk.emplace(idx);
        // }
    }
    break;
    case mir::ExpType::CALL:
    {
        // avail_mem.clear();
        // 没副作用，参数相同的call可以复用
        auto call = static_cast<mir::Call*>(e);
    
        if(!func_infos->count(*call->name)) break;
        auto func_info = (*func_infos)[*call->name];
        vector<string> symbols;
        if(!func_info->side_effect){
            for(auto& cexp: *call->args){
                string idx;
                switch (cexp->kind)
                {
                case mir::ExpType::CONST:
                {
                    idx = static_cast<mir::Const*>(cexp)->to_string();
                }
                break;
                case mir::ExpType::TEMP:
                {
                    auto ctemp = static_cast<mir::Temp*>(cexp);
                    ctemp->temp = find_root(ctemp->temp);
                    idx = find_root(ctemp->temp)->to_string();
                }
                break;
                default:
                assert(0);
                break;
                }
                symbols.emplace_back(idx);
            }
            string tot_idx = *call->name + "(";
            for(auto& symbol: symbols){
                tot_idx += symbol + ",";
            }
            tot_idx += ")";
            if(avail.count(tot_idx)){
                movstm->src = new mir::Temp(avail[tot_idx]);
                equal_stk.emplace(ltemp);
                union_temp(ltemp, avail[tot_idx]);
            }else{
                avail_stk.emplace(tot_idx);
                avail[tot_idx] = ltemp;
            }
        }
    }
    break;
    case mir::ExpType::PHI:
    {
        // for PRE
    }
    break;
    case mir::ExpType::NAME:
    {
        string idx = static_cast<mir::Name*>(e)->label->to_string();
        if(avail.count(idx)){
            movstm->src = new mir::Temp(avail[idx]);
            equal_stk.emplace(ltemp);
            union_temp(ltemp, avail[idx]);
        }else{
            avail_stk.emplace(idx);
            avail[idx] = ltemp;
        }

    }
    break;
    case mir::ExpType::TEMP:
    {
        equal_stk.emplace(ltemp);
        union_temp(ltemp, static_cast<mir::Temp*>(e)->temp);
        auto mtemp = static_cast<mir::Temp*>(e);
        mtemp->temp = find_root(mtemp->temp);
    }
    break;
    case mir::ExpType::BINOP:
    {
        auto bop = static_cast<mir::Binop*>(e);
        // bop->print();
        vector<string> symbols;
        for(auto& curr: vector<mir::Exp*>{bop->left, bop->right}){
            string idx;
            switch (curr->kind)
            {
            case mir::ExpType::CONST:
            {
                idx = static_cast<mir::Const*>(curr)->to_string();
            }
            break;
            case mir::ExpType::TEMP:
            {
                auto ctemp = static_cast<mir::Temp*>(curr);
                ctemp->temp = find_root(ctemp->temp);
                idx = find_root(ctemp->temp)->to_string();
            }
            break;
            default:
            assert(0);
            break;
            }
            symbols.emplace_back(idx);
        }
        string tot_idx = symbols[0] + " " + mir::binop2str(bop->op) + " " + symbols[1];
        if(avail.count(tot_idx)){
            movstm->src = new mir::Temp(avail[tot_idx]);
            equal_stk.emplace(ltemp);
            union_temp(ltemp, avail[tot_idx]);
        }else{
            avail_stk.emplace(tot_idx);
            avail[tot_idx] = ltemp;
        }
    }
    default:
        break;
    }
    
    // if(oristm->dst->kind == mir::ExpType::MEM){
    //     auto movmem = static_cast<mir::Mem*>(oristm->dst);
    //     string idx;
    //     if(movmem->exp->kind == mir::ExpType::TEMP){
    //         idx = find_root(static_cast<mir::Temp*>(movmem->exp)->temp)->to_string();
    //     }else if(movmem->exp->kind == mir::ExpType::NAME){
    //         idx = static_cast<mir::Name*>(movmem->exp)->label->to_string();
    //         if(avail.count(idx)){
    //             idx = find_root(avail[idx])->to_string();
    //         }
    //     }else{
    //         assert(0);
    //     }
    //     if(avail_mem.count(idx)){
    //         if(avail_mem[idx] == find_root(ltemp)){
    //             // (*it)->print();
    //             *it = new mir::ExpStm(new mir::Const(0));
    //             return;
    //         }
    //     }
    //     avail_mem.clear();
    //     avail_mem[idx] = find_root(ltemp);
    //     avail_mem_stk.emplace(idx);

    // }

}


void GlobalValueNumber::dfs_dtree_for_mem(Node* curr){
    avail_mem_stk.emplace("");
    auto ssa_sl = ssa_ir->get_info(curr);
    for(auto it = ssa_sl->begin(); it != ssa_sl->end(); it++){
        auto s = *it;
        if(s->kind == mir::StmType::MOVE){
            mem2temp(it, ssa_sl);
        }else if(s->kind == mir::StmType::EXP && static_cast<mir::ExpStm*>(s)->exp->kind == mir::ExpType::CALL){
            avail_mem.clear();
        }
    }
    assert(dtree->children.count(curr));
    for(auto& child: dtree->children[curr]){
        dfs_dtree_for_mem(child);
    }
    while(avail_mem_stk.top() != ""){
        auto top = avail_mem_stk.top();
        avail_mem_stk.pop();
        avail_mem.erase(top);
    }
    avail_mem_stk.pop();
}
void GlobalValueNumber::mem2temp(mir::StmList::iterator& it, mir::StmList* sl){
    auto movstm = static_cast<mir::Move*>(*it);
    if(movstm->dst->kind == mir::ExpType::MEM){
        auto mem = static_cast<mir::Mem*>(movstm->dst);
        string idx;
        switch (mem->exp->kind)
        {
        case mir::ExpType::TEMP:
        {
            idx = static_cast<mir::Temp*>(mem->exp)->temp->to_string();
        }
        break;
        case mir::ExpType::NAME:
        {
            idx = static_cast<mir::Name*>(mem->exp)->label->to_string();
        }
        break;
        default:
        assert(0);
        break;

        }
        if(avail_mem.count(idx)){
            auto avail_exp = avail_mem[idx];
            if(avail_exp->kind == movstm->src->kind){
                switch (movstm->src->kind)
                {
                case mir::ExpType::TEMP:
                {
                    if(static_cast<mir::Temp*>(movstm->src)->temp == static_cast<mir::Temp*>(avail_exp)->temp){
                        *it = new mir::ExpStm(new mir::Const(0));
                        return;
                    }
                }
                break;
                case mir::ExpType::CONST:
                {
                    if(static_cast<mir::Const*>(movstm->src)->n.i == static_cast<mir::Const*>(avail_exp)->n.i){
                        *it = new mir::ExpStm(new mir::Const(0));
                        return;
                    }
                }
                break;
                default:
                assert(0);
                    break;
                }
            }
        }
        avail_mem.clear();
        avail_mem[idx] = movstm->src;
        avail_mem_stk.emplace(idx);
        return;
    }
    assert(movstm->dst->kind == mir::ExpType::TEMP);
    if(movstm->src->kind == mir::ExpType::MEM){
        auto mem = static_cast<mir::Mem*>(movstm->src);
        string idx;
        if(mem->exp->kind == mir::ExpType::TEMP){
            idx = static_cast<mir::Temp*>(mem->exp)->temp->to_string();
        }else if(mem->exp->kind == mir::ExpType::NAME){
            idx = static_cast<mir::Name*>(mem->exp)->label->to_string();
        }else{
            assert(0);
        }
        if(avail_mem.count(idx)){
            movstm->src = avail_mem[idx]->exp_deepcopy();
        }else{
            avail_mem[idx] = movstm->dst;
            avail_mem_stk.emplace(idx);
        }
    }else if(movstm->src->kind == mir::ExpType::CALL){
        // avail_mem.clear();
        auto call = static_cast<mir::Call*>(movstm->src);
        if(func_infos->count(*call->name) && (*func_infos)[*call->name]->side_effect){
            avail_mem.clear();
        }
    }
}


}