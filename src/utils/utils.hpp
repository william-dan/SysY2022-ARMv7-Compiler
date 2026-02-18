#ifndef __UTILS_HPP
#define __UTILS_HPP

#include "../structure/mir/mir.hpp"
#include "entry.hpp"
#include "temp.hpp"

enum class btype_t;



static float i2f(int i) {
    return (float)i;
}
static int f2i(float f) {
    return (int)f;
}

static inline mir::Stm* nopStm() { 
    return (new mir::ExpStm(new mir::Const(0))); 
}
static bool isNop(mir::Stm* x) {
    return x->kind == mir::StmType::EXP && 
            (static_cast<mir::ExpStm*>(x)->exp->kind == mir::ExpType::CONST);
}

static mir::Stm* seq(mir::Stm* x, mir::Stm* y) {
    if (isNop(x))   return y;
    if (isNop(y))   return x;
    return new mir::Seq(x, y);
}

static mir::Exp* mir_i2f(mir::Exp* exp) {
    if (exp->kind == mir::ExpType::CONST) {
        auto a = static_cast<mir::Const*>(exp)->n;
        return new mir::Const(float(a.i));
    }
    else {
        return new mir::Call(new std::string("@i2f"), new mir::ExpList(1, exp), utils::TempType::FLOAT);
    }
}
static mir::Exp* mir_f2i(mir::Exp* exp) {
    if (exp->kind == mir::ExpType::CONST) {
        auto a = static_cast<mir::Const*>(exp)->n;
        return new mir::Const(int(a.f));
    }
    else {
        return new mir::Call(new std::string("@f2i"), new mir::ExpList(1, exp), utils::TempType::INT);
    }
}

static int cal_int( int l, mir::BinOp op, int r) {
    switch (op) {
        case mir::BinOp::PLUS:        return l + r;
        case mir::BinOp::MINUS:       return l - r;
        case mir::BinOp::MUL:         return l * r;
        case mir::BinOp::DIV:         return l / (r != 0 ? r : 1);
        default: assert(0);
    }
}


static float cal_float(float l, mir::BinOp op, float r) {
    switch (op) {
        case mir::BinOp::PLUS:        return l + r;
        case mir::BinOp::MINUS:       return l - r;
        case mir::BinOp::MUL:         return l * r;
        case mir::BinOp::DIV:         return l / (r != 0 ? r : 1);
        default: assert(0);
    }
}


static ENTRY::Type* binaryOpExpType(mir::BinOp op, ENTRY::Type* l, ENTRY::Type* r) {
    if (l->kind == ENTRY::Type_t::Type_INT && r->kind == ENTRY::Type_t::Type_INT) {
        int lValue = *l->Ivalue, rValue = *r->Ivalue;
        int newint = 0;
        if (op == mir::BinOp::MOD) {
            if (rValue == 0) {
                newint = lValue % 1;
            }
            else {
                newint = lValue % rValue;
            }
            return ENTRY::createIntInfo(new int(newint), false);
        }
        else {
            newint = cal_int(lValue, op, rValue);
            return ENTRY::createIntInfo(new int(newint), false);
        }
    }
    else if (l->kind == ENTRY::Type_t::Type_FLOAT && r->kind == ENTRY::Type_t::Type_INT) {
        float lValue = *l->Fvalue, rValue = (float)*r->Ivalue;
        float newfloat = cal_float(lValue, op, rValue);
        // new int 开辟4字节指针 应该会让 float 保持原状？
        return ENTRY::createFloatInfo(new float(newfloat), false);
    }
    else if (l->kind == ENTRY::Type_t::Type_INT && r->kind == ENTRY::Type_t::Type_FLOAT) {
        float lValue = (float)*l->Ivalue, rValue = *r->Fvalue;
        float newfloat = cal_float(lValue, op, rValue);
        return ENTRY::createFloatInfo(new float(newfloat), false);
    }
    else if (l->kind == ENTRY::Type_t::Type_FLOAT && r->kind == ENTRY::Type_t::Type_FLOAT) {
        float lValue = (float)*l->Fvalue, rValue = *r->Fvalue;
        float newfloat = cal_float(lValue, op, rValue);
        return ENTRY::createFloatInfo(new float(newfloat), false);
    }
    else {
        assert(0);
    }
}

static mir::Exp* calIRExp_float(mir::BinOp op, mir::Exp* lexp, mir::Exp* rexp) {
    switch (op) {
        case mir::BinOp::PLUS: return new mir::Binop(mir::BinOp::PLUS, lexp, rexp);
        case mir::BinOp::MINUS: return new mir::Binop(mir::BinOp::MINUS, lexp, rexp);
        case mir::BinOp::MUL: return new mir::Binop(mir::BinOp::MUL, lexp, rexp);
        case mir::BinOp::DIV: return new mir::Binop(mir::BinOp::DIV, lexp, rexp);
        default: assert(0);
    }
    return nullptr;
}

static mir::Exp* binaryMIRexp(mir::BinOp op, ENTRY::Type* lty, mir::Exp* lexp, 
                            ENTRY::Type* rty, mir::Exp* rexp) {
    if (lty->kind == ENTRY::Type_t::Type_INT && rty->kind == ENTRY::Type_t::Type_INT) {
        return new mir::Binop(op, lexp, rexp);
    } else if (lty->kind == ENTRY::Type_t::Type_INT && rty->kind == ENTRY::Type_t::Type_FLOAT) {
        lexp = mir_i2f(lexp);
        return calIRExp_float(op, lexp, rexp);
    } else if (lty->kind == ENTRY::Type_t::Type_FLOAT && rty->kind == ENTRY::Type_t::Type_INT) {
        rexp = mir_i2f(rexp);
        return calIRExp_float(op, lexp, rexp);
    } else if (lty->kind == ENTRY::Type_t::Type_FLOAT && rty->kind == ENTRY::Type_t::Type_FLOAT) {
        return calIRExp_float(op, lexp, rexp);
    } 
    else {
        assert(0);
    }
        
}


static void doPatch(mir::patchList* aList, utils::Label* label) {
    for (; aList; aList = aList->tail) {
        (aList->head)->id = label->id;
    }
}

static mir::patchList* joinPatch(mir::patchList* a, mir::patchList* b) {
    if (!a) return b;
    mir::patchList* tmp = a;
    for (; tmp->tail; tmp = tmp->tail)
        ;
    tmp->tail = b;
    return a;
}

static ENTRY::Type_t traversesArrayType(ENTRY::Type* p) {
    while (p->kind == ENTRY::Type_t::Type_ARRAY) {   
        p = p->point;
    }
    return p->kind;
}



#endif

