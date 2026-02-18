#include "mc.hpp"

namespace ssa{

using namespace mir;
MeanlessCalculation::MeanlessCalculation(BlockGraph* bg){
    for(auto& n: *bg->mynodes){
        for(auto& s: *bg->get_info(n)){
            handle_single_stm(s);
        }
    }
}

void MeanlessCalculation::handle_single_stm(mir::Stm* s){
    if(s->kind == StmType::MOVE){
        auto mov = static_cast<Move*>(s);
        if(mov->src->kind == ExpType::BINOP){
            auto bop = static_cast<Binop*>(mov->src);
            if(bop->left->kind == ExpType::TEMP && bop->right->kind == ExpType::CONST){
                auto rconst = static_cast<Const*>(bop->right);
                switch (bop->op)
                {
                case BinOp::PLUS:
                case BinOp::MINUS:
                {
                    if(rconst->n.i == 0){
                        mov->src = bop->left;
                    }
                }
                break;
                case BinOp::MUL:
                {
                    if(rconst->n.i == 0){
                        mov->src = bop->right;
                    }else if(rconst->n.i == 1){
                        mov->src = bop->left;
                    }else if(rconst->n.i == -1){
                        bop->right = bop->left;
                        bop->left = new Const(0);
                        bop->op = BinOp::MINUS;
                    }
                }
                case BinOp::DIV:
                {
                    if(rconst->n.i == 1){
                        mov->src = bop->left;
                    }else if(rconst->n.i == -1){
                        bop->right = bop->left;
                        bop->left = new Const(0);
                        bop->op = BinOp::MINUS;
                    }
                }
                break;
                
                default:
                    break;
                }
            }else if(bop->left->kind == ExpType::CONST && bop->right->kind == ExpType::TEMP){
                auto lconst = static_cast<Const*>(bop->left);
                switch (bop->op)
                {
                case BinOp::PLUS:
                {
                    if(lconst->n.i == 0){
                        mov->src = bop->right;
                    }
                }
                break;
                case BinOp::MUL:
                {
                    if(lconst->n.i == 0){
                        mov->src = bop->left;
                    }else if(lconst->n.i == 1){
                        mov->src = bop->right;
                    }else if(lconst->n.i == -1){
                        bop->left = new Const(0);
                        bop->op = BinOp::MINUS;
                    }
                }
                case BinOp::DIV:
                {
                    if(lconst->n.i == 0){
                        mov->src = new Const(0);
                    }
                }
                break;
                
                default:
                    break;
                }
            }
        }
    }
}



}