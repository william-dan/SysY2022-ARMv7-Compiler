#include "mir.hpp"
#include "../../backend/alloc/color.hpp"
#include <string>
#include <cassert>
#include <vector>
#include <cmath>
namespace mir{
using namespace std;
static utils::TempList call_def_reg;
static utils::TempList call_use_reg;

static void binop_handle(utils::Temp* dst, Binop* bop, assem::InstrList* instrlist);
static void call_handle(utils::Temp* dst, Call* call, assem::InstrList* instrlist);
static void mem_handle(utils::Temp* dst, Mem* mem, assem::InstrList* instrlist);
static void name_handle(utils::Temp* dst, Name* name, assem::InstrList* instrlist);

std::string relop2str(RelOp op){
    switch (op)
    {
    case RelOp::EQ:{
            return std::string("beq");
            break;
        }
        case RelOp::NE:{
            return std::string("bne");
            break;    
        }
        case RelOp::LT:
        case RelOp::ULT:{
            return std::string("blt");
            break;
        }
        case RelOp::GT:
        case RelOp::UGT:
        {
            return std::string("bgt");
        } 
        case RelOp::LE:
        case RelOp::ULE: {
            return std::string("ble");
        } 
        case RelOp::GE:
        case RelOp::UGE:{
            return std::string("bge");
        }
    }
}

std::string binop2str(BinOp op){
    switch (op)
    {
        case BinOp::PLUS:{
            return std::string("add");
            break;
        }
        case BinOp::MINUS:{
            return std::string("sub");
            break;    
        }
        case BinOp::MUL:{
            return std::string("mul");
            break;
        }
        case BinOp::DIV:
        {
            return std::string("sdiv");
        } 
        case BinOp::MOD:
        {
            return std::string("mod");
        }
        case BinOp::AND: {
            return std::string("and");
        } 
        case BinOp::OR:{
            return std::string("orr");
        }
        default:{
            assert(0);
            std::cerr<<"not supported"<<std::endl;
            return std::string("add");
        }     
    }
    return std::string("void");
}

std::string f_binop2str(BinOp op){
    switch (op)
    {
        case BinOp::PLUS:{
            return std::string("vadd.f32");
            break;
        }
        case BinOp::MINUS:{
            return std::string("vsub.f32");
            break;    
        }
        case BinOp::MUL:{
            return std::string("vmul.f32");
            break;
        }
        case BinOp::DIV:
        {
            return std::string("vdiv.f32");
        } 
        default:{
            assert(0);
            std::cerr<<"not supported"<<std::endl;
            return std::string("add");
        }     
    }
    return std::string("void");
}

void Fun::ir2assem(assem::Instrlist* instrlist){
    assert(0);
}

void Seq::ir2assem(assem::Instrlist* instrlist){
    // left->ir2assem(instrlist);
    // right->ir2assem(instrlist);
    assert(0);
}

void Label::ir2assem(assem::Instrlist* instrlist){
    instrlist->emplace_back(new assem::LabelInstr(label));
}


void Jump::ir2assem(assem::Instrlist* instrlist){
    instrlist->emplace_back(
        new assem::OperInstr(
            "b `j0", new utils::TempList(), new utils::TempList(),
            new utils::LabelList{label}
        )
    );
}

void Cjump::ir2assem(assem::Instrlist* instrlist){
    
    auto left_tmp = left->ir2assem(instrlist);
    auto right_tmp = right->ir2assem(instrlist);
    assert(left_tmp->type == right_tmp->type);
    switch (left_tmp->type)
    {
    case utils::TempType::INT:
    {
        instrlist->emplace_back(
            new assem::OperInstr(
                "cmp `s0, `s1",
                new utils::TempList{left_tmp, right_tmp},
                new utils::TempList(), 
                new utils::LabelList()
            )
        );
    }
    break;
    case utils::TempType::FLOAT:
    {
        instrlist->emplace_back(
            new assem::OperInstr(
                "vcmp.f32 `s0, `s1", 
                new utils::TempList{left_tmp, right_tmp},
                new utils::TempList(), new utils::LabelList()
            )
        );
        instrlist->emplace_back(
            new assem::OperInstr(
                "vmrs APSR_nzcv, fpscr",
                new utils::TempList(), new utils::TempList(), new utils::LabelList()
            )
        );
    }
    break;
    default:
        assert(0);
    break;
    }
    auto extra_label = new utils::Label();
    instrlist->emplace_back(
        new assem::OperInstr(
            relop2str(op) + " `j0",
            new utils::TempList(),
            new utils::TempList(),
            new utils::LabelList{true_label, extra_label}
        )
    );
    instrlist->emplace_back(
        new assem::LabelInstr(extra_label)
    );
    instrlist->emplace_back(
        new assem::OperInstr(
            "b `j0",
            new utils::TempList(),
            new utils::TempList(),
            new utils::LabelList{false_label}
        )
    );
}

void Move::ir2assem(assem::Instrlist* instrlist){
    if(dst->kind == ExpType::MEM){
        Mem* mem = static_cast<Mem*>(dst);
        utils::Temp* src_tmp = src->ir2assem(instrlist);

        if(mem->exp->kind == ExpType::BINOP){
            auto bop = static_cast<Binop*>(mem->exp);
            assert(bop->op == BinOp::PLUS);
            if(bop->left->kind == ExpType::CONST){
                assert(bop->right->kind == ExpType::TEMP);
                assem::emit_str_back(instrlist, new utils::TempList(), 
                    new utils::TempList{src_tmp, static_cast<Temp*>(bop->right)->temp}, 
                    static_cast<Const*>(bop->left)->n.i,
                    dst->get_type());
                return;
            }else if(bop->right->kind == ExpType::CONST){
                assert(bop->left->kind == ExpType::TEMP);
                assem::emit_str_back(instrlist, new utils::TempList(), 
                    new utils::TempList{src_tmp, static_cast<Temp*>(bop->left)->temp}, 
                    static_cast<Const*>(bop->right)->n.i,
                    dst->get_type());
                return;
            }
        }
        utils::Temp* mem_tmp = mem->exp->ir2assem(instrlist);
        assem::emit_str_back(instrlist, new utils::TempList(), new utils::TempList{src_tmp, mem_tmp}, 0, dst->get_type());
    }else{
        utils::Temp* dst_tmp = dst->ir2assem(instrlist);
        switch (src->kind)
        {
        case ExpType::BINOP:
        {
            binop_handle(dst_tmp, static_cast<Binop*>(src), instrlist);
        }
        break;
        case ExpType::CALL:
        {   
            call_handle(dst_tmp, static_cast<Call*>(src), instrlist);
        }
        break;
        case ExpType::MEM:
        {
            mem_handle(dst_tmp, static_cast<Mem*>(src), instrlist);
        }
        break;
        case ExpType::NAME:
        {
            name_handle(dst_tmp, static_cast<Name*>(src), instrlist);
        }
        break;
        default:
        {
            utils::Temp* src_tmp = src->ir2assem(instrlist);
            assert(dst_tmp->type == src_tmp->type);
            assem::emit_mov_back(instrlist, new utils::TempList{dst_tmp}, new utils::TempList{src_tmp}, 0, dst_tmp->type);
        }
        break;
        }
        
    }
}

void ExpStm::ir2assem(assem::Instrlist* instrlist){
    if(exp->kind == ExpType::CONST){
        return;
    }
    exp->ir2assem(instrlist);
}

void Return::ir2assem(assem::Instrlist* instrlist){
    // retrieve callee saved register
    for(int i = 4; i <= 10; i++){
        instrlist->emplace_back(
            new assem::MoveInstr(
                "mov `d0, `s0",
                new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string("callee_save_" + to_string(i)))},
                new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string(to_string(i)))}
            )
        );
    }
    if(exp->kind != ExpType::NAME){
        auto ret_tmp = exp->ir2assem(instrlist);
        assem::emit_mov_back(instrlist, new utils::TempList{utils::Temp::new_Temp(ret_tmp->type, new string("0"))},
            new utils::TempList{ret_tmp}, 0, ret_tmp->type);
    }
    // retrieve last fp
    instrlist->emplace_back(
        new assem::OperInstr(
            "mov sp, fp",
            new utils::TempList(),
            new utils::TempList(), new utils::LabelList()
        )
    );
    // pop sp
    instrlist->emplace_back(
        new assem::OperInstr(
            "pop {fp,`d0}",
            new utils::TempList(),
            new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string("lr"))},
            new utils::LabelList()
        )
    );
    // bx addr
    instrlist->emplace_back(
        new assem::OperInstr(
            "bx `s0", new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string("lr"))},
            new utils::TempList(), new utils::LabelList()
        )
    );
}


utils::Temp* Binop::ir2assem(assem::Instrlist* instrlist){
    if(left->kind == ExpType::CONST && right->kind != ExpType::CONST && (op == BinOp::PLUS || op == BinOp::MUL)){
        auto tmp = left;
        left = right;
        right = tmp;
    }
    utils::Temp* left_tmp = left->ir2assem(instrlist);
    utils::Temp* ret = new utils::Temp(left_tmp->type);

    if(right->kind == ExpType::CONST){
        auto rconst = static_cast<Const*>(right);
        if(left_tmp->type == utils::TempType::INT){
            // if(left_tmp->type != rconst->type) print();
            assert(left_tmp->type == rconst->type);
            // special case
            if(op == BinOp::MUL && rconst->n.i == 0){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "mov `d0, #0", new utils::TempList(), new utils::TempList{ret},
                        new utils::LabelList()
                    )
                );
                return ret;
            }else if(op == BinOp::MUL && assem::is2power(rconst->n.i)){
                int base = log2(abs(rconst->n.i));
                // cout << "log2(" << abs(rconst->n.i) << ") is " << base;
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "lsl `d0, `s0, #" + std::to_string(base),
                        new utils::TempList{left_tmp},
                        new utils::TempList{ret}, new utils::LabelList()
                    )
                );

                if(rconst->n.i < 0){
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "rsb `d0, `s0, #0", 
                            new utils::TempList{ret},
                            new utils::TempList{ret}, new utils::LabelList()
                        )
                    );
                }
                return ret;  
            }else if(op == BinOp::DIV && assem::is2power(rconst->n.i)){
                assert(rconst->n.i != 0);
                int base = log2(abs(rconst->n.i));
                // cout << "log2(" << abs(rconst->n.i) << ") is " << base;
                auto temp_after_add = new utils::Temp(utils::TempType::INT);
                bool isop2 = assem::isOperand2(abs(rconst->n.i) - 1);
                if(!isop2){
                    assem::emit_mov_back(instrlist, new utils::TempList{temp_after_add}, new utils::TempList(), abs(rconst->n.i) - 1, utils::TempType::INT);
                }
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "cmp `s0, #0",
                        new utils::TempList{left_tmp},
                        new utils::TempList(), new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "ite lt",
                        new utils::TempList(),
                        new utils::TempList(), new utils::LabelList()
                    )
                );
                if(isop2){
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "addlt `d0, `s0, #" + std::to_string(abs(rconst->n.i) - 1),
                            new utils::TempList{left_tmp},
                            new utils::TempList{temp_after_add}, new utils::LabelList()
                        )
                    );
                }else{
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "addlt `d0, `s0, `s1",
                            new utils::TempList{left_tmp, temp_after_add},
                            new utils::TempList{temp_after_add}, new utils::LabelList()
                        )
                    );
                }
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "movge `d0, `s0",
                        new utils::TempList{left_tmp},
                        new utils::TempList{temp_after_add}, new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "asr `d0, `s0, #" + std::to_string(base),
                        new utils::TempList{temp_after_add},
                        new utils::TempList{ret}, new utils::LabelList()
                    )
                );

                if(rconst->n.i < 0){
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "rsb `d0, `s0, #0", 
                            new utils::TempList{ret},
                            new utils::TempList{ret}, new utils::LabelList()
                        )
                    );
                }
                return ret;  
            }else if((assem::isOperand2(rconst->n.i)) && (op == BinOp::PLUS || op == BinOp::MINUS)){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        binop2str(op) + " `d0, `s0, #" + std::to_string(rconst->n.i),
                        new utils::TempList{left_tmp},
                        new utils::TempList{ret}, new utils::LabelList()
                    )
                );

                return ret;
            }
            
           
        }


    }

    if(op == BinOp::MOD && right->kind == mir::ExpType::CONST){
        auto div_tmp = new utils::Temp(left_tmp->type);
        auto mul_tmp = new utils::Temp(left_tmp->type);
        auto div = new Move(new Temp(div_tmp), new Binop(BinOp::DIV, new Temp(left_tmp), right->exp_deepcopy()));
        auto mul = new Move(new Temp(mul_tmp), new Binop(BinOp::MUL, new Temp(div_tmp), right->exp_deepcopy()));
        auto minus = new Move(new Temp(ret), new Binop(BinOp::MINUS, new Temp(left_tmp), new Temp(mul_tmp)));
        div->ir2assem(instrlist);
        mul->ir2assem(instrlist);
        minus->ir2assem(instrlist);
        delete div;
        delete mul;
        delete minus;
        return ret;
    }

    utils::Temp* right_tmp = right->ir2assem(instrlist);
    assert(left_tmp->type == right_tmp->type);

    if(op == BinOp::MOD){
        auto div_tmp = new utils::Temp(left_tmp->type);
        auto mul_tmp = new utils::Temp(left_tmp->type);
        auto div = new Move(new Temp(div_tmp), new Binop(BinOp::DIV, new Temp(left_tmp), new Temp(right_tmp)));
        auto mul = new Move(new Temp(mul_tmp), new Binop(BinOp::MUL, new Temp(div_tmp), new Temp(right_tmp)));
        auto minus = new Move(new Temp(ret), new Binop(BinOp::MINUS, new Temp(left_tmp), new Temp(mul_tmp)));
        div->ir2assem(instrlist);
        mul->ir2assem(instrlist);
        minus->ir2assem(instrlist);
        delete div;
        delete mul;
        delete minus;
    }else{
        if(left_tmp->type == utils::TempType::INT){
            instrlist->emplace_back(
                new assem::OperInstr(
                    binop2str(op) + " `d0, `s0, `s1",
                    new utils::TempList{left_tmp, right_tmp},
                    new utils::TempList{ret},
                    new utils::LabelList()
                )
            );
        }else if(left_tmp->type == utils::TempType::FLOAT){
            instrlist->emplace_back(
                new assem::OperInstr(
                    f_binop2str(op) + " `d0, `s0, `s1",
                    new utils::TempList{left_tmp, right_tmp},
                    new utils::TempList{ret},
                    new utils::LabelList()
                )
            );
        }
    }

    return ret;
}
// Exp
static void binop_handle(utils::Temp* dst, Binop* bop, assem::InstrList* instrlist){

    if(bop->left->kind == ExpType::CONST && bop->right->kind != ExpType::CONST && (bop->op == BinOp::PLUS || bop->op == BinOp::MUL)){
        auto tmp = bop->left;
        bop->left = bop->right;
        bop->right = tmp;
    }
    utils::Temp* left_tmp = bop->left->ir2assem(instrlist);
    utils::Temp* ret = dst;
    assert(ret->type == left_tmp->type);

    if(bop->right->kind == ExpType::CONST){
        auto rconst = static_cast<Const*>(bop->right);
        if(left_tmp->type == utils::TempType::INT){
            // if(left_tmp->type != rconst->type) print();
            assert(left_tmp->type == rconst->type);
            // special case
            if(bop->op == BinOp::MUL && rconst->n.i == 0){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "mov `d0, #0", new utils::TempList(), new utils::TempList{ret},
                        new utils::LabelList()
                    )
                );
                return;
            }else if(bop->op == BinOp::MUL && assem::is2power(rconst->n.i)){
                int base = log2(abs(rconst->n.i));
                // cout << "log2(" << abs(rconst->n.i) << ") is " << base;
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "lsl `d0, `s0, #" + std::to_string(base),
                        new utils::TempList{left_tmp},
                        new utils::TempList{ret}, new utils::LabelList()
                    )
                );

                if(rconst->n.i < 0){
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "rsb `d0, `s0, #0", 
                            new utils::TempList{ret},
                            new utils::TempList{ret}, new utils::LabelList()
                        )
                    );
                }
                return; 
            }else if(bop->op == BinOp::DIV && assem::is2power(rconst->n.i)){
                assert(rconst->n.i != 0);
                int base = log2(abs(rconst->n.i));
                // cout << "log2(" << abs(rconst->n.i) << ") is " << base;
                auto temp_after_add = new utils::Temp(utils::TempType::INT);
                bool isop2 = assem::isOperand2(abs(rconst->n.i) - 1);
                if(!isop2){
                    assem::emit_mov_back(instrlist, new utils::TempList{temp_after_add}, new utils::TempList(), abs(rconst->n.i) - 1, utils::TempType::INT);
                }
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "cmp `s0, #0",
                        new utils::TempList{left_tmp},
                        new utils::TempList(), new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "ite lt",
                        new utils::TempList(),
                        new utils::TempList(), new utils::LabelList()
                    )
                );
                if(isop2){
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "addlt `d0, `s0, #" + std::to_string(abs(rconst->n.i) - 1),
                            new utils::TempList{left_tmp},
                            new utils::TempList{temp_after_add}, new utils::LabelList()
                        )
                    );
                }else{
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "addlt `d0, `s0, `s1",
                            new utils::TempList{left_tmp, temp_after_add},
                            new utils::TempList{temp_after_add}, new utils::LabelList()
                        )
                    );
                }
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "movge `d0, `s0",
                        new utils::TempList{left_tmp},
                        new utils::TempList{temp_after_add}, new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "asr `d0, `s0, #" + std::to_string(base),
                        new utils::TempList{temp_after_add},
                        new utils::TempList{ret}, new utils::LabelList()
                    )
                );

                if(rconst->n.i < 0){
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "rsb `d0, `s0, #0", 
                            new utils::TempList{ret},
                            new utils::TempList{ret}, new utils::LabelList()
                        )
                    );
                }
                return; 
            }else if((assem::isOperand2(rconst->n.i)) && (bop->op == BinOp::PLUS || bop->op == BinOp::MINUS)){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        binop2str(bop->op) + " `d0, `s0, #" + std::to_string(rconst->n.i),
                        new utils::TempList{left_tmp},
                        new utils::TempList{ret}, new utils::LabelList()
                    )
                );

                return;
            }
            
           
        }


    }
    if(bop->op == BinOp::MOD && bop->right->kind == mir::ExpType::CONST){
        auto div_tmp = new utils::Temp(left_tmp->type);
        auto mul_tmp = new utils::Temp(left_tmp->type);
        auto div = new Move(new Temp(div_tmp), new Binop(BinOp::DIV, new Temp(left_tmp), bop->right->exp_deepcopy()));
        auto mul = new Move(new Temp(mul_tmp), new Binop(BinOp::MUL, new Temp(div_tmp), bop->right->exp_deepcopy()));
        auto minus = new Move(new Temp(ret), new Binop(BinOp::MINUS, new Temp(left_tmp), new Temp(mul_tmp)));
        div->ir2assem(instrlist);
        mul->ir2assem(instrlist);
        minus->ir2assem(instrlist);
        delete div;
        delete mul;
        delete minus;
        return;
    }
    utils::Temp* right_tmp = bop->right->ir2assem(instrlist);
    assert(left_tmp->type == right_tmp->type);

    if(bop->op == BinOp::MOD){
        auto div_tmp = new utils::Temp(left_tmp->type);
        auto mul_tmp = new utils::Temp(left_tmp->type);
        auto div = new Move(new Temp(div_tmp), new Binop(BinOp::DIV, new Temp(left_tmp), new Temp(right_tmp)));
        auto mul = new Move(new Temp(mul_tmp), new Binop(BinOp::MUL, new Temp(div_tmp), new Temp(right_tmp)));
        auto minus = new Move(new Temp(ret), new Binop(BinOp::MINUS, new Temp(left_tmp), new Temp(mul_tmp)));
        div->ir2assem(instrlist);
        mul->ir2assem(instrlist);
        minus->ir2assem(instrlist);
        delete div;
        delete mul;
        delete minus;
    }else{
        if(left_tmp->type == utils::TempType::INT){
            instrlist->emplace_back(
                new assem::OperInstr(
                    binop2str(bop->op) + " `d0, `s0, `s1",
                    new utils::TempList{left_tmp, right_tmp},
                    new utils::TempList{ret},
                    new utils::LabelList()
                )
            );
        }else if(left_tmp->type == utils::TempType::FLOAT){
            instrlist->emplace_back(
                new assem::OperInstr(
                    f_binop2str(bop->op) + " `d0, `s0, `s1",
                    new utils::TempList{left_tmp, right_tmp},
                    new utils::TempList{ret},
                    new utils::LabelList()
                )
            );
        }
    }

    // return ret;
}

utils::Temp* Mem::ir2assem(assem::Instrlist* instrlist){
    utils::Temp* ret = new utils::Temp(type);
    assem::emit_ldr_back(instrlist, new utils::TempList{ret}, new utils::TempList{exp->ir2assem(instrlist)},
        0, type);
    return ret;
}

static void mem_handle(utils::Temp* dst, Mem* mem, assem::InstrList* instrlist){
    utils::Temp* ret = dst;
    assem::emit_ldr_back(instrlist, new utils::TempList{ret}, new utils::TempList{mem->exp->ir2assem(instrlist)},
        0, mem->type);
}

utils::Temp* Temp::ir2assem(assem::Instrlist* instrlist){
    return temp;
}
utils::Temp* Eseq::ir2assem(assem::Instrlist* instrlist){
    assert(0);
    return nullptr;
}
utils::Temp* Name::ir2assem(assem::Instrlist* instrlist){
    utils::Temp* tmp = new utils::Temp(utils::TempType::INT);
    instrlist->emplace_back(
        new assem::OperInstr(
            "movw `d0, #:lower16:" + *label->id,
            new utils::TempList(), new utils::TempList{tmp},
            new utils::LabelList()
        )
    );
    instrlist->emplace_back(
        new assem::OperInstr(
            "movt `d0, #:upper16:" + *label->id,
            new utils::TempList(), new utils::TempList{tmp},
            new utils::LabelList()
        )
    );
    return tmp;
}


static void name_handle(utils::Temp* dst, Name* name, assem::InstrList* instrlist){
    utils::Temp* tmp = dst;
    instrlist->emplace_back(
        new assem::OperInstr(
            "movw `d0, #:lower16:" + *name->label->id,
            new utils::TempList(), new utils::TempList{tmp},
            new utils::LabelList()
        )
    );
    instrlist->emplace_back(
        new assem::OperInstr(
            "movt `d0, #:upper16:" + *name->label->id,
            new utils::TempList(), new utils::TempList{tmp},
            new utils::LabelList()
        )
    );
}

utils::Temp* Format::ir2assem(assem::Instrlist* instrlist){
    utils::Temp* ret = new utils::Temp(utils::TempType::INT);
    GlobalInfo::global_table[ret] = new GlobalInfo(
        new utils::Label(utils::SpecialLabelType::LC),
        new string(*str)
    );
    return ret;
}
utils::Temp* Const::ir2assem(assem::Instrlist* instrlist){
    utils::Temp* ret = new utils::Temp(this->type);
    assem::emit_mov_back(instrlist, new utils::TempList{ret}, new utils::TempList(), this->n.i, type);
    return ret;
}
utils::Temp* Call::ir2assem(assem::Instrlist* instrlist){
   
    if(*name == "@i2f"){
        assert(args->size() == 1);
        auto arg_tmp = args->front()->ir2assem(instrlist);
        assert(arg_tmp->type == utils::TempType::INT);
        auto f_tmp = new utils::Temp(utils::TempType::FLOAT);
        auto ret = new utils::Temp(utils::TempType::FLOAT);
        instrlist->emplace_back(
            new assem::OperInstr(
                "vmov `d0, `s0", new utils::TempList{arg_tmp},
                new utils::TempList{f_tmp}, new utils::LabelList()
            )
        );
        instrlist->emplace_back(
            new assem::OperInstr(
                "vcvt.f32.s32 `d0, `s0", new utils::TempList{f_tmp},
                new utils::TempList{ret}, new utils::LabelList()
            )
        );
        return ret;
    }else if(*name == "@f2i"){
        assert(args->size() == 1);
        auto arg_tmp = args->front()->ir2assem(instrlist);
        assert(arg_tmp->type == utils::TempType::FLOAT);
        auto s_tmp = new utils::Temp(utils::TempType::FLOAT);
        auto ret = new utils::Temp(utils::TempType::INT);
        instrlist->emplace_back(
            new assem::OperInstr(
                "vcvt.s32.f32 `d0, `s0", new utils::TempList{arg_tmp},
                new utils::TempList{s_tmp}, new utils::LabelList()
            )
        );
        instrlist->emplace_back(
            new assem::OperInstr(
                "vmov `d0, `s0", new utils::TempList{s_tmp},
                new utils::TempList{ret}, new utils::LabelList()
            )
        );
        return ret;

    }else if(*name == "malloc"){
        assert(args->size() == 1);
        assert(args->front()->kind == ExpType::CONST);
        auto const_node = static_cast<Const*>(args->front());
        assert(const_node->type == utils::TempType::INT);
        auto arg_const = const_node->n.i;
        assert(arg_const % 4 == 0);
        if(arg_const % 8) arg_const += 4;
        // alloc::Color::real_spill_n += arg_const / 4;
        auto ret = new utils::Temp(utils::TempType::INT);
        auto sub_sp = new Move(new Temp(utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))), 
            new Binop(BinOp::MINUS, new Temp(utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))),
            new Const(arg_const)));
        auto assign_array_poing = new Move(new Temp(ret), new Temp(utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))));
        sub_sp->ir2assem(instrlist);
        assign_array_poing->ir2assem(instrlist);

        return ret;

    }else{
        int arg_n = 0;
        vector<utils::Temp*> spill_tmp;
    
        for(auto& arg: *args){
            if(arg_n < 4){
                auto arg_tmp = arg->ir2assem(instrlist);
                assem::emit_mov_back(instrlist, new utils::TempList{utils::Temp::new_Temp(arg_tmp->type, new string(to_string(arg_n)))},
                    new utils::TempList{arg_tmp}, 0, arg_tmp->type);
            }else{
                spill_tmp.emplace_back(arg->ir2assem(instrlist));
            }
            arg_n++;
        } 
        // 常数可能过大
        int i = 1;
        for(; i <= spill_tmp.size(); i++){
            auto off = -i * mir::DATA_SIZE;
            assem::emit_str_back(instrlist, new utils::TempList(), 
                new utils::TempList{spill_tmp[i-1], utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))},
                off, spill_tmp[i-1]->type);
        }

        if(!spill_tmp.empty()){
            auto off = spill_tmp.size()*mir::DATA_SIZE;
            if(assem::isOperand2(off) || assem::isImm12(off)){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "sub sp, sp, #" + std::to_string(off), 
                        new utils::TempList(), new utils::TempList(), new utils::LabelList()
                    )
                );
            }else{
                auto cp = assem::get_BigConst(off);
                auto ctmp = new utils::Temp(utils::TempType::INT);
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "movw `d0, #:lower16:" + std::to_string(off), 
                        new utils::TempList(), new utils::TempList{ctmp}, new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "movt `d0, #:upper16:" + std::to_string(off), 
                        new utils::TempList(), new utils::TempList{ctmp}, new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "sub sp, sp, `s0",
                        new utils::TempList{ctmp}, new utils::TempList(), new utils::LabelList()
                    )
                );

            }
        }
        string funname = *name;
        auto sp_tmp = new utils::Temp(utils::TempType::INT);
        if(*name == "putfloat"){
            instrlist->emplace_back(
                new assem::OperInstr(
                    "vmov `d0, `s0", new utils::TempList{utils::Temp::new_Temp(utils::TempType::FLOAT, new string("0"))}, 
                    new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string("0"))}, new utils::LabelList()
                )
            );
        }
        auto fix_call_use_reg = new utils::TempList(call_use_reg);
        for(int i = 0; i < min(4, (int)args->size()); i++){
            fix_call_use_reg->emplace_back(utils::Temp::new_Temp(utils::TempType::INT, new string(to_string(i))));
        }
        if(funname == "_sysy_starttime" || funname == "_sysy_stoptime"){
            fix_call_use_reg->emplace_back(utils::Temp::new_Temp(utils::TempType::INT, new string("0")));
            instrlist->emplace_back(
                new assem::OperInstr(
                    "mov `d0, #0", new utils::TempList(), 
                    new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string("0"))}, new utils::LabelList()
                )
            );
        }
        instrlist->emplace_back(
            new assem::OperInstr(
                "bl " + funname,
                fix_call_use_reg, 
                new utils::TempList(call_def_reg),
                new utils::LabelList()
            )
        );
        
        if(arg_n > 4){
            auto add_back_sp = new Move(
                new Temp(utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))),
                new Binop(BinOp::PLUS, new Temp(utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))),
                new Const((arg_n-4)*DATA_SIZE)));
            add_back_sp->ir2assem(instrlist);
        }
        if(type == utils::TempType::VOID) return utils::Temp::new_Temp(utils::TempType::INT, new string("0"));
        else return utils::Temp::new_Temp(this->type, new string("0"));
    }
    
}

static void call_handle(utils::Temp* dst, Call* call, assem::InstrList* instrlist){
    auto name = call->name;
    auto args = call->args;
    if(*name == "@i2f"){
        assert(args->size() == 1);
        auto arg_tmp = args->front()->ir2assem(instrlist);
        assert(arg_tmp->type == utils::TempType::INT);
        auto f_tmp = new utils::Temp(utils::TempType::FLOAT);
        auto ret = dst;
        instrlist->emplace_back(
            new assem::OperInstr(
                "vmov `d0, `s0", new utils::TempList{arg_tmp},
                new utils::TempList{f_tmp}, new utils::LabelList()
            )
        );
        instrlist->emplace_back(
            new assem::OperInstr(
                "vcvt.f32.s32 `d0, `s0", new utils::TempList{f_tmp},
                new utils::TempList{ret}, new utils::LabelList()
            )
        );
        return;
    }else if(*name == "@f2i"){
        assert(args->size() == 1);
        auto arg_tmp = args->front()->ir2assem(instrlist);
        assert(arg_tmp->type == utils::TempType::FLOAT);
        auto s_tmp = new utils::Temp(utils::TempType::FLOAT);
        auto ret = dst;
        instrlist->emplace_back(
            new assem::OperInstr(
                "vcvt.s32.f32 `d0, `s0", new utils::TempList{arg_tmp},
                new utils::TempList{s_tmp}, new utils::LabelList()
            )
        );
        instrlist->emplace_back(
            new assem::OperInstr(
                "vmov `d0, `s0", new utils::TempList{s_tmp},
                new utils::TempList{ret}, new utils::LabelList()
            )
        );
        return;
    }else if(*name == "malloc"){
        assert(args->size() == 1);
        assert(args->front()->kind == ExpType::CONST);
        auto const_node = static_cast<Const*>(args->front());
        assert(const_node->type == utils::TempType::INT);
        auto arg_const = const_node->n.i;
        assert(arg_const % 4 == 0);
        if(arg_const % 8) arg_const += 4;
        // alloc::Color::real_spill_n += arg_const / 4;
        auto ret = dst;
        auto sub_sp = new Move(new Temp(utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))), 
            new Binop(BinOp::MINUS, new Temp(utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))),
            new Const(arg_const)));
        auto assign_array_poing = new Move(new Temp(ret), new Temp(utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))));
        sub_sp->ir2assem(instrlist);
        assign_array_poing->ir2assem(instrlist);

        return;
    }else{
        int arg_n = 0;
        vector<utils::Temp*> spill_tmp;
    
        for(auto& arg: *args){
            if(arg_n < 4){
                auto arg_tmp = arg->ir2assem(instrlist);
                assem::emit_mov_back(instrlist, new utils::TempList{utils::Temp::new_Temp(arg_tmp->type, new string(to_string(arg_n)))},
                    new utils::TempList{arg_tmp}, 0, arg_tmp->type);
            }else{
                spill_tmp.emplace_back(arg->ir2assem(instrlist));
            }
            arg_n++;
        } 
        // 常数可能过大
        int i = 1;
        for(; i <= spill_tmp.size(); i++){
            auto off = -i * mir::DATA_SIZE;
            assem::emit_str_back(instrlist, new utils::TempList(), 
                new utils::TempList{spill_tmp[i-1], utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))},
                off, spill_tmp[i-1]->type);
        }

        if(!spill_tmp.empty()){
            auto off = spill_tmp.size()*mir::DATA_SIZE;
            if(assem::isOperand2(off) || assem::isImm12(off)){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "sub sp, sp, #" + std::to_string(off), 
                        new utils::TempList(), new utils::TempList(), new utils::LabelList()
                    )
                );
            }else{
                auto cp = assem::get_BigConst(off);
                auto ctmp = new utils::Temp(utils::TempType::INT);
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "movw `d0, #:lower16:" + std::to_string(off), 
                        new utils::TempList(), new utils::TempList{ctmp}, new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "movt `d0, #:upper16:" + std::to_string(off), 
                        new utils::TempList(), new utils::TempList{ctmp}, new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "sub sp, sp, `s0",
                        new utils::TempList{ctmp}, new utils::TempList(), new utils::LabelList()
                    )
                );

            }
        }
        string funname = *name;
        auto sp_tmp = new utils::Temp(utils::TempType::INT);
        if(*name == "putfloat"){
            instrlist->emplace_back(
                new assem::OperInstr(
                    "vmov `d0, `s0", new utils::TempList{utils::Temp::new_Temp(utils::TempType::FLOAT, new string("0"))}, 
                    new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string("0"))}, new utils::LabelList()
                )
            );
        }
        auto fix_call_use_reg = new utils::TempList(call_use_reg);
        for(int i = 0; i < min(4, (int)args->size()); i++){
            fix_call_use_reg->emplace_back(utils::Temp::new_Temp(utils::TempType::INT, new string(to_string(i))));
        }
        if(funname == "_sysy_starttime" || funname == "_sysy_stoptime"){
           
            fix_call_use_reg->emplace_back(utils::Temp::new_Temp(utils::TempType::INT, new string("0")));
            instrlist->emplace_back(
                new assem::OperInstr(
                    "mov `d0, #0", new utils::TempList(), 
                    new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string("0"))}, new utils::LabelList()
                )
            );
        }
        instrlist->emplace_back(
            new assem::OperInstr(
                "bl " + funname,
                fix_call_use_reg, 
                new utils::TempList(call_def_reg),
                new utils::LabelList()
            )
        );
        
        if(arg_n > 4){
            auto add_back_sp = new Move(
                new Temp(utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))),
                new Binop(BinOp::PLUS, new Temp(utils::Temp::new_Temp(utils::TempType::INT, new string("sp"))),
                new Const((arg_n-4)*DATA_SIZE)));
            add_back_sp->ir2assem(instrlist);
        }
        if(call->type != utils::TempType::VOID){
            assem::emit_mov_back(instrlist, 
            new utils::TempList{dst}, new utils::TempList{utils::Temp::new_Temp(call->type, new string("0"))},
            0, call->type);
        }
    }
    
}


utils::Temp* Phi::ir2assem(assem::Instrlist* instrlist){
    assert(0);
}
assem::Instrlist* ir2assem(mir::Fun* fun, mir::StmList* slist){
    if(call_def_reg.empty()){
        call_def_reg = {
            utils::Temp::new_Temp(utils::TempType::INT, new string("0")), utils::Temp::new_Temp(utils::TempType::INT, new string("1")),
            utils::Temp::new_Temp(utils::TempType::INT, new string("2")), utils::Temp::new_Temp(utils::TempType::INT, new string("3")),
            utils::Temp::new_Temp(utils::TempType::INT, new string("lr")),utils::Temp::new_Temp(utils::TempType::INT, new string("12"))
        };
        for(int i = 0; i < 32; i++){
            call_def_reg.emplace_back(
                utils::Temp::new_Temp(utils::TempType::FLOAT, new string(std::to_string(i)))
            );
        }
    }
    if(call_use_reg.empty()){
        // call_use_reg = {
        //     utils::Temp::new_Temp(utils::TempType::INT, new string("0")), utils::Temp::new_Temp(utils::TempType::INT, new string("1")),
        //     utils::Temp::new_Temp(utils::TempType::INT, new string("2")), utils::Temp::new_Temp(utils::TempType::INT, new string("3")),
            
        // };
        for(int i = 0; i < 4; i++){
            call_use_reg.emplace_back(
                utils::Temp::new_Temp(utils::TempType::FLOAT, new string(std::to_string(i)))
            );
        }
    }
    assem::InstrList* instrlist = new assem::InstrList();
    // function label
    // cout << "function label" << endl;
    instrlist->emplace_back(
        new assem::OperInstr(
            ".global " + *fun->name,
            new utils::TempList(), new utils::TempList(),
            new utils::LabelList()
        )
    );
    // cout << "function label 1" << endl;
    instrlist->emplace_back(
        new assem::LabelInstr(new utils::Label(new string(*fun->name)))
    );
    // push stack
    // cout << "push stack" << endl;
    instrlist->emplace_back(
        new assem::OperInstr(
            "push {fp,`s0}",
            new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string("lr"))},
            new utils::TempList(), new utils::LabelList()
        )
    );
    // save frame
    // cout << "save frame" << endl;
    instrlist->emplace_back(
        new assem::OperInstr(
            "mov fp, sp",
            new utils::TempList(),
            new utils::TempList(), new utils::LabelList()
        )
    );
    // sp increase for spill, now void instr
    instrlist->emplace_back(
        new assem::OperInstr(
            "", new utils::TempList(), new utils::TempList(), new utils::LabelList()
        )
    );
    // callee save reg
    // cout << "callee save reg" << endl;
    for(int i = 4; i <= 10; i++){
        instrlist->emplace_back(
            new assem::MoveInstr(
                "mov `d0, `s0",
                new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string(to_string(i)))},
                new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string("callee_save_" + to_string(i)))}
            )
        );
    }
    // get args
    // cout << "get args" << endl;
    int arg_n = 0;
    int tot_n = fun->args->size();
    for(auto& arg: *fun->args){
        if(arg_n < 4){
            assem::emit_mov_back(instrlist, new utils::TempList{arg},
                new utils::TempList{utils::Temp::new_Temp(arg->type, new string(to_string(arg_n)))},
                0, arg->type);
        }else{
            auto off = (tot_n - arg_n + 2 - 1) * DATA_SIZE;
            assem::emit_ldr_back(instrlist, new utils::TempList{arg},
                new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new string("fp"))},
                off, arg->type);
        }
        arg_n++;
    }
    for(auto& s: *slist){
        // s->print();
        s->ir2assem(instrlist);
    }
    
    // dump_GlobalInfo::global_table();
    for(auto it = instrlist->begin(); it != instrlist->end(); ){
        auto it_next = it;
        it_next++;
        auto i = *it;
        auto defs = i->get_def();
        auto uses = i->get_use();
        for(auto& def: *defs){
            if(GlobalInfo::global_table.count(def)){
                assert(0);   
            }
        }
        for(auto& use: *uses){
            if(GlobalInfo::global_table.count(use)){
                GlobalInfo* gi = GlobalInfo::global_table[use];
                switch (gi->type)
                {
                case GlobalInfoType::FORMAT:
                {
                    instrlist->emplace(it,
                        new assem::OperInstr(
                            "movw `d0, #:lower16:" + *gi->label->id,
                            new utils::TempList(), new utils::TempList{use},
                            new utils::LabelList()
                        )
                    );
                    instrlist->emplace(it,
                        new assem::OperInstr(
                            "movt `d0, #:upper16:" + *gi->label->id,
                            new utils::TempList(), new utils::TempList{use},
                            new utils::LabelList()
                        )
                    );
                }
                break;
                default:
                assert(0);
                break;
                }
            }
        }
        it = it_next;
    }
    return instrlist;
}


static void extract_global_array_initVal(Seq* seq, ConstMap* cl){
    if(seq->left->kind == StmType::SEQ){
        extract_global_array_initVal(static_cast<Seq*>(seq->left), cl);
    }else{
        // get index
        int index = 0;
        auto l_move = static_cast<Move*>(seq->left);
        auto m_dst = static_cast<Mem*>(l_move->dst);
        if(m_dst->exp->kind == ExpType::TEMP){
            index = 0;
        }else if(m_dst->exp->kind == ExpType::BINOP){
            auto bop = static_cast<Binop*>(m_dst->exp);
            auto c = static_cast<Const*>(bop->right);
            index = c->n.i / DATA_SIZE;
        }else{
            assert(0);
        }
        // get val
        auto m_src = static_cast<Const*>(l_move->src);
        (*cl)[index] = m_src;
    }
    if(seq->right->kind == StmType::SEQ){
        extract_global_array_initVal(static_cast<Seq*>(seq->right), cl);
    }else{
        // get index
        int index = 0;
        auto l_move = static_cast<Move*>(seq->right);
        auto m_dst = static_cast<Mem*>(l_move->dst);
        if(m_dst->exp->kind == ExpType::TEMP){
            index = 0;
        }else if(m_dst->exp->kind == ExpType::BINOP){
            auto bop = static_cast<Binop*>(m_dst->exp);
            auto c = static_cast<Const*>(bop->right);
            index = c->n.i / DATA_SIZE;
        }else{
            assert(0);
        }
        // get val
        auto m_src = static_cast<Const*>(l_move->src);
        (*cl)[index] = m_src;
    }
}

void handle_global_stm(Stm* s){
    // cout << endl;
    // cout << endl;
    // cout << endl;
    // s->print();
    switch (s->kind)
    {
    case StmType::MOVE:
    {
        auto u_move = static_cast<Move*>(s);
        auto dst_tmp = static_cast<Temp*>(u_move->dst)->temp;
        if(u_move->src->kind == ExpType::CONST){
            GlobalInfo::global_table[dst_tmp] = new GlobalInfo(
                new utils::Label(utils::SpecialLabelType::LC), 
                static_cast<Const*>(u_move->src));
        }else if(u_move->src->kind == ExpType::ESEQ){
            auto eseq = static_cast<Eseq*>(u_move->src);
            if(eseq->stm->kind == StmType::MOVE){
                auto eseq_move = static_cast<Move*>(eseq->stm);
                auto call_param = static_cast<Call*>(eseq_move->src)->args->front();
                int size = static_cast<Const*>(call_param)->n.i / DATA_SIZE;
                auto cl = new ConstMap();
                (*cl)[-1] = new Const(size);
                GlobalInfo::global_table[dst_tmp] = new GlobalInfo(
                    new utils::Label(utils::SpecialLabelType::LC), cl
                );
            }else if(eseq->stm->kind == StmType::SEQ){
                auto seq = static_cast<Seq*>(eseq->stm);
                auto alloc = static_cast<Move*>(seq->left);
                auto call_param = static_cast<Call*>(alloc->src)->args->front();
                int size = static_cast<Const*>(call_param)->n.i / DATA_SIZE;
                auto cl = new ConstMap();
                (*cl)[-1] = new Const(size);
                if(seq->right->kind == StmType::MOVE){
                    auto mov = static_cast<Move*>(seq->right);
                    // get index
                    int index = 0;
                    auto m_dst = static_cast<Mem*>(mov->dst);
                    if(m_dst->exp->kind == ExpType::TEMP){
                        index = 0;
                    }else if(m_dst->exp->kind == ExpType::BINOP){
                        auto bop = static_cast<Binop*>(m_dst->exp);
                        auto c = static_cast<Const*>(bop->right);
                        index = c->n.i / DATA_SIZE;
                    }else{
                        assert(0);
                    }
                    // get val
                    auto m_src = static_cast<Const*>(mov->src);
                    (*cl)[index] = m_src;
                }else if(seq->right->kind == StmType::SEQ){
                    extract_global_array_initVal(static_cast<Seq*>(seq->right), cl);
                }else{
                    assert(0);
                }
                
                GlobalInfo::global_table[dst_tmp] = new GlobalInfo(
                    new utils::Label(utils::SpecialLabelType::LC), cl
                );
            }else{
                assert(0);
            }
        }else{
            assert(0);
        }
    }
    break;
    case StmType::SEQ:
    {
        auto seq = static_cast<Seq*>(s);
        handle_global_stm(seq->left);
        handle_global_stm(seq->right);
    }
    break;
    default:
        assert(0);
    break;
    }
}

void get_global_table(mir::UnitList* ul){
    // cout << "get_GlobalInfo::global_table in" << endl;
    for(auto& u: *ul){
        if(u->kind == mir::UnitType::STM){
            auto u_stm = static_cast<Stm*>(u);
            handle_global_stm(u_stm);
        }
    }
    // cout << "get_GlobalInfo::global_table out" << endl;
}

assem::Instrlist* assem_add_epilog(assem::InstrList* instrlist){
    auto epilog = new assem::InstrList();
    epilog->emplace_back(
        new assem::OperInstr(
            ".arch armv7ve\n.arm", new utils::TempList(), new utils::TempList(), new utils::LabelList()
        )
    );
    epilog->emplace_back(
        new assem::OperInstr(
            ".data", new utils::TempList(), new utils::TempList(), new utils::LabelList()
        )
    );
    for(auto& g: GlobalInfo::global_table){
        if(g.second->type == GlobalInfoType::FORMAT) continue;
        if(g.second->type == GlobalInfoType::ARRAY && g.second->u.clist->size() == 1) continue;
        epilog->emplace_back(
            new assem::LabelInstr(g.second->label)
        );
        switch (g.second->type)
        {
        case GlobalInfoType::ARRAY:
        {
            int prev_index = 0;
            int tot_size = 0;
            for(auto& c: *g.second->u.clist){
                if(c.first == -1){
                    tot_size = c.second->n.i;
                    continue;
                }
                if(c.first > prev_index){
                    epilog->emplace_back(
                        new assem::OperInstr(
                            ".space " + to_string((c.first - prev_index) * DATA_SIZE),
                            new utils::TempList(), new utils::TempList(), new utils::LabelList()
                        )
                    );
                }
                epilog->emplace_back(
                    new assem::OperInstr(
                        ".word  " + c.second->to_string(), new utils::TempList(), new utils::TempList(), new utils::LabelList()
                    )
                );
                prev_index = c.first + 1;
            }
            assert(tot_size);
            if(tot_size > prev_index){
                epilog->emplace_back(
                    new assem::OperInstr(
                        ".space " + to_string((tot_size - prev_index) * DATA_SIZE),
                        new utils::TempList(), new utils::TempList(), new utils::LabelList()
                    )
                );
            }
        }
        break;
        case GlobalInfoType::CONST:
        {
            epilog->emplace_back(
                new assem::OperInstr(
                    ".word  " + g.second->u.c->to_string(), new utils::TempList(), new utils::TempList(), new utils::LabelList()
                )
            );
        }
        break;
        default:
        assert(0);
        break;
        }
    }
    epilog->emplace_back(
        new assem::OperInstr(
            ".section   .rodata", new utils::TempList(), new utils::TempList(), new utils::LabelList()
        )
    );
    for(auto& g: GlobalInfo::global_table){
        if(g.second->type == GlobalInfoType::FORMAT){
            epilog->emplace_back(
                new assem::LabelInstr(g.second->label)
            );
            epilog->emplace_back(
                new assem::OperInstr(
                    ".ascii \"" + *g.second->u.format + "\"",
                    new utils::TempList(), new utils::TempList(), new utils::LabelList()
                )
            );
        }
    }
    epilog->emplace_back(
        new assem::OperInstr(
            ".bss", new utils::TempList(), new utils::TempList(), new utils::LabelList()
        )
    );
    for(auto& g: GlobalInfo::global_table){
        if(g.second->type == GlobalInfoType::FORMAT) continue;
        if(!(g.second->type == GlobalInfoType::ARRAY && g.second->u.clist->size() == 1)) continue;

        epilog->emplace_back(
            new assem::LabelInstr(g.second->label)
        );
        switch (g.second->type)
        {
        case GlobalInfoType::ARRAY:
        {
            int prev_index = 0;
            int tot_size = 0;
            for(auto& c: *g.second->u.clist){
                if(c.first == -1){
                    tot_size = c.second->n.i;
                    continue;
                }
                if(c.first > prev_index){
                    epilog->emplace_back(
                        new assem::OperInstr(
                            ".space " + to_string((c.first - prev_index) * DATA_SIZE),
                            new utils::TempList(), new utils::TempList(), new utils::LabelList()
                        )
                    );
                }
                epilog->emplace_back(
                    new assem::OperInstr(
                        ".word  " + c.second->to_string(), new utils::TempList(), new utils::TempList(), new utils::LabelList()
                    )
                );
                prev_index = c.first + 1;
            }
            assert(tot_size);
            if(tot_size > prev_index){
                epilog->emplace_back(
                    new assem::OperInstr(
                        ".space " + to_string((tot_size - prev_index) * DATA_SIZE),
                        new utils::TempList(), new utils::TempList(), new utils::LabelList()
                    )
                );
            }
        }
        break;
        case GlobalInfoType::CONST:
        {
            epilog->emplace_back(
                new assem::OperInstr(
                    ".word  " + g.second->u.c->to_string(), new utils::TempList(), new utils::TempList(), new utils::LabelList()
                )
            );
        }
        break;
        default:
        assert(0);
        break;
        }
    }
    epilog->emplace_back(
        new assem::OperInstr(
            ".text", new utils::TempList(), new utils::TempList(), new utils::LabelList()
        )
    );
    epilog->splice(epilog->end(), *instrlist);
    return epilog;
}



}