#include "build_ssa.hpp"
#include "../../structure/mir/mir.hpp"
#include <vector>
namespace ssa{
bool is_a_phi(mir::Stm* stm){
    if(stm->kind == mir::StmType::MOVE){
        auto movstm = static_cast<mir::Move*>(stm);
        if(movstm->src->kind == mir::ExpType::PHI) return true;
    }
    return false;
}
mir::Exp* get_def(mir::Stm* stm){
    if (stm->kind == mir::StmType::MOVE) {
        auto movstm = static_cast<mir::Move*>(stm);
        if (movstm->dst->kind == mir::ExpType::TEMP) {
            auto tempexp = static_cast<mir::Temp*>(movstm->dst);
            movstm->dst = new mir::Temp(tempexp->temp);
            return movstm->dst;
        }
    }
    return NULL;
};

mir::Temp* new_temp_exp(mir::Exp* exp){
    auto tempexp = static_cast<mir::Temp*>(exp);
    return new mir::Temp(tempexp->temp);
    return tempexp;
}

mir::ExpList get_uses(mir::Stm* stm){
    mir::ExpList uses;
    switch (stm->kind) {
    case mir::StmType::MOVE: {
        auto movstm = static_cast<mir::Move*>(stm);
        auto exp = movstm->src;
        switch (exp->kind)
        {
        case mir::ExpType::BINOP:{
            auto binopexp = static_cast<mir::Binop*>(exp);
            if(binopexp->left->kind ==  mir::ExpType::TEMP) {
                binopexp->left = new_temp_exp(binopexp->left);
                uses.push_back(binopexp->left);
            }
            if(binopexp->right->kind ==  mir::ExpType::TEMP){
                binopexp->right = new_temp_exp(binopexp->right);
                uses.push_back(binopexp->right);
            }
            break;
        }
        case mir::ExpType::TEMP:{
            movstm->src = new_temp_exp(exp);
            uses.push_back(movstm->src);
            break;
        }
        case mir::ExpType::MEM:{
            auto memexp = static_cast<mir::Mem*>(exp);
            if(memexp->exp->kind ==  mir::ExpType::TEMP) {
                memexp->exp = new_temp_exp(memexp->exp);
                uses.push_back(memexp->exp);
            }
            break;
        }
        case mir::ExpType::CALL:{
            auto callexp = static_cast<mir::Call*>(exp);
            auto args = callexp->args;
            for(auto& argexp: *args){
                if(argexp->kind ==  mir::ExpType::TEMP){
                    argexp = new_temp_exp(argexp);
                    uses.push_back(argexp);
                }
            }
            break;
        }
        default:
            break;
        }
        if (movstm->dst->kind == mir::ExpType::MEM) {
            auto memexp = static_cast<mir::Mem*>(movstm->dst);
            if(memexp->exp->kind ==  mir::ExpType::TEMP){
                memexp->exp = new_temp_exp(memexp->exp);
                uses.push_back(memexp->exp);
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
                    argexp = new_temp_exp(argexp);
                    uses.push_back(argexp);
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
                binopexp->left = new_temp_exp(binopexp->left);
                uses.push_back(binopexp->left);
            }
            if(binopexp->right->kind ==  mir::ExpType::TEMP){
                binopexp->right = new_temp_exp(binopexp->right);
                uses.push_back(binopexp->right);
            }
            break;
        }
        case mir::ExpType::TEMP:{
            retstm->exp = new_temp_exp(exp);
            uses.push_back(retstm->exp);
            break;
        }
        case mir::ExpType::MEM:{
            auto memexp = static_cast<mir::Mem*>(exp);
            if(memexp->exp->kind ==  mir::ExpType::TEMP) {
                memexp->exp = new_temp_exp(memexp->exp);
                uses.push_back(memexp->exp);
            }
            break;
        }
        case mir::ExpType::CALL:{
            auto callexp = static_cast<mir::Call*>(exp);
            auto args = callexp->args;
            for(auto& argexp: *args){
                if(argexp->kind ==  mir::ExpType::TEMP){
                    argexp = new_temp_exp(argexp);
                    uses.push_back(argexp);
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
            cjumpstm->left = new_temp_exp(cjumpstm->left);
            uses.push_back(cjumpstm->left);
        }
        if(cjumpstm->right->kind ==  mir::ExpType::TEMP){
            cjumpstm->right = new_temp_exp(cjumpstm->right);
            uses.push_back(cjumpstm->right);
        }
        break;
    }
    }
    return uses;

};

utils::TempList* Ssa::get_origin_def(Node* n){
    utils::TempList* ret = new utils::TempList();
    mir::StmList* stmlist = std::any_cast<mir::StmList*>(n->info);
    for(auto& stm: *stmlist){
        if (stm->kind == mir::StmType::MOVE) {
                auto movstm = static_cast<mir::Move*>(stm);
                if (movstm->dst->kind == mir::ExpType::TEMP) {
                    auto tempexp = static_cast<mir::Temp*>(movstm->dst);
                    ret->push_back(tempexp->temp);
                    origin_defs[n].insert(tempexp->temp);
                    defsites[tempexp->temp].push_back(n);
                }
            }
    }
    return ret;
} 


void Ssa::place_phi(){
    //for every temp
    for(auto iter = defsites.begin();iter!=defsites.end();iter++){
        auto temp = iter->first;
        std::set<Node*> worklist;
        std::set<Node*> has_phi;
        for(auto& node: iter->second) worklist.insert(node);
        while(!worklist.empty()){
            auto noden = *worklist.begin();
            worklist.erase(noden);
            auto node_df = domi_tree->dominance_frontier[noden];
            // auto node_df = domi_table[noden]->dominance_frontier;
            for(auto& nodey: node_df){
                if(has_phi.count(nodey) == 0){
                    //place phi
                    auto preds = nodey->preds;
                    // std::vector<utils::Temp*>* temp_param = new std::vector<utils::Temp*>();
                    // std::vector<utils::Label*>* label_param = new std::vector<utils::Label*>();
                    utils::TempList* temp_param = new utils::TempList();
                    utils::LabelList* label_param = new utils::LabelList();
                    for(auto &pred: *preds){
                        mir::StmList* stmlist = std::any_cast<mir::StmList*>(pred->info);
                        mir::Label* labelstm = static_cast<mir::Label*>(*stmlist->begin());
                        temp_param->push_back(temp);
                        label_param->push_back(labelstm->label);
                    }
                    mir::Move* phistm = new mir::Move(new mir::Temp(temp),
                        new mir::Phi(temp_param, label_param));
                    mir::StmList* stmlist = std::any_cast<mir::StmList*>(nodey->info);
                    auto insertpos = stmlist->begin();
                    insertpos++;
                    stmlist->insert(insertpos, phistm);
                    has_phi.insert(nodey);
                    if(origin_defs[nodey].count(temp) == 0) {
                        worklist.insert(nodey);
                        origin_defs[nodey].insert(temp);
                    }
                }
            }
        }
    }
}

void Ssa::rename_single_node(Node* n){
    if(done.count(n)) return;
    done.insert(n);
    utils::TempList renamed_temps;
    // std::cout<<"rename node "<< n->mykey<<std::endl; 
    mir::StmList* stmlist = std::any_cast<mir::StmList*>(n->info);
    for(auto& stm: *stmlist){
        mir::Exp* def = get_def(stm);
        mir::ExpList uses = get_uses(stm);
        if(stm->kind == mir::StmType::MOVE){
            auto movstm = static_cast<mir::Move*>(stm);
            //already phi
            if(movstm->src->kind == mir::ExpType::PHI){
            }else{
                for(auto& use: uses){
                if(stacks[static_cast<mir::Temp*>(use)->temp].empty()){
                    std::cerr<<"wrong"<<std::endl;
                    std::cerr<<static_cast<mir::Temp*>(use)->temp->to_string() <<std::endl;
                }
                static_cast<mir::Temp*>(use)->temp = stacks[static_cast<mir::Temp*>(use)->temp].top();
            }
         };
        }else{
            for(auto& use: uses){
                if(stacks[static_cast<mir::Temp*>(use)->temp].empty()){
                    std::cerr<<"wrong"<<std::endl;
                    std::cerr<<static_cast<mir::Temp*>(use)->temp->to_string() <<std::endl;
                }
                static_cast<mir::Temp*>(use)->temp = stacks[static_cast<mir::Temp*>(use)->temp].top();
            }
            
        }
        if(def){
            utils::Temp* new_temp = new utils::Temp(static_cast<mir::Temp*>(def)->temp->type);
            renamed_temps.push_back(static_cast<mir::Temp*>(def)->temp);
            stacks[static_cast<mir::Temp*>(def)->temp].push(new_temp);
            auto movstm = static_cast<mir::Move*>(stm);
            movstm->dst = new mir::Temp(new_temp);
            
        }
        // stm->print();
    }
    // std::cout<<"not phi done"<<std::endl;
    auto succs = n->succs;
    for(auto& y: *succs){
        int count = 0;
        for(auto &pred: *y->preds){
            if(pred == n){
                break;
            }
            count++;
        }
        mir::StmList* y_stmlist = std::any_cast<mir::StmList*>(y->info);
        auto n_label_stm = static_cast<mir::Label*>(stmlist->front());
        for(auto& y_stm: *y_stmlist){
            if(y_stm->kind == mir::StmType::MOVE){
                auto movstm = static_cast<mir::Move*>(y_stm);
                // phi
                if(movstm->src->kind == mir::ExpType::PHI){
                    auto phiexp = static_cast<mir::Phi*>(movstm->src);
                    auto insertpos = phiexp->tl->begin();
                    auto labelpos = phiexp->ll->begin();
                    for(int i = 0; i < count; i++){
                        insertpos++;
                        labelpos++;
                    }
                    assert(*n_label_stm->label->id == *(*labelpos)->id);
                    *insertpos= stacks[*insertpos].top();
                }
            }
        }
    }
    // std::cout<<"phi done"<<std::endl;
    for(auto& succ: *n->succs){
        rename_single_node(succ);
    }

    for(auto& temp: renamed_temps){
        stacks[temp].pop();
    }
 
}

void Ssa::rename(){
    for(auto& iter: defsites){
        stacks[iter.first].push(iter.first);
    }
    for(auto& iter: *fun_args){
        if(stacks.count(iter) == 0) stacks[iter].push(iter);
    }
    
        rename_single_node(nodelist->front());
}

void Ssa::back_to_ir(){
    for(auto &node: *nodelist){
        if(node->preds->size() <= 1) continue; 
        mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
        for(auto& stm: *stmlist){
            if(stm->kind == mir::StmType::MOVE){
                auto movstm = static_cast<mir::Move*>(stm);
                //a phi
                if(movstm->src->kind == mir::ExpType::PHI){
                    auto phiexp = static_cast<mir::Phi*>(movstm->src);
                    auto dsttemp = static_cast<mir::Temp*>(movstm->dst);
                    auto srcpos = phiexp->tl->begin();
                    for(auto pred: *node->preds){
                        mir::StmList* presl = std::any_cast<mir::StmList*>(pred->info);
                        auto insetpos = presl->end();
                        insetpos--;
                        mir::Exp* src;
                        if(phiexp->temp_const_map.count(*srcpos)){
                            src = phiexp->temp_const_map[*srcpos];
                        }else{
                            src = new mir::Temp(*srcpos);
                        }
                        if(dsttemp->temp != *srcpos)
                        presl->insert(insetpos, new mir::Move(new mir::Temp(dsttemp->temp), src));
                        srcpos++;
                    }
                    delete stm;
                    stm = new mir::ExpStm(new mir::Const(0));
                };
            }
        }
    }
    
};

void Ssa::remove_empty(){
    for(auto &node: *nodelist){
        mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
        for(auto stm_iter = stmlist->begin(); stm_iter != stmlist->end(); ){
            auto stm = *stm_iter;
            if(stm->kind == mir::StmType::EXP){
                auto exp = static_cast<mir::ExpStm*>(stm)->exp;
                if(exp->kind == mir::ExpType::CONST){
                    delete exp;
                    delete stm;
                    stm_iter = stmlist->erase(stm_iter);
                    continue;
                }
            }
            stm_iter++;
        }
    }
}

} // namespace ssa