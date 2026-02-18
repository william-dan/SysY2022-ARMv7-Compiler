#include "fi.hpp"
namespace mir{
using namespace std;

bool is_call(Stm* stm){
    if(stm->kind == StmType::MOVE){
        auto movstm = static_cast<Move*>(stm);
        if(movstm->src->kind == ExpType::CALL){
            return true;
        }
    }
    if(stm->kind == StmType::EXP){
        auto expstm = static_cast<ExpStm*>(stm);
        if(expstm->exp->kind == ExpType::CALL){
            return true;
        }
    }
    return false;
}
std::string get_call_name(Stm* stm){
    if(stm->kind == StmType::MOVE){
        auto movstm = static_cast<Move*>(stm);
        if(movstm->src->kind == ExpType::CALL){
            return *static_cast<mir::Call*>(movstm->src)->name;
        }
    }
    if(stm->kind == StmType::EXP){
        auto expstm = static_cast<ExpStm*>(stm);
        if(expstm->exp->kind == ExpType::CALL){
            return *static_cast<mir::Call*>(expstm->exp)->name;
        }
    }
    return nullptr;
}

mir::ExpList* get_call_params(Stm* stm){
    if(stm->kind == StmType::MOVE){
        auto movstm = static_cast<Move*>(stm);
        if(movstm->src->kind == ExpType::CALL){
            return static_cast<mir::Call*>(movstm->src)->args;
        }
    }
    if(stm->kind == StmType::EXP){
        auto expstm = static_cast<ExpStm*>(stm);
        if(expstm->exp->kind == ExpType::CALL){
            return static_cast<mir::Call*>(expstm->exp)->args;
        }
    }
    return NULL;
}

mir::Exp* get_call_return(Stm* stm){
    if(stm->kind == StmType::MOVE){
        auto movstm = static_cast<Move*>(stm);
        if(movstm->src->kind == ExpType::CALL){
            return movstm->dst->exp_deepcopy();
        }
    }
    return NULL;
}

bool FunctionInline::valid_inline(std::string fun_name){
    return false;

    if(func_infos.count(fun_name) == 0) return false;
    auto fun_info = func_infos[fun_name];
    // fun_info->show();
    // if(fun_info->has_malloc) return false;
    if(fun_info->recursive) return false;
    if(fun_info->called_num == 1) return true; 
    if(fun_info->length > length_threshold) return false;
    return true;
}

void FunctionInline::fun_inline(){
    for(auto this_fun_name: funcs){
        // std::cerr << "fun_in_process" <<this_fun_name << std::endl;
        auto func_info = func_infos[this_fun_name];
        auto stmlist = func_info->ir;
        for(auto iter = stmlist->begin(); iter!=stmlist->end();){
            auto stm = *iter;
            if(is_call(stm)){
                // stm->print();
                auto called_fun_name = get_call_name(stm);
                // std::cerr<<called_fun_name<<std::endl;
                if(called_fun_name == this_fun_name) func_info->recursive = true;
                // cerr<<called_fun_name<<endl;
                // cerr<<valid_inline(called_fun_name)<<endl;
                if(!valid_inline(called_fun_name)) {
                    // std::cerr << called_fun_name << std::endl;
                    iter++;
                    continue;
                }
                // inline
                func_infos[called_fun_name]->called_num--;
                std::unordered_map<utils::Label*, utils::Label*> label_map;
                auto exit_label = new utils::Label();
                auto return_exp = get_call_return(stm);
                auto call_params = get_call_params(stm);
                if(call_params) {
                    auto param_iter = call_params->begin();
                    for(auto arg: *func_infos[called_fun_name]->fun->args){
                        auto new_stm = new mir::Move(new mir::Temp(arg), (*param_iter)->exp_deepcopy());
                        iter = stmlist->insert(iter, new_stm);
                        iter++;
                        param_iter++;
                    }
                }
                for(auto called_ir_stm: *func_infos[called_fun_name]->ir){
                    if(called_ir_stm->kind == StmType::RETURN){
                        if(return_exp){
                            auto new_stm = new mir::Move(return_exp, static_cast<mir::Return*>(called_ir_stm)->exp->exp_deepcopy());
                            iter = stmlist->insert(iter, new_stm);
                            iter++;
                        }
                        iter = stmlist->insert(iter, new Jump(exit_label));
                    }else{
                        iter = stmlist->insert(iter, called_ir_stm->stm_deepcopy_with_label(label_map));
                    }
                    iter++;
                }
                iter = stmlist->insert(iter, new Label(exit_label));
                iter++;
                *iter = new ExpStm(new Const(0));    
            }
            iter++;
        }
        func_info->ir = stmlist;
    }

    for(auto this_fun_name: funcs){
        if(func_infos[this_fun_name]->called_num == 0){
            func_infos.erase(this_fun_name);
        }
    }
}

void FunctionInline::fun_analysis(){
    for(auto fun_map: func_infos){
        
        auto fun_info = fun_map.second;
        auto fun_ir = fun_info->ir;
        if(fun_map.first == "main") fun_info->called_num = 1;
        for(auto stm: *fun_ir){
            if(is_call(stm)){ 
                auto called_fun = get_call_name(stm);
                if(called_fun == "malloc") fun_info->has_malloc = true;
                if(func_infos.count(called_fun) > 0){
                    fun_info->call_num++;
                    func_infos[called_fun]->called_num++;
                    // cerr<<called_fun<<"\t"<<fun_map.first<<endl;
                    fun_info->recursive = (called_fun == fun_map.first);
                }
            }else if(stm->kind == StmType::MOVE){
                auto mov = static_cast<Move*>(stm);
                if(mov->dst->kind == ExpType::MEM){
                    fun_info->side_effect = true;
                }
                if(mov->src->kind == ExpType::MEM){
                    fun_info->side_effect = true;
                }

            }
        }
    }
};

};