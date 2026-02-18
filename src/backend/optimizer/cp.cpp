#include "cp.hpp"
#include <set>

namespace ssa{

mir::Const* calbinop(mir::Const* lhs, mir::BinOp op, mir::Const* rhs) {
    switch (op) {
    case mir::BinOp::PLUS:
        if (lhs->type == utils::TempType::INT && rhs->type == utils::TempType::INT) {return new mir::Const(lhs->n.i + rhs->n.i);}
        if (lhs->type == utils::TempType::INT && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.i + rhs->n.f);
        if (!(lhs->type == utils::TempType::INT) && rhs->type == utils::TempType::INT) return new mir::Const(lhs->n.f + rhs->n.i);
        if (!(lhs->type == utils::TempType::INT) && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.f + rhs->n.f);
        break;
    case mir::BinOp::MINUS:
        if (lhs->type == utils::TempType::INT && rhs->type == utils::TempType::INT) {return new mir::Const(lhs->n.i - rhs->n.i);}
        if (lhs->type == utils::TempType::INT && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.i - rhs->n.f);
        if (!(lhs->type == utils::TempType::INT) && rhs->type == utils::TempType::INT) return new mir::Const(lhs->n.f - rhs->n.i);
        if (!(lhs->type == utils::TempType::INT) && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.f - rhs->n.f);
        break;
    case mir::BinOp::MUL:
        if (lhs->type == utils::TempType::INT && rhs->type == utils::TempType::INT) {return new mir::Const(lhs->n.i * rhs->n.i);}
        if (lhs->type == utils::TempType::INT && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.i * rhs->n.f);
        if (!(lhs->type == utils::TempType::INT) && rhs->type == utils::TempType::INT) return new mir::Const(lhs->n.f * rhs->n.i);
        if (!(lhs->type == utils::TempType::INT) && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.f * rhs->n.f);
        break;
    case mir::BinOp::DIV:
        if (lhs->type == utils::TempType::INT && rhs->type == utils::TempType::INT) {return new mir::Const(lhs->n.i / rhs->n.i);}
        if (lhs->type == utils::TempType::INT && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.i / rhs->n.f);
        if (!(lhs->type == utils::TempType::INT) && rhs->type == utils::TempType::INT) return new mir::Const(lhs->n.f / rhs->n.i);
        if (!(lhs->type == utils::TempType::INT) && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.f / rhs->n.f);
        break;
    case mir::BinOp::MOD:
        if (lhs->type == utils::TempType::INT && rhs->type == utils::TempType::INT) return new mir::Const(lhs->n.i % rhs->n.i);
        break;
    default:;
    }
    return new::mir::Const(0);
}

std::pair<bool, mir::Const*> ConstPropagation::is_const(mir::Exp* exp){
    switch (exp->kind)
    {
    case mir::ExpType::BINOP:{
        auto binopexp = static_cast<mir::Binop*>(exp);
        mir::Const* left; 
        mir::Const* right;
        if(binopexp->left->kind ==  mir::ExpType::TEMP) {
            auto tempexp = static_cast<mir::Temp*>(binopexp->left);
            if(temp_status[tempexp->temp] != TempStatus::Const){
                return std::make_pair<bool, mir::Const*>(false, NULL);
            }else{
                left = temp_const[tempexp->temp];
            }
        }else if(binopexp->left->kind !=  mir::ExpType::CONST){
            return std::make_pair<bool, mir::Const*>(false, NULL);
        }else{
            left = static_cast<mir::Const*>(binopexp->left);
        }
        if(binopexp->right->kind ==  mir::ExpType::TEMP){
            auto tempexp = static_cast<mir::Temp*>(binopexp->right);
            if(temp_status[tempexp->temp] != TempStatus::Const){
                return std::make_pair<bool, mir::Const*>(false, NULL);
            }else{
                right = temp_const[tempexp->temp];
            }
        }else if(binopexp->right->kind !=  mir::ExpType::CONST){
            return std::make_pair<bool, mir::Const*>(false, NULL);
        }else{
            right = static_cast<mir::Const*>(binopexp->right);
        }
        std::pair<bool, mir::Const*> ret;
        ret.first = true;
        ret.second = calbinop(left, binopexp->op, right);
        return ret;
        break;
    }
    case mir::ExpType::TEMP:{
        auto tempexp = static_cast<mir::Temp*>(exp);
        auto temp = tempexp->temp;
        if(temp_status[tempexp->temp] != TempStatus::Const){
            return std::make_pair<bool, mir::Const*>(false, NULL);
        }else{
            mir::Const* temp_c = temp_const[tempexp->temp];
            std::pair<bool, mir::Const*> ret;
            ret.first = true;
            ret.second = temp_c;
            return ret;
        }
        break;
    }
    case mir::ExpType::CONST:{
        return std::make_pair<bool, mir::Const*>(true, static_cast<mir::Const*>(exp));
        break;
    }
    case mir::ExpType::MEM:{
        return std::make_pair<bool, mir::Const*>(false, NULL);
        break;
    }
    case mir::ExpType::CALL:{
        return std::make_pair<bool, mir::Const*>(false, NULL);
        break;
    }
    default:
        return std::make_pair<bool, mir::Const*>(false, NULL);
        break;
    }
};

void ConstPropagation::replace_stm_temp(mir::Stm* stm){
    switch (stm->kind) {
    case mir::StmType::MOVE: {
        auto movstm = static_cast<mir::Move*>(stm);
        auto exp = movstm->src;
        switch (exp->kind)
        {
        case mir::ExpType::PHI:{
            auto phiexp = static_cast<mir::Phi*>(exp);
            for(auto temp: *phiexp->tl){
                if(temp_const.count(temp)){
                    phiexp->temp_const_map[temp] = temp_const[temp];
                }
            }
            break;
        }
        case mir::ExpType::BINOP:{
            auto binopexp = static_cast<mir::Binop*>(exp);
            auto left_const = is_const(binopexp->left);
            if(left_const.first) {
                binopexp->left = left_const.second;
            }
            auto right_const = is_const(binopexp->right);
            if(right_const.first) {
                binopexp->right = right_const.second;
            }
            break;
        }
        case mir::ExpType::TEMP:{
            auto src_const = is_const(movstm->src);
            if(src_const.first) {
                movstm->src = src_const.second;
            }
            break;
        }
        case mir::ExpType::MEM:{
            auto memexp = static_cast<mir::Mem*>(exp);
            auto src_const = is_const(memexp->exp);
            if(src_const.first) {
                memexp->exp = src_const.second;
            }
            break;
        }
        case mir::ExpType::CALL:{
            auto callexp = static_cast<mir::Call*>(exp);
            auto args = callexp->args;
            for(auto& argexp: *args){
                auto arg_const = is_const(argexp);
                if(arg_const.first) {
                    argexp = arg_const.second;
                }
            }
            break;
        }
        default:
            break;
        }
        if (movstm->dst->kind == mir::ExpType::MEM) {
            auto memexp = static_cast<mir::Mem*>(movstm->dst);
            auto src_const = is_const(memexp->exp);
            if(src_const.first) {
                memexp->exp = src_const.second;
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
                auto arg_const = is_const(argexp);
                if(arg_const.first) {
                    argexp = arg_const.second;
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
            auto left_const = is_const(binopexp->left);
            if(left_const.first) {
                binopexp->left = left_const.second;
            }
            auto right_const = is_const(binopexp->right);
            if(right_const.first) {
                binopexp->right = right_const.second;
            }
            break;
        }
        case mir::ExpType::TEMP:{
            auto src_const = is_const(retstm->exp);
            if(src_const.first) {
                retstm->exp = src_const.second;
            }
            break;
        }
        case mir::ExpType::MEM:{
            auto memexp = static_cast<mir::Mem*>(exp);
            auto src_const = is_const(memexp->exp);
            if(src_const.first) {
                memexp->exp = src_const.second;
            }
            break;
        }
        case mir::ExpType::CALL:{
            auto callexp = static_cast<mir::Call*>(exp);
            auto args = callexp->args;
            for(auto& argexp: *args){
                auto arg_const = is_const(argexp);
                if(arg_const.first) {
                    argexp = arg_const.second;
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
        auto left_const = is_const(cjumpstm->left);
            if(left_const.first) {
                cjumpstm->left = left_const.second;
            }
            auto right_const = is_const(cjumpstm->right);
            if(right_const.first) {
                cjumpstm->right = right_const.second;
            }
        break;
    }
    default:
    break;
    }
}

void ConstPropagation::replace_temp(){
    for(auto& node: *nodelist){
        mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
        for(auto& stm: *stmlist){
            replace_stm_temp(stm);
        }
    }
}

void ConstPropagation::scp(){
    // std::cout<<"scp"<<std::endl;
    std::set<mir::Stm**> worklist;
    for(auto& node: *nodelist){
        mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
        for(auto& stm: *stmlist){
            worklist.insert(&stm);
            auto uses = get_uses(stm);
            for(auto use: uses){
                auto tempexp = static_cast<mir::Temp*>(use);
                use_stm[tempexp->temp].emplace_back(&stm);
            }
        }
    }
    while(!worklist.empty()){
        auto stmptr = *worklist.begin();
        worklist.erase(worklist.begin());
        auto def = get_def(*stmptr);
        if(!def) continue;
        auto movstm = static_cast<mir::Move*>(*stmptr);
        auto exp = movstm->src;
        auto is_c = is_const(exp);
        if(!is_c.first) continue;
        auto tempexp = static_cast<mir::Temp*>(def);
        temp_status[tempexp->temp] =  TempStatus::Const;
        
        temp_const[tempexp->temp] = is_c.second;
        for(auto use_s: use_stm[tempexp->temp]){
            worklist.insert(use_s);
        }
        *stmptr = new mir::ExpStm(new mir::Const(0));
    }


}

}//namespace ssa