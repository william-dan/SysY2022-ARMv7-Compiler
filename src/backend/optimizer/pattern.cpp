#include "peephole.hpp"
namespace assem{
bool Pattern::is_add0(Instr* i){
    if(i->type == AssemType::Oper){
        auto operi = static_cast<OperInstr*>(i);
        auto assem = operi->assem;
        if(assem == "add `d0 `s0 #0" ||assem == "sub `d0 `s0 #0"){
            auto src = operi->src;
            auto dst = operi->dst;
            if(src->front() == dst->front()) return true;
        }
    }
    return false;
}
bool Pattern::is_mul1(Instr* i){
    if(i->type == AssemType::Oper){
        auto operi = static_cast<OperInstr*>(i);
        auto assem = operi->assem;
        if(assem == "mul `d0 `s0 #1" ||assem == "sdiv `d0 `s0 #1"){
            auto src = operi->src;
            auto dst = operi->dst;
            if(src->front() == dst->front()) return true;
        }
    }
    return false;
}

bool Pattern::is_movself(Instr* i){
    if(i->type == AssemType::Oper){
        auto operi = static_cast<OperInstr*>(i);
        auto assem = operi->assem;
        if(assem == "mov `d0 `s0"){
            auto src = operi->src;
            auto dst = operi->dst;
            if(src->front() == dst->front()) return true;
        }
    }
    return false;
}

bool Pattern::has_side_effect(Instr* i){
    return false;
}
//         case mir::StmType::MOVE: {
//             auto movstm = static_cast<mir::Move*>(stm);
//             auto exp = movstm->src;
//             switch (exp->kind){
//             case mir::ExpType::CALL:{
//                 return true;
//                 break;
//             }
//             default:
//                 break;
//             }
//             if (movstm->dst->kind == mir::ExpType::MEM) {
//                 return true;
//             }
//             // if (movstm->dst->kind == mir::ExpType::TEMP) {
//             //     return false;
//             // }
//         }
//         case mir::StmType::EXP: {
//             auto expstm = static_cast<mir::ExpStm*>(stm);
//             if (expstm->exp->kind == mir::ExpType::CALL) {
//                 return true;
//             }
//             break;
//         }
//         case mir::StmType::CJUMP: {
//             return true;
//             break;
//         }
//         case mir::StmType::RETURN: {
//             return true;
//             break;
//         }
//     }
//     return false;
// }

}