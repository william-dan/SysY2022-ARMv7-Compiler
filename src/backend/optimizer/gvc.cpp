#include "gvc.hpp"
#include <unordered_set>

namespace ssa{
using namespace std;

bool GlobalVarConst::is_init_zero(utils::Temp* ga, mir::Exp* e){
    assert(mir::GlobalInfo::global_table.count(ga));
    return mir::GlobalInfo::global_table[ga]->init_zero
        && e->kind == mir::ExpType::CONST 
        && static_cast<mir::Const*>(e)->n.i == 0;
}



GlobalVarConst::GlobalVarConst(std::unordered_map<mir::Fun*, mir::StmList*>* fun_ir):
fun_ir(fun_ir){
    
    unordered_set<utils::Temp*> def_glob;

    utils::Temp* curr_glob_array = nullptr;
    unordered_set<utils::Temp*> array_tracer;
    for(auto& ginfo: mir::GlobalInfo::global_table){
        if(ginfo.second->type == mir::GlobalInfoType::ARRAY){
            auto clist = ginfo.second->u.clist;
            for(auto it = clist->begin(); it != clist->end();){
                auto c = *it;
                if(c.second->n.i == 0){
                    it = clist->erase(it);
                }else{
                    it++;
                }
            }
            assert(clist->size() >= 1);
            ginfo.second->init_zero = (clist->size() == 1);
        }
    }
    // cerr<<"ini ok"<<endl;
    for(auto& fun_pr: *fun_ir){
        for(auto& s: *fun_pr.second){
            auto defs = s->get_def();
            for(auto& def: defs){
                if(mir::GlobalInfo::global_table.count(def)){
                    def_glob.emplace(def);
                }
            }
            //可能call里传全局数组参数，然后进函数赋值
            //这里只要看到call里有全局数组，就判断这个数组被定义过
            // if(s->kind == mir::StmType::MOVE){
            //     auto mov = static_cast<mir::Move*>(s);
            //     if(mov->dst->kind == mir::ExpType::MEM){
            //         auto mem = static_cast<mir::Mem*>(mov->dst);
            //         assert(mem->exp->kind == mir::ExpType::TEMP);
            //         auto mem_temp = static_cast<mir::Temp*>(mem->exp)->temp;
            //         if(curr_glob_array && array_tracer.count(mem_temp) && !is_init_zero(curr_glob_array, mov->src)){
            //             def_glob.emplace(curr_glob_array);
            //         }else if(mir::GlobalInfo::global_table.count(mem_temp) && !is_init_zero(mem_temp, mov->src)){
            //             def_glob.emplace(mem_temp);
            //         }
            //     }else if(mov->src->kind == mir::ExpType::BINOP
            //     || mov->src->kind == mir::ExpType::TEMP){
            //         assert(mov->dst->kind == mir::ExpType::TEMP);
            //         auto uses = s->get_use();
            //         for(auto& use: uses){
            //             if(mir::GlobalInfo::global_table.count(use)){
            //                 curr_glob_array = use;
            //                 array_tracer.clear();
            //                 array_tracer.emplace(use);
            //                 array_tracer.emplace(static_cast<mir::Temp*>(mov->dst)->temp);
            //                 break;
            //             }else if(array_tracer.count(use)){
            //                 array_tracer.emplace(static_cast<mir::Temp*>(mov->dst)->temp);
            //                 break;
            //             }
            //         }
            //     }else if(mov->src->kind == mir::ExpType::CALL){
            //         auto call = static_cast<mir::Call*>(mov->src);
            //         for(auto& arg: *call->args){
            //             if(arg->kind != mir::ExpType::TEMP) continue;
            //             auto arg_temp = static_cast<mir::Temp*>(arg)->temp;
            //             if(curr_glob_array && array_tracer.count(arg_temp)){
            //                 def_glob.emplace(curr_glob_array);
            //             }else if(mir::GlobalInfo::global_table.count(arg_temp)){
            //                 def_glob.emplace(arg_temp);
            //             }
            //         }
            //     }
            // }else if(s->kind == mir::StmType::EXP){
            //     auto expstm = static_cast<mir::ExpStm*>(s);
            //     if(expstm->exp->kind == mir::ExpType::CALL){
            //         auto call = static_cast<mir::Call*>(expstm->exp);
            //         for(auto& arg: *call->args){
            //             if(arg->kind != mir::ExpType::TEMP) continue;
            //             auto arg_temp = static_cast<mir::Temp*>(arg)->temp;
            //             if(curr_glob_array && array_tracer.count(arg_temp)){
            //                 def_glob.emplace(curr_glob_array);
            //             }else if(mir::GlobalInfo::global_table.count(arg_temp)){
            //                 def_glob.emplace(arg_temp);
            //             }
            //         }
            //     }
            // }
        }
    }
    // cerr<<"analysis ok" <<endl;
    for(auto& glob_pr: mir::GlobalInfo::global_table){
        if(!def_glob.count(glob_pr.first)){
            glob_pr.second->is_const = true;
        }
    }
    // cerr<<"isconst ok" <<endl;
    for(auto& fun_pr: *fun_ir){
        for(auto& s: *fun_pr.second){
            auto defs = s->get_def_ref();
            auto uses = s->get_use_ref();
            for(auto& mdef: defs){
                auto def = static_cast<mir::Temp*>(*mdef)->temp;
                if(mir::GlobalInfo::global_table.count(def)){
                    auto info = mir::GlobalInfo::global_table[def];
                    switch (info->type)
                    {
                    case mir::GlobalInfoType::ARRAY:
                    {
                        assert(0);
                        *mdef = new mir::Name(info->label);
                        
                    }
                    break;
                    case mir::GlobalInfoType::CONST:
                    {
                        *mdef = new mir::Mem(new mir::Name(info->label), def->type);
                    }
                    break;
                    default:
                        assert(0);
                    break;
                    }
                }
            }
            for(auto& muse: uses){
                auto use = static_cast<mir::Temp*>(*muse)->temp;
                if(mir::GlobalInfo::global_table.count(use)){
                    auto info = mir::GlobalInfo::global_table[use];
                    if(info->is_const && info->type == mir::GlobalInfoType::CONST){
                        *muse = info->u.c->exp_deepcopy();
                    }else{
                        switch (info->type)
                        {
                        case mir::GlobalInfoType::ARRAY:
                        {
                            *muse = new mir::Name(info->label);
                            
                        }
                        break;
                        case mir::GlobalInfoType::CONST:
                        {
                            *muse = new mir::Mem(new mir::Name(info->label), use->type);
                        }
                        break;
                        default:
                            assert(0);
                        break;
                        }
                    }
                }
            }

        }
    }

    
}



}