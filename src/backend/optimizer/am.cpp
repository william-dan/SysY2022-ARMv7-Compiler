#include "am.hpp"

namespace mir{

ArrayMotion::ArrayMotion(StmList* sl){
    StmList stage_pool;
    for(auto& s: *sl){
        if(s->kind == StmType::MOVE){
            auto mov = static_cast<Move*>(s);
            if(mov->src->kind == ExpType::CALL){
                auto call = static_cast<Call*>(mov->src);
                if(*call->name == "@malloc"){
                    *call->name = "malloc";
                    stage_pool.emplace_back(s);
                    assert(call->args->size() == 1);
                    s = new ExpStm(new Const(0));
                }else if(*call->name == "@calloc"){
                    *call->name = "malloc";
                    stage_pool.emplace_back(s);
                    assert(call->args->size() == 1);
                    s = new ExpStm(new Call(new string("memset"), 
                        new ExpList{mov->dst->exp_deepcopy(), new Const(0), call->args->front()->exp_deepcopy()},
                        utils::TempType::VOID));
                }
            }
        }
    }
    for(auto& s: stage_pool){
        sl->emplace_front(s);
    }
}


}