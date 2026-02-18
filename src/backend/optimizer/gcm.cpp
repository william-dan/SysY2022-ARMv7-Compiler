#include "gcm.hpp"
namespace ssa{

bool GlobalCodeMotion::has_side_effect(mir::Stm* stm){
    // if(array_init.count(stm)) return true;
    switch (stm->kind) {
        case mir::StmType::MOVE: {
            auto movstm = static_cast<mir::Move*>(stm);
            auto exp = movstm->src;
            switch (exp->kind){
            case mir::ExpType::CALL:{
                return true;
                break;
            }
            case mir::ExpType::PHI:{
                return true;
                break;
            }
            case mir::ExpType::MEM:{
                return true;
                break;
            }
            case mir::ExpType::BINOP:{
                auto binopexp = static_cast<mir::Binop*>(movstm->src);
                if(binopexp->left->kind == mir::ExpType::MEM) return true;
                if(binopexp->right->kind == mir::ExpType::MEM) return true;
                break;
            }
            default:
                break;
            }
            if (movstm->dst->kind == mir::ExpType::MEM) {
                return true;
            }
            // if (movstm->dst->kind == mir::ExpType::TEMP) {
            //     return false;
            // }
        }
        case mir::StmType::EXP: {
            auto expstm = static_cast<mir::ExpStm*>(stm);
            if (expstm->exp->kind == mir::ExpType::CALL) {
                return true;
            }
            break;
        }
        case mir::StmType::CJUMP: {
            return true;
            break;
        }
        case mir::StmType::JUMP: {
            return true;
            break;
        }
        case mir::StmType::LABEL: {
            return true;
            break;
        }
        case mir::StmType::RETURN: {
            return true;
            break;
        }
    }
    return false;
}

utils::TempSet get_uses_with_phi(mir::Stm* stm){
    utils::TempSet uses;
    switch (stm->kind) {
    case mir::StmType::MOVE: {
        auto movstm = static_cast<mir::Move*>(stm);
        auto exp = movstm->src;
        switch (exp->kind)
        {
        case mir::ExpType::BINOP:{
            auto binopexp = static_cast<mir::Binop*>(exp);
            if(binopexp->left->kind ==  mir::ExpType::TEMP) {
                uses.insert(static_cast<mir::Temp*>(binopexp->left)->temp);
            }
            if(binopexp->right->kind ==  mir::ExpType::TEMP){
                uses.insert(static_cast<mir::Temp*>(binopexp->right)->temp);
            }
            break;
        }
        case mir::ExpType::TEMP:{
            uses.insert(static_cast<mir::Temp*>(exp)->temp);
            break;
        }
        case mir::ExpType::MEM:{
            auto memexp = static_cast<mir::Mem*>(exp);
            if(memexp->exp->kind ==  mir::ExpType::TEMP) {
                uses.insert(static_cast<mir::Temp*>(memexp->exp)->temp);
            }
            break;
        }
        case mir::ExpType::CALL:{
            auto callexp = static_cast<mir::Call*>(exp);
            auto args = callexp->args;
            for(auto& argexp: *args){
                if(argexp->kind ==  mir::ExpType::TEMP){
                    uses.insert(static_cast<mir::Temp*>(argexp)->temp);
                }
            }
            break;
        }
        case mir::ExpType::PHI:{
            auto phiexp = static_cast<mir::Phi*>(exp);
            for(auto temp: *phiexp->tl){
                if(phiexp->temp_const_map.count(temp) == 0) uses.insert(temp);
            }
            break;
        }
        default:
            break;
        }
        if (movstm->dst->kind == mir::ExpType::MEM) {
            auto memexp = static_cast<mir::Mem*>(movstm->dst);
            if(memexp->exp->kind ==  mir::ExpType::TEMP){
                uses.insert(static_cast<mir::Temp*>(memexp->exp)->temp);
            }
        }
        break;
    }
    case mir::StmType::EXP: {
        auto expstm = static_cast<mir::ExpStm*>(stm);
        if (expstm->exp->kind == mir::ExpType::CALL) {
            auto callexp = static_cast<mir::Call*>(expstm->exp);
            auto args = callexp->args;
            for(auto& argexp: *args){
                if(argexp->kind ==  mir::ExpType::TEMP){
                    uses.insert(static_cast<mir::Temp*>(argexp)->temp);
                }
            }
        }
        break;
    }
    case mir::StmType::RETURN: {
        auto retstm = static_cast<mir::Return*>(stm);
        auto exp = retstm->exp;
        switch (exp->kind)
        {
        case mir::ExpType::BINOP:{
            auto binopexp = static_cast<mir::Binop*>(exp);
            if(binopexp->left->kind ==  mir::ExpType::TEMP) {
                uses.insert(static_cast<mir::Temp*>(binopexp->left)->temp);
            }
            if(binopexp->right->kind ==  mir::ExpType::TEMP){
                uses.insert(static_cast<mir::Temp*>(binopexp->right)->temp);
            }
            break;
        }
        case mir::ExpType::TEMP:{
            uses.insert(static_cast<mir::Temp*>(exp)->temp);
            break;
        }
        case mir::ExpType::MEM:{
            auto memexp = static_cast<mir::Mem*>(exp);
            if(memexp->exp->kind ==  mir::ExpType::TEMP) {
                uses.insert(static_cast<mir::Temp*>(memexp->exp)->temp);
            }
            break;
        }
        case mir::ExpType::CALL:{
            auto callexp = static_cast<mir::Call*>(exp);
            auto args = callexp->args;
            for(auto& argexp: *args){
                if(argexp->kind ==  mir::ExpType::TEMP){
                    uses.insert(static_cast<mir::Temp*>(argexp)->temp);
                }
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case mir::StmType::CJUMP: {
        auto cjumpstm = static_cast<mir::Cjump*>(stm);
        if(cjumpstm->left->kind ==  mir::ExpType::TEMP){
            uses.insert(static_cast<mir::Temp*>(cjumpstm->left)->temp);
        }
        if(cjumpstm->right->kind ==  mir::ExpType::TEMP){
            uses.insert(static_cast<mir::Temp*>(cjumpstm->right)->temp);
        }
        break;
    }
    }
    return uses;

};

mir::Exp* contain_mem(mir::Stm* stm){
    switch (stm->kind) {
    case mir::StmType::MOVE: {
        auto movstm = static_cast<mir::Move*>(stm);
        auto exp = movstm->src;
        switch (exp->kind)
        {
        case mir::ExpType::BINOP:{
            auto binopexp = static_cast<mir::Binop*>(exp);
            if(binopexp->left->kind ==  mir::ExpType::MEM) {
                return binopexp->left;
            }
            if(binopexp->right->kind ==  mir::ExpType::MEM){
                return binopexp->right;
            }
            break;
        }
        case mir::ExpType::TEMP:{
            break;
        }
        case mir::ExpType::MEM:{
            return exp;
            break;
        }
        case mir::ExpType::CALL:{
            break;
        }
        case mir::ExpType::PHI:{
        }
        default:
            break;
        }
        if (movstm->dst->kind == mir::ExpType::MEM) {
          return movstm->dst;
        }
        break;
    }
    case mir::StmType::EXP: {
        auto expstm = static_cast<mir::ExpStm*>(stm);
        if (expstm->exp->kind == mir::ExpType::MEM) {
            return expstm->exp;            
        }
        break;
    }
    case mir::StmType::RETURN: {
        auto retstm = static_cast<mir::Return*>(stm);
        auto exp = retstm->exp;
        switch (exp->kind)
        {
        case mir::ExpType::BINOP:{
            auto binopexp = static_cast<mir::Binop*>(exp);
            if(binopexp->left->kind ==  mir::ExpType::MEM) {
                return binopexp->left;
            }
            if(binopexp->right->kind ==  mir::ExpType::MEM){
                return binopexp->right;
            }
            break;
        }
        case mir::ExpType::TEMP:{
            break;
        }
        case mir::ExpType::MEM:{
          return exp;
            break;
        }
        case mir::ExpType::CALL:{
            break;
        }
        default:
            break;
        }
        break;
    }
    case mir::StmType::CJUMP: {
        auto cjumpstm = static_cast<mir::Cjump*>(stm);
        if(cjumpstm->left->kind ==  mir::ExpType::MEM){
            return cjumpstm->left;        
        }
        if(cjumpstm->right->kind ==  mir::ExpType::MEM){
            return cjumpstm->left;        
        }
        break;
    }
    }
    return NULL;

};

GlobalCodeMotion::GlobalCodeMotion(NodeList* nodelist)
:nodelist(nodelist)
{
    //init
    domi_tree = new DominatorTree(nodelist);
    domi_tree->compute_depth(nodelist->front(), 0);
    domi_depth = domi_tree->domi_depth;
    auto loop_tree = new Loop(nodelist);
    loop_depth = loop_tree->n2dep;
    for(auto node: *nodelist){
        mir::Stm* pres = NULL;
        mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
        for(auto& stm: *stmlist){
            // std::cout << "de in work"<< std::endl;
            stm_to_block[stm] = node;
            auto def = get_def(stm);
            if(def){
                def_stm[static_cast<mir::Temp*>(def)->temp] = stm;
            }
            auto uses = get_uses_with_phi(stm);
            for(auto use: uses){
                use_stms[use].push_back(stm);
            }
            
            if(!pres){
                pres = stm;
                continue;
            }
            auto mem = contain_mem(stm);
            if(mem) {
                assert(mem->kind == mir::ExpType::MEM);
                auto memexp = static_cast<mir::Mem*>(mem);
                if(memexp->exp->kind ==  mir::ExpType::TEMP) {
                    auto memtemp = static_cast<mir::Temp*>(memexp->exp)->temp;
                    if(def_stm.count(memtemp)){
                        auto memtempdef = def_stm[memtemp];
                        assert(memtempdef->kind == mir::StmType::MOVE);
                        auto movstm = static_cast<mir::Move*>(memtempdef);
                        if(movstm->src->kind == mir::ExpType::BINOP) array_init.insert(memtempdef);
                    }   
                }
            };
            

        }
    }

    // schedule early
    // for(auto node: *nodelist){
    //     mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
    //     for(auto& stm: *stmlist){
    //         if(visited.count(stm)) continue;
    //         if(!has_side_effect(stm)) continue;
    //         visited.insert(stm);
    //         auto uses = get_uses_with_phi(stm);
    //         for(auto use: uses){
    //             if(def_stm.count(use) == 0) continue;
    //             auto x = def_stm[use];
    //             auto x_block = schedule_early(x);
    //         }
    //     }
    // }
    // visited.clear();
    // schedule late
    for(auto node: *nodelist){
        mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
        for(auto& stm: *stmlist){
            if(visited.count(stm)) continue;
            if(!has_side_effect(stm)) continue;
            visited.insert(stm);
            auto def = get_def(stm);
            if(!def) continue;
            if(use_stms.count(static_cast<mir::Temp*>(def)->temp)){
                for(auto use_stm: use_stms[static_cast<mir::Temp*>(def)->temp]){
                    auto x_block = schedule_late(use_stm);
                }
            }
        }
    }
}

Node* GlobalCodeMotion::schedule_early(mir::Stm* stm){
    // std::cout << "stm";
    // stm->print();
    if(visited.count(stm)) return stm_to_block[stm];
    if(has_side_effect(stm)) return stm_to_block[stm];
    visited.insert(stm);
    if(array_init.count(stm)) return stm_to_block[stm];
    auto uses = get_uses_with_phi(stm);
    auto i_block = nodelist->front();
    for(auto use: uses){
        if(def_stm.count(use) == 0) return stm_to_block[stm];
        auto x = def_stm[use];
        auto x_block = schedule_early(x);
        if(domi_depth[x_block] > domi_depth[i_block]) i_block = x_block;
    }
    auto o_block = stm_to_block[stm];
    if(o_block != i_block){
        // std::cout << "****stm in process***" << std::endl;
        // stm->print();
        // std::cout << o_block->mykey << "to" << i_block->mykey << std::endl;
        mir::StmList* stmlist = std::any_cast<mir::StmList*>(o_block->info);
        stmlist->remove(stm);
        stm_to_block[stm] = i_block;
        stmlist = std::any_cast<mir::StmList*>(i_block->info);
        auto insert_pos = stmlist->end();
        insert_pos--;
        stmlist->insert(insert_pos, stm);  
    }
    return i_block;
}
    
Node* GlobalCodeMotion::lca(Node* a, Node* b){
    if(a == NULL) return b;
    while (domi_depth[a] > domi_depth[b]) {
      a = domi_tree->idom[a];
    }
    while (domi_depth[a] < domi_depth[b]) {
      b = domi_tree->idom[b];
    }
    while (a != b) {
        a = domi_tree->idom[a];
        b = domi_tree->idom[b];
    }
    return a;
};
Node* GlobalCodeMotion::schedule_late(mir::Stm* stm){
    if(visited.count(stm)) return stm_to_block[stm];
    visited.insert(stm);
    if(stm->kind!=mir::StmType::MOVE) return stm_to_block[stm];
    Node* lca_node = NULL;
    auto def = get_def(stm);
    if(!def) return stm_to_block[stm];
    auto movstm = static_cast<mir::Move*>(stm);
    for(auto use_stm: use_stms[static_cast<mir::Temp*>(def)->temp]){
        auto use_block = schedule_late(use_stm);
        if(is_a_phi(use_stm)){
            // return stm_to_block[stm];
            auto phistm = static_cast<mir::Move*>(use_stm);
            auto phiexp = static_cast<mir::Phi*>(phistm->src);
            auto pred_iter = stm_to_block[use_stm]->preds->begin();
            for(auto temp: *phiexp->tl){
                if(temp == static_cast<mir::Temp*>(def)->temp){
                    use_block = *pred_iter;
                    lca_node = lca(lca_node, use_block);
                }
                pred_iter++;
            }
        }
        lca_node = lca(lca_node, use_block);
    }
    if(has_side_effect(stm)) return stm_to_block[stm];
    if(lca_node == NULL) return stm_to_block[stm];
    auto best_node = lca_node;
    auto iter_node = lca_node;
    while(iter_node != stm_to_block[stm]){
        if(loop_depth[iter_node] < loop_depth[best_node]){
            best_node = iter_node;
        }
        iter_node = domi_tree->idom[iter_node];
    }
    auto o_block = stm_to_block[stm];
    assert(best_node);
    if(o_block != best_node){        
        mir::StmList* stmlist = std::any_cast<mir::StmList*>(o_block->info);
        stmlist->remove(stm);
        stm_to_block[stm] = best_node;
        stmlist = std::any_cast<mir::StmList*>(best_node->info);
        auto insert_pos = stmlist->begin();
        insert_pos++;
        stmlist->insert(insert_pos, stm); 
    }
    return stm_to_block[stm];
}

}