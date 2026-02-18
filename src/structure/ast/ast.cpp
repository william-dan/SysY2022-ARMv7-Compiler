#include <assert.h>
#include <cstring>
#include <string>
#include <cmath>


#include "../../utils/table.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/temp.hpp"
#include "../../utils/entry.hpp"
#include "../mir/mir.hpp"
#include "ast.hpp"

#define SIZEOFINT 4


// TODO: 必须
// todo: 可能


// TODO: 有关于隐式转换可能有更多的错误，有待测试
// TODO: 非main函数中的数组定义不能用malloc
// TODO: 常量折叠 functional 78


// utils

static ENTRY::Type* createBtypeInfo(AST::type t) {
    switch (t) {
        case AST::type::INT: return ENTRY::createIntInfo(new int(0), false);
        case AST::type::FLOAT: return ENTRY::createFloatInfo(new float(0.0), false);
        case AST::type::VOID: return ENTRY::createVoidInfo();
        default: assert(0);
    }
}
static utils::TempType type2Type_t(AST::type t) {
    switch (t) {
        case AST::type::INT: return utils::TempType::INT;
        case AST::type::FLOAT: return utils::TempType::FLOAT;
        case AST::type::VOID: return utils::TempType::VOID;
        default: assert(0);
    }
}

static mir::Const mircalconst(const mir::Const* lexp, mir::BinOp op, const mir::Const* rexp) {
    switch (op) {
    case mir::BinOp::PLUS:
        if (lexp->type == utils::TempType::INT && rexp->type == utils::TempType::INT) return mir::Const(lexp->n.i + rexp->n.i);
        if (lexp->type == utils::TempType::INT && rexp->type == utils::TempType::FLOAT) return mir::Const(lexp->n.i + rexp->n.f);
        if (lexp->type == utils::TempType::FLOAT && rexp->type == utils::TempType::INT) return mir::Const(lexp->n.f + rexp->n.i);
        if (lexp->type == utils::TempType::FLOAT && rexp->type == utils::TempType::FLOAT) return mir::Const(lexp->n.f + rexp->n.f);
        break;
    case mir::BinOp::MINUS:
        if (lexp->type == utils::TempType::INT && rexp->type == utils::TempType::INT) return mir::Const(lexp->n.i - rexp->n.i);
        if (lexp->type == utils::TempType::INT && rexp->type == utils::TempType::FLOAT) return mir::Const(lexp->n.i - rexp->n.f);
        if (lexp->type == utils::TempType::FLOAT && rexp->type == utils::TempType::INT) return mir::Const(lexp->n.f - rexp->n.i);
        if (lexp->type == utils::TempType::FLOAT && rexp->type == utils::TempType::FLOAT) return mir::Const(lexp->n.f - rexp->n.f);
        break;
    case mir::BinOp::MUL:
        if (lexp->type == utils::TempType::INT && rexp->type == utils::TempType::INT) return mir::Const(lexp->n.i * rexp->n.i);
        if (lexp->type == utils::TempType::INT && rexp->type == utils::TempType::FLOAT) return mir::Const(lexp->n.i * rexp->n.f);
        if (lexp->type == utils::TempType::FLOAT && rexp->type == utils::TempType::INT) return mir::Const(lexp->n.f * rexp->n.i);
        if (lexp->type == utils::TempType::FLOAT && rexp->type == utils::TempType::FLOAT) return mir::Const(lexp->n.f * rexp->n.f);
        break;
    case mir::BinOp::DIV:
        if (lexp->type == utils::TempType::INT && rexp->type == utils::TempType::INT) return mir::Const(lexp->n.i / rexp->n.i);
        if (lexp->type == utils::TempType::INT && rexp->type == utils::TempType::FLOAT) return mir::Const(lexp->n.i / rexp->n.f);
        if (lexp->type == utils::TempType::FLOAT && rexp->type == utils::TempType::INT) return mir::Const(lexp->n.f / rexp->n.i);
        if (lexp->type == utils::TempType::FLOAT && rexp->type == utils::TempType::FLOAT) return mir::Const(lexp->n.f / rexp->n.f);
        break;
    case mir::BinOp::MOD:
        if (lexp->type == utils::TempType::INT && rexp->type == utils::TempType::INT) return mir::Const(lexp->n.i % rexp->n.i);
        if (lexp->type == utils::TempType::INT && rexp->type == utils::TempType::FLOAT) assert(0);
        if (lexp->type == utils::TempType::FLOAT && rexp->type == utils::TempType::INT) assert(0);
        if (lexp->type == utils::TempType::FLOAT && rexp->type == utils::TempType::FLOAT) assert(0);
        break;
    default:
        assert(0);
    }
}


// print

void AST::printBlock(int depth) {
    std::cout <<  std::string(3*depth, ' ');
}

void AST::CompUnits::printTree(int depth) {
    int _depth = depth;
    if (list.size() > 1) {
        printBlock(_depth);
        std::cout << name << "\n";
        _depth += 1;
    }
    
    for (auto& i: list) {
        i->printTree(_depth);
    }
}


void AST::ConstDecl::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (defs)
        defs->printTree(depth + 1);
}

void AST::ConstDefs::printTree(int depth) {
    int _depth = depth;
    if (list.size() > 1) {
        printBlock(_depth);
        std::cout << name << "\n";
        _depth += 1;
    }
    
    for (auto& i: list) {
        i->printTree(_depth);
    }
}

void AST::ConstDef::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (id)
        id->printTree(depth + 1);
    if (initval)
        initval->printTree(depth + 1);
    if (index_list)
        index_list->printTree(depth + 1);
}

void AST::VarDecl::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (defs)
        defs->printTree(depth + 1);
}

void AST::VarDefs::printTree(int depth) {
    int _depth = depth;
    if (list.size() > 1) {
        printBlock(_depth);
        std::cout << name << "\n";
        _depth += 1;
    }
    
    for (auto& i: list) {
        i->printTree(_depth);
    }
}

void AST::VarDef::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    id->printTree(depth + 1);
    index_list->printTree(depth + 1);
    if (initval)
        initval->printTree(depth + 1);
}

void AST::Arr_Index::printTree(int depth) {
    int _depth = depth;
    if (list.size() > 1) {
        printBlock(_depth);
        std::cout << name << "\n";
        _depth += 1;
    }
    
    for (auto& i: list) {
        i->printTree(_depth);
    }
}


void AST::FuncDef::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (id)
        id->printTree(depth + 1);
    if (parameters)
        parameters->printTree(depth + 1);
    if (block)
        block->printTree(depth + 1);
}

void AST::Parameters::printTree(int depth) {
    int _depth = depth;
    if (list.size() > 1) {
        printBlock(_depth);
        std::cout << name << "\n";
        _depth += 1;
    }
    
    for (auto& i: list) {
        i->printTree(_depth);
    }
}

void AST::Parameter::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (id)
        id -> printTree(depth + 1);
}

void AST::Block::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (items)
        items->printTree(depth + 1);
}

void AST::BlockItems::printTree(int depth) {
    int _depth = depth;
    if (list.size() > 1) {
        printBlock(_depth);
        std::cout << name << "\n";
        _depth += 1;
    }
    
    for (auto& i: list) {
        i->printTree(_depth);
    }
}

void AST::AssignStmt::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (lval)
        lval->printTree(depth + 1);
    if (exp)
        exp->printTree(depth + 1);
}

void AST::ExpStmt::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (exp)
        exp->printTree(depth + 1);
}

void AST::IfStmt::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (exp)
        exp->printTree(depth + 1);
    if_part->printTree(depth + 1);
    if (else_part)
        else_part->printTree(depth + 1);
}

void AST::WhileStmt::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (exp)
        exp->printTree(depth + 1);
    if (loop)
        loop->printTree(depth + 1);
}

void AST::BreakStmt::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
}

void AST::ContinueStmt::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
}

void AST::ReturnStmt::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (exp)
        exp->printTree(depth + 1);
}

void AST::IdExp::printTree(int depth) {
    printBlock(depth);
    std::cout << name << ": " << *str
              << "\n";
}

void AST::Lval::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (id)
        id->printTree(depth);
    if (arrayindex)
        arrayindex->printTree(depth);
}

void AST::IntNum::printTree(int depth) {
    printBlock(depth);
    std::cout << name << ": " << value << "\n";
}

void AST::FloatNum::printTree(int depth) {
    printBlock(depth);
    std::cout << name << ": " << *str << "\n";
}

void AST::UnaryExp::printTree(int depth) {
    printBlock(depth);
    std::cout << name << " ";
    if(ast_isconst() && constval.is_int)
        std::cout << constval.val.i;
    else if(ast_isconst() && !constval.is_int)
        std::cout << constval.val.f;
    std::cout<<"\n";
    if (exp)
        exp->printTree(depth + 1);
}

void AST::CallExp::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (id)
        id->printTree(depth + 1);
    if (params)
        params->printTree(depth + 1);
}

void AST::Exps::printTree(int depth) {
    int _depth = depth;
    if (list.size() > 1) {
        printBlock(_depth);
        std::cout << name << "\n";
        _depth += 1;
    }
    
    for (auto& i: list) {
        i->printTree(_depth);
    }
}

void AST::MulExp::printTree(int depth) {
    printBlock(depth);
    std::cout << name << " ";
    if(ast_isconst() && constval.is_int)
        std::cout << constval.val.i;
    else if(ast_isconst() && !constval.is_int)
        std::cout << constval.val.f;
    std::cout<<"\n";
    if (lexp)
        lexp->printTree(depth + 1);
    if (rexp)
        rexp->printTree(depth + 1);
}

void AST::AddExp::printTree(int depth) {
    printBlock(depth);
    std::cout << name << " ";
    if(ast_isconst() && constval.is_int)
        std::cout << constval.val.i;
    else if(ast_isconst() && !constval.is_int)
        std::cout << constval.val.f;
    std::cout<<"\n";
    if (lexp)
        lexp->printTree(depth + 1);
    if (rexp)
        rexp->printTree(depth + 1);
}

void AST::RelExp::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (lexp)
        lexp->printTree(depth + 1);
    if (rexp)
        rexp->printTree(depth + 1);
}

void AST::EqExp::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (lexp)
        lexp->printTree(depth + 1);
    if (rexp)
        rexp->printTree(depth + 1);
}

void AST::LAndExp::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (lexp)
        lexp->printTree(depth + 1);
    if (rexp)
        rexp->printTree(depth + 1);
}

void AST::LOrExp::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
    if (lexp)
        lexp->printTree(depth + 1);
    if (rexp)
        rexp->printTree(depth + 1);
}

void AST::StarttimeStmt::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
}

void AST::StoptimeStmt::printTree(int depth) {
    printBlock(depth);
    std::cout << name << "\n";
}

void AST::FormatExp::printTree(int depth) {
    printBlock(depth);
    std::cout << name << ": " << *str << "\n";
}

void initFenv(TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    ENTRY::Type* inttype = ENTRY::createIntInfo(new int(0), false);
    ENTRY::Type* floattype = ENTRY::createFloatInfo(new float(0.0), false);
    ENTRY::Type* voidtype = ENTRY::createVoidInfo();
    fenv->enter("getint", new ENTRY::EnFunction(
        ENTRY::createFunctionInfo(inttype, std::vector<ENTRY::Type*>()), new utils::Label(new std::string("getint")), utils::TempType::INT
    ));
    fenv->enter("getch", new ENTRY::EnFunction(
        ENTRY::createFunctionInfo(inttype, std::vector<ENTRY::Type*>()), new utils::Label(new std::string("getch")), utils::TempType::INT
    ));
    fenv->enter("getfloat", new ENTRY::EnFunction(
        ENTRY::createFunctionInfo(floattype, std::vector<ENTRY::Type*>()), new utils::Label(new std::string("getfloat")), utils::TempType::FLOAT
    ));
    fenv->enter("getarray", new ENTRY::EnFunction(
        ENTRY::createFunctionInfo(inttype, std::vector<ENTRY::Type*>(
            1, ENTRY::createArrayInfo(inttype, 1, false)
        )), new utils::Label(new std::string("getarray")), utils::TempType::INT
    ));
    fenv->enter("getfarray", new ENTRY::EnFunction(
        ENTRY::createFunctionInfo(inttype, std::vector<ENTRY::Type*>(
            1, ENTRY::createArrayInfo(floattype, 1, false)
        )), new utils::Label(new std::string("getfarray")), utils::TempType::INT
    ));
    fenv->enter("putint", new ENTRY::EnFunction(
        ENTRY::createFunctionInfo(voidtype, std::vector<ENTRY::Type*>(
            1, inttype
        )), new utils::Label(new std::string("putint")), utils::TempType::VOID
    ));
    fenv->enter("putch", new ENTRY::EnFunction(
        ENTRY::createFunctionInfo(voidtype, std::vector<ENTRY::Type*>(
            1, inttype
        )), new utils::Label(new std::string("putch")), utils::TempType::VOID
    ));
    fenv->enter("putfloat", new ENTRY::EnFunction(
        ENTRY::createFunctionInfo(voidtype, std::vector<ENTRY::Type*>(
            1, floattype
        )), new utils::Label(new std::string("putfloat")), utils::TempType::VOID
    ));
    fenv->enter("putarray", new ENTRY::EnFunction(
        ENTRY::createFunctionInfo(voidtype, std::vector<ENTRY::Type*>(
            {inttype, ENTRY::createArrayInfo(inttype, 1, false)}
        )), new utils::Label(new std::string("putarray")), utils::TempType::VOID
    ));
    fenv->enter("putfarray", new ENTRY::EnFunction(
        ENTRY::createFunctionInfo(voidtype, std::vector<ENTRY::Type*>(
            {inttype, ENTRY::createArrayInfo(floattype, 1, false)}
        )), new utils::Label(new std::string("putfarray")), utils::TempType::VOID
    ));
}

mir::UnitList* AST::CompUnits::semanticAnalysis(TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                           TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    mir::UnitList* units = new mir::UnitList();
    initFenv(fenv);
    assert(list.size() && "miss compunit");
    for (const auto& i: list) {
        mir::Unit* tmpUnit = i->semanticAnalysis(new utils::Label(new std::string("")), 
                            new utils::Label(new std::string("")), 
                            new utils::Label(new std::string("")), venv, fenv);
        units->push_back(tmpUnit);
    }
    return units;
}


mir::Fun* AST::FuncDef::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    ENTRY::Type* pointTy = ENTRY::createFunctionInfo(NULL, std::vector<ENTRY::Type*>());
    pointTy->point = createBtypeInfo(t);
    for (auto i : parameters->list) {
        if (i->arrayindex->list.empty()) {
            pointTy->param.emplace_back(createBtypeInfo(i->t));
        }
        else {
            ENTRY::Type* head;
            if (i->t == AST::type::INT)
                head = ENTRY::createIntInfo(new int(0), false);
            else
                head = ENTRY::createFloatInfo(new float(0.0), false);
            
            for (int j = (int)(i->arrayindex->list.size()) - 1; j >= 0; j--) {
                AST::Exp* exp = i->arrayindex->list[j];
                head = ENTRY::createArrayInfo(head, 0, false);
            }
            if (i->t == AST::type::INT) {
                head->Ivalue = new int[head->arraysize];
                memset(head->Ivalue, 0, head->arraysize * SIZEOFINT);
            }
            else if (i->t == AST::type::FLOAT) {
                head->Fvalue = new float[head->arraysize];
                memset(head->Fvalue, 0, head->arraysize * SIZEOFINT);
            }
            
            pointTy->param.emplace_back(head);
        }
    }
    label = new utils::Label(id->str);
    utils::Label* FunLabel = label;
    utils::TempType returnType = type2Type_t(t);
    fenv->enter(*FunLabel->id, new ENTRY::EnFunction(pointTy, FunLabel, returnType));

    venv->beginScope(NULL);

    // utils::TempList* args
    utils::TempList* args = new utils::TempList();
    for (AST::Parameter* i : parameters->list) {
        AST::VarDef(i->id, i->arrayindex, NULL, i->lineno).semanticAnalysis(i->t, label, venv, fenv);
        ENTRY::LocVar* entry
            = static_cast<ENTRY::LocVar*>(venv->find(*static_cast<AST::IdExp*>(i->id)->str));
        args->emplace_back(entry->temp);
    }
    mir::Stm* finalStm = block->semanticAnalysis(breakL, continueL, label, venv, fenv);
    venv->endScope();
    // Fun::Fun(string* name, utils::TempList* args, Stm* stm):
    return new mir::Fun(id->str, args, finalStm);
}



mir::Stm* AST::ConstDecl::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,
                                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    mir::Stm* stm = nopStm();
    for (auto& i: defs->list) {
        stm = seq(stm, i->semanticAnalysis(t, label, venv, fenv));
    }
    return stm;
}

mir::Stm* AST::ConstDef::semanticAnalysis(type t, utils::Label* label, 
                                          TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                          TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    
    // 全局变量声明中指定的初值表达式必须是常量表达式，并且一定有初值
    // 可能要区分 Loc 和 Glo
    /*
    Glo: 
    Loc: 数组存在栈帧中
    */
    switch (t) {
        case AST::type::INT: {
            if (index_list->list.empty()) {     // int
                utils::Temp* newTemp = new utils::Temp(utils::TempType::INT);
                mir::ExpTy* expty = initval->toexptyP(label, venv, fenv);
                assert((expty->ty->Ivalue || expty->ty->Fvalue) && "const need initval");
                int* newint = new int();
                // 隐式转换 f2i
                if (expty->ty->kind == ENTRY::Type_t::Type_FLOAT) {
                    *newint = f2i(*expty->ty->Fvalue);
                }
                else if (expty->ty->kind == ENTRY::Type_t::Type_INT) {
                    *newint = *expty->ty->Ivalue;
                }
                else {
                    assert(0);
                }
                if (*label->id == "") {         // global
                    venv->enter(*id->str, new ENTRY::GloVar(ENTRY::createIntInfo(newint, true), new utils::Temp()));
                }
                else {                          // local
                    venv->enter(*id->str, new ENTRY::LocVar(ENTRY::createIntInfo(newint, true), new utils::Temp()));
                }
                return new mir::Move(new mir::Temp(newTemp), new mir::Const(*newint));
            }
            else {                              // int array
                ENTRY::Type* arr_head = ENTRY::createIntInfo(new int(0), true);
                for (int i = (int)(index_list->list.size()) -1; i >= 0; i--) {
                    AST::Exp* p = index_list->list[i];
                    mir::ExpTy* expty = p->toexptyP(label, venv, fenv);
                    arr_head = ENTRY::createArrayInfo(arr_head, *expty->ty->Ivalue, true);
                }

                utils::Temp* pointTemp = new utils::Temp();
                utils::Temp* addrTemp = new utils::Temp();
                
                mir::ExpTy* assignValue = initval->arrayAssign(arr_head, new mir::Temp(addrTemp), 0, label, venv, fenv);
                mir::ExpList* mallocParam = new mir::ExpList();

                mallocParam->emplace_back(new mir::Const(arr_head->arraysize * SIZEOFINT));
                mir::Eseq* mallocSpace = new mir::Eseq(seq(new mir::Move(new mir::Temp(addrTemp), new mir::Call(new std::string("@calloc"), mallocParam, utils::TempType::INT)), assignValue->exp->unNX()), new mir::Temp(addrTemp));

                if (*label->id == "") {         // global
                    venv->enter(*id->str, new ENTRY::GloVar(arr_head, pointTemp));
                }
                else {                          // local
                    venv->enter(*id->str, new ENTRY::LocVar(arr_head, pointTemp));
                }
                return new mir::Move(new mir::Temp(pointTemp), mallocSpace);
            }
        }
        break;
        case AST::type::FLOAT: {
            if (index_list->list.empty()) {     // float
                utils::Temp* newTemp = new utils::Temp(utils::TempType::FLOAT);
                mir::ExpTy* expty = initval->toexptyP(label, venv, fenv);
                assert((expty->ty->Ivalue || expty->ty->Fvalue) && "const need initval");
                mir::Exp* newexp = expty->exp->unEX();
                float* newint = new float();
                // 隐式转换 i2f
                if (expty->ty->kind == ENTRY::Type_t::Type_INT) {
                    *newint = i2f(*expty->ty->Ivalue);
                    newexp = mir_i2f(newexp);
                }
                else if(expty->ty->kind == ENTRY::Type_t::Type_FLOAT) {
                    *newint = *expty->ty->Fvalue;
                }
                else {
                    assert(0);
                }
                if (*label->id == "") {         // global
                    venv->enter(*id->str, new ENTRY::GloVar(ENTRY::createFloatInfo(newint, true), new utils::Temp()));
                }
                else {                          // local
                    venv->enter(*id->str, new ENTRY::LocVar(ENTRY::createFloatInfo(newint, true), new utils::Temp()));
                }
                return new mir::Move(new mir::Temp(newTemp), newexp);
            }
            else {                              // float array
                ENTRY::Type* arr_head = ENTRY::createFloatInfo(new float(0.0), true);
                for (int i = (int)(index_list->list.size()) -1; i >= 0; i--) {
                    AST::Exp* p = index_list->list[i];
                    mir::ExpTy* expty = p->toexptyP(label, venv, fenv);
                    arr_head = ENTRY::createArrayInfo(arr_head, *expty->ty->Ivalue, true);
                }

                utils::Temp* pointTemp = new utils::Temp();
                utils::Temp* addrTemp = new utils::Temp();
                
                mir::ExpTy* assignValue = initval->arrayAssign(arr_head, new mir::Temp(addrTemp), 0, label, venv, fenv);
                mir::ExpList* mallocParam = new mir::ExpList();

                mallocParam->emplace_back(new mir::Const(arr_head->arraysize * SIZEOFINT));
                mir::Eseq* mallocSpace = new mir::Eseq(seq(new mir::Move(new mir::Temp(addrTemp), new mir::Call(new std::string("@calloc"), mallocParam, utils::TempType::INT)), assignValue->exp->unNX()), new mir::Temp(addrTemp));

                if (*label->id == "") {         // global
                    venv->enter(*id->str, new ENTRY::GloVar(arr_head, pointTemp));
                }
                else {                          // local
                    venv->enter(*id->str, new ENTRY::LocVar(arr_head, pointTemp));
                }
                return new mir::Move(new mir::Temp(pointTemp), mallocSpace);
            }
        }
        break;
        default: {
            assert(0 && "ConstDef semant");
        }
    }
}

mir::Stm* AST::VarDecl::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label, 
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    mir::Stm* stm = new mir::ExpStm(new mir::Const(0));
    for (auto& i : defs->list) { 
        stm = seq(stm, i->semanticAnalysis(t, label, venv, fenv));
    }
    // std::cout << int(stm->kind);
    return stm;
}


mir::ExpTy* AST::InitVals::arrayAssign(ENTRY::Type* ty, mir::Exp* addr, int offset, 
                                utils::Label* label, 
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {

    int offsetTmp = 0;
    mir::Stm* newStm = nopStm();
    
    ty->Ivalue = new int[ty->arraysize];
    memset(ty->Ivalue, 0, SIZEOFINT * ty->arraysize);
    ty->Fvalue = new float[ty->arraysize];
    memset(ty->Fvalue, 0, SIZEOFINT * ty->arraysize);
    
    for (const auto& i: list) {
        mir::ExpTy* expty = i->arrayAssign(ty->point, addr, offset + offsetTmp, label, venv, fenv);
        mir::Stm* tmpStm = expty->exp->unNX();
        newStm = seq(newStm, tmpStm);
        if (expty->ty->point != NULL) {
            std::memcpy(&ty->Ivalue[offsetTmp], expty->ty->Ivalue, SIZEOFINT * expty->ty->arraysize);
            delete expty->ty->Ivalue;
            std::memcpy(&ty->Fvalue[offsetTmp], expty->ty->Fvalue, SIZEOFINT * expty->ty->arraysize);
            delete expty->ty->Fvalue;
        }
        else {
            if (expty->ty->Ivalue) {
                ty->Ivalue[offsetTmp] = *expty->ty->Ivalue;
            }
            else if (expty->ty->Fvalue) {
                ty->Fvalue[offsetTmp] = *expty->ty->Fvalue;
            }
            else {
                assert(0);
            }
        }
        offsetTmp += expty->ty->arraysize;
    }

    return new mir::ExpTy(new mir::Tr_Exp(newStm), ty);
}



mir::Stm* AST::VarDef::semanticAnalysis(type t, utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    
    if (*label->id == "") {                         // global
        // TODO
        switch (t) {
            case AST::type::INT: {
                if (index_list->list.empty()) {     // int
                    if (initval) {
                        utils::Temp* newTemp = new utils::Temp(utils::TempType::INT);
                        mir::ExpTy* expty = initval->toexptyP(label, venv, fenv);
                        int* newint = new int();
                        mir::Exp* newexp = expty->exp->unEX();
                        // 隐式转换 f2i
                        if (expty->ty->kind == ENTRY::Type_t::Type_FLOAT) {
                            *newint = f2i(*expty->ty->Fvalue);
                            newexp = mir_f2i(newexp);
                        }
                        else if (expty->ty->kind == ENTRY::Type_t::Type_INT) {
                            *newint = *expty->ty->Ivalue;
                        }
                        else {
                            assert(0);
                        }
                        venv->enter(*id->str, new ENTRY::GloVar(ENTRY::createIntInfo(newint, false), newTemp));
                        return new mir::Move(new mir::Temp(newTemp), newexp);
                    }
                    else {
                        utils::Temp* newTemp = new utils::Temp(utils::TempType::INT);
                        venv->enter(*id->str, new ENTRY::GloVar(ENTRY::createIntInfo(new int(0), false), newTemp));
                        return new mir::Move(new mir::Temp(newTemp), new mir::Const((int)(0)));
                    }
                }
                else {                          // int array
                    ENTRY::Type* arr_head = ENTRY::createIntInfo(new int(0), false);
                    for (int i = (int)(index_list->list.size()) - 1; i >= 0; i--) {
                        AST::Exp* p = index_list->list[i];
                        mir::ExpTy* expty = p->toexptyP(label, venv, fenv);
                        arr_head = ENTRY::createArrayInfo(arr_head, *expty->ty->Ivalue, false);
                    }
                    
                    if (initval) {
                        utils::Temp* pointTemp = new utils::Temp();
                        utils::Temp* addrTemp = new utils::Temp();
                        venv->enter(*id->str, new ENTRY::GloVar(arr_head, pointTemp));
                        mir::ExpTy* assignValue = initval->arrayAssign(arr_head, new mir::Temp(addrTemp), 0, label, venv, fenv);
                        mir::ExpList* mallocParam = new mir::ExpList();

                        mallocParam->emplace_back(new mir::Const(arr_head->arraysize * SIZEOFINT));
                        mir::Eseq* mallocSpace = new mir::Eseq(seq(new mir::Move(new mir::Temp(addrTemp), new mir::Call(new std::string("@calloc"), mallocParam, utils::TempType::INT)), assignValue->exp->unNX()), new mir::Temp(addrTemp));
                        return new mir::Move(new mir::Temp(pointTemp), mallocSpace);
                    }
                    else {
                        utils::Temp* pointTemp = new utils::Temp();
                        utils::Temp* addrTemp = new utils::Temp();
                        arr_head->Ivalue = new int[arr_head->arraysize];
                        memset(arr_head->Ivalue, 0, SIZEOFINT * arr_head->arraysize);
                        venv->enter(*id->str, new ENTRY::GloVar(arr_head, pointTemp));
                        mir::ExpList* mallocParam = new mir::ExpList();

                        mallocParam->emplace_back(new mir::Const(arr_head->arraysize * SIZEOFINT));
                        mir::Eseq* mallocSpace = new mir::Eseq(new mir::Move(new mir::Temp(addrTemp), new mir::Call(new std::string("@calloc"), mallocParam, utils::TempType::INT)), new mir::Temp(addrTemp));
                        return new mir::Move(new mir::Temp(pointTemp), mallocSpace);
                    }
                }
            }
            break;
            case AST::type::FLOAT: {
                if (index_list->list.empty()) {  // float
                    if (initval) {
                        utils::Temp* newTemp = new utils::Temp(utils::TempType::FLOAT);
                        mir::ExpTy* expty = initval->toexptyP(label, venv, fenv);
                        float* newint = new float();
                        mir::Exp* newexp = expty->exp->unEX();
                        // 隐式转换 i2f
                        if (expty->ty->kind == ENTRY::Type_t::Type_INT) {
                            *newint = i2f(*expty->ty->Ivalue);
                            newexp = mir_i2f(newexp);
                        }
                        else if (expty->ty->kind == ENTRY::Type_t::Type_FLOAT) {
                            *newint = *expty->ty->Fvalue;
                        }
                        else {
                            assert(0);
                        }
                        venv->enter(*id->str, new ENTRY::GloVar(ENTRY::createFloatInfo(newint, false), newTemp));
                        return new mir::Move(new mir::Temp(newTemp), newexp);
                    }
                    else {
                        utils::Temp* newTemp = new utils::Temp(utils::TempType::FLOAT);
                        venv->enter(*id->str, new ENTRY::GloVar(ENTRY::createFloatInfo(new float(0.0), false), newTemp));
                        return new mir::Move(new mir::Temp(newTemp), new mir::Const((float)(0.0)));
                    }
                }
                else {                          // float array
                    ENTRY::Type* arr_head = ENTRY::createFloatInfo(new float(0.0), false);
                    for (int i = (int)(index_list->list.size()) - 1; i >= 0; i--) {
                        AST::Exp* p = index_list->list[i];
                        mir::ExpTy* expty = p->toexptyP(label, venv, fenv);
                        arr_head = ENTRY::createArrayInfo(arr_head, *expty->ty->Ivalue, false);
                    }
                    // 局部变量的数组应该在栈帧中(已删除)
                    if (initval) {
                        utils::Temp* pointTemp = new utils::Temp();
                        utils::Temp* addrTemp = new utils::Temp();
                        venv->enter(*id->str, new ENTRY::GloVar(arr_head, pointTemp));
                        mir::ExpTy* assignValue = initval->arrayAssign(arr_head, new mir::Temp(addrTemp), 0, label, venv, fenv);
                        mir::ExpList* mallocParam = new mir::ExpList();

                        mallocParam->emplace_back(new mir::Const(arr_head->arraysize * SIZEOFINT));
                        mir::Eseq* mallocSpace = new mir::Eseq(seq(new mir::Move(new mir::Temp(addrTemp), new mir::Call(new std::string("@calloc"), mallocParam, utils::TempType::INT)), assignValue->exp->unNX()), new mir::Temp(addrTemp));
                        return new mir::Move(new mir::Temp(pointTemp), mallocSpace);
                    }
                    else {
                        utils::Temp* pointTemp = new utils::Temp();
                        utils::Temp* addrTemp = new utils::Temp();
                        arr_head->Fvalue = new float[arr_head->arraysize];
                        memset(arr_head->Fvalue, 0, SIZEOFINT * arr_head->arraysize);
                        venv->enter(*id->str, new ENTRY::GloVar(arr_head, pointTemp));
                        mir::ExpList* mallocParam = new mir::ExpList();

                        mallocParam->emplace_back(new mir::Const(arr_head->arraysize * SIZEOFINT));
                        mir::Eseq* mallocSpace = new mir::Eseq(new mir::Move(new mir::Temp(addrTemp), new mir::Call(new std::string("@calloc"), mallocParam, utils::TempType::INT)), new mir::Temp(addrTemp));
                        return new mir::Move(new mir::Temp(pointTemp), mallocSpace);
                    }
                }
            }
            break;
            default: {
                assert(0);
            }
        }
    }
    else {                                          // loc
        switch (t) {
            case AST::type::INT: {
                if (index_list->list.empty()) {     // int
                    if (initval) {
                        utils::Temp* newTemp = new utils::Temp(utils::TempType::INT);
                        mir::ExpTy* expty = initval->toexptyP(label, venv, fenv);
                        int* newint = new int();
                        mir::Exp* newexp = expty->exp->unEX();
                        // 隐式转换 f2i
                        if (expty->ty->kind == ENTRY::Type_t::Type_FLOAT) {
                            *newint = f2i(*expty->ty->Fvalue);
                            newexp = mir_f2i(newexp);
                        }
                        else if (expty->ty->kind == ENTRY::Type_t::Type_INT) {
                            *newint = *expty->ty->Ivalue;
                        }
                        else {
                            assert(0);
                        }
                        venv->enter(*id->str, new ENTRY::LocVar(ENTRY::createIntInfo(newint, false), newTemp));
                        return new mir::Move(new mir::Temp(newTemp), newexp);
                    }
                    else {
                        utils::Temp* newTemp = new utils::Temp(utils::TempType::INT);
                        venv->enter(*id->str, new ENTRY::LocVar(ENTRY::createIntInfo(new int(0), false), newTemp));
                        return nopStm();
                    }
                }
                else {                          // int array
                    ENTRY::Type* arr_head = ENTRY::createIntInfo(new int(0), false);
                    for (int i = (int)(index_list->list.size()) - 1; i >= 0; i--) {
                        AST::Exp* p = index_list->list[i];
                        mir::ExpTy* expty = p->toexptyP(label, venv, fenv);
                        arr_head = ENTRY::createArrayInfo(arr_head, *expty->ty->Ivalue, false);
                    }
                    // 局部变量的数组应该在栈帧中(已删除)
                    if (initval) {
                        utils::Temp* pointTemp = new utils::Temp();
                        utils::Temp* addrTemp = new utils::Temp();
                        venv->enter(*id->str, new ENTRY::LocVar(arr_head, pointTemp));
                        mir::ExpTy* assignValue = initval->arrayAssign(arr_head, new mir::Temp(addrTemp), 0, label, venv, fenv);
                        mir::ExpList* mallocParam = new mir::ExpList();

                        mallocParam->emplace_back(new mir::Const(arr_head->arraysize * SIZEOFINT));
                        mir::Eseq* mallocSpace = new mir::Eseq(seq(new mir::Move(new mir::Temp(addrTemp), new mir::Call(new std::string("@calloc"), mallocParam, utils::TempType::INT)), assignValue->exp->unNX()), new mir::Temp(addrTemp));
                        return new mir::Move(new mir::Temp(pointTemp), mallocSpace);
                    }
                    else {
                        utils::Temp* pointTemp = new utils::Temp();
                        utils::Temp* addrTemp = new utils::Temp();
                        arr_head->Ivalue = new int[arr_head->arraysize];
                        memset(arr_head->Ivalue, 0, SIZEOFINT * arr_head->arraysize);
                        venv->enter(*id->str, new ENTRY::LocVar(arr_head, pointTemp));
                        mir::ExpList* mallocParam = new mir::ExpList();

                        mallocParam->emplace_back(new mir::Const(arr_head->arraysize * SIZEOFINT));
                        mir::Eseq* mallocSpace = new mir::Eseq(new mir::Move(new mir::Temp(addrTemp), new mir::Call(new std::string("@malloc"), mallocParam, utils::TempType::INT)), new mir::Temp(addrTemp));
                        return new mir::Move(new mir::Temp(pointTemp), mallocSpace);
                    }
                }
            }
            break;
            case AST::type::FLOAT: {
                if (index_list->list.empty()) {  // float
                    if (initval) {
                        utils::Temp* newTemp = new utils::Temp(utils::TempType::FLOAT);
                        mir::ExpTy* expty = initval->toexptyP(label, venv, fenv);
                        float* newint = new float();
                        mir::Exp* newexp = expty->exp->unEX();
                        // 隐式转换 i2f
                        if (expty->ty->kind == ENTRY::Type_t::Type_INT) {
                            *newint = i2f(*expty->ty->Ivalue);
                            newexp = mir_i2f(newexp);
                        }
                        else if (expty->ty->kind == ENTRY::Type_t::Type_FLOAT) {
                            *newint = *expty->ty->Fvalue;
                        }
                        else {
                            assert(0);
                        }
                        venv->enter(*id->str, new ENTRY::LocVar(ENTRY::createFloatInfo(newint, false), newTemp));
                        return new mir::Move(new mir::Temp(newTemp), newexp);
                    }
                    else {
                        utils::Temp* newTemp = new utils::Temp(utils::TempType::FLOAT);
                        venv->enter(*id->str, new ENTRY::LocVar(ENTRY::createFloatInfo(new float(0.0), false), newTemp));
                        return nopStm();
                    }
                }
                else {                          // float array
                    ENTRY::Type* arr_head = ENTRY::createFloatInfo(new float(0.0), false);
                    for (int i = (int)(index_list->list.size()) - 1; i >= 0; i--) {
                        AST::Exp* p = index_list->list[i];
                        mir::ExpTy* expty = p->toexptyP(label, venv, fenv);
                        arr_head = ENTRY::createArrayInfo(arr_head, *expty->ty->Ivalue, false);
                    }
                    // 局部变量的数组应该在栈帧中(已删除)
                    if (initval) {
                        utils::Temp* pointTemp = new utils::Temp();
                        utils::Temp* addrTemp = new utils::Temp();
                        venv->enter(*id->str, new ENTRY::LocVar(arr_head, pointTemp));
                        mir::ExpTy* assignValue = initval->arrayAssign(arr_head, new mir::Temp(addrTemp), 0, label, venv, fenv);
                        mir::ExpList* mallocParam = new mir::ExpList();

                        mallocParam->emplace_back(new mir::Const(arr_head->arraysize * SIZEOFINT));
                        mir::Eseq* mallocSpace = new mir::Eseq(seq(new mir::Move(new mir::Temp(addrTemp), new mir::Call(new std::string("@calloc"), mallocParam, utils::TempType::INT)), assignValue->exp->unNX()), new mir::Temp(addrTemp));
                        return new mir::Move(new mir::Temp(pointTemp), mallocSpace);
                    }
                    else {
                        utils::Temp* pointTemp = new utils::Temp();
                        utils::Temp* addrTemp = new utils::Temp();
                        arr_head->Fvalue = new float[arr_head->arraysize];
                        memset(arr_head->Fvalue, 0, SIZEOFINT * arr_head->arraysize);
                        venv->enter(*id->str, new ENTRY::LocVar(arr_head, pointTemp));
                        mir::ExpList* mallocParam = new mir::ExpList();

                        mallocParam->emplace_back(new mir::Const(arr_head->arraysize * SIZEOFINT));
                        mir::Eseq* mallocSpace = new mir::Eseq(new mir::Move(new mir::Temp(addrTemp), new mir::Call(new std::string("@malloc"), mallocParam, utils::TempType::INT)), new mir::Temp(addrTemp));
                        return new mir::Move(new mir::Temp(pointTemp), mallocSpace);
                    }
                }
            }
            break;
            default: {
                assert(0 && "VarDef semant");
            }
        }
    }
}

mir::Stm* AST::Block::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                    TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                    TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    venv->beginScope(NULL);
    mir::Stm* stm = new mir::ExpStm(new mir::Const(0));
    for (AST::BlockItem*& it : this->items->list) {
        mir::Stm* tmp_stm = it->semanticAnalysis(breakL, continueL, label, venv, fenv);
        stm = seq(stm, tmp_stm);
    }
    venv->endScope();
    return stm;
}

mir::Stm* AST::AssignStmt::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    mir::ExpTy* l = lval->toexptyP(label, venv, fenv); 
    mir::ExpTy* r = exp->toexptyP(label, venv, fenv);
    mir::Exp* lexp = l->exp->unEX();
    mir::Exp* rexp = r->exp->unEX();
    ENTRY::Type_t lty = l->ty->kind;
    ENTRY::Type_t rty = r->ty->kind;

    // 隐式转换
    if (lty == ENTRY::Type_t::Type_FLOAT && rty == ENTRY::Type_t::Type_INT) {
        rexp = mir_i2f(rexp);
    } else if (lty == ENTRY::Type_t::Type_INT && rty == ENTRY::Type_t::Type_FLOAT) {
        rexp = mir_f2i(rexp);
    }
    return new mir::Move(lexp, rexp);
}


mir::Stm* AST::ExpStmt::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    
    mir::ExpStm* stm = new mir::ExpStm(exp->toexptyP(label, venv, fenv)->exp->unEX());
    return stm;
}

mir::Stm* AST::IfStmt::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    mir::CX tmpCX = exp->toexptyP(label, venv, fenv)->exp->unCX();
    utils::Label* trueLa = new utils::Label();
    utils::Label* falseLa = new utils::Label();
    utils::Label* final = new utils::Label();
    doPatch(tmpCX.trues, trueLa);
    doPatch(tmpCX.falses, falseLa);
    mir::Stm* ifstm = if_part->semanticAnalysis(breakL, continueL, label, venv, fenv);
    mir::Stm* elsestm = new mir::ExpStm(new mir::Const(0));
    if (this->else_part) 
        elsestm = else_part->semanticAnalysis(breakL, continueL, label, venv, fenv);
    return seq(tmpCX.stm,
               seq(new mir::Label(trueLa),
                   seq(ifstm, seq(new mir::Jump(final), seq(new mir::Label(falseLa),
                                                          seq(elsestm, new mir::Label(final)))))));
}

mir::Stm* AST::WhileStmt::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    /*
    while (a) {
        b;
    }

    test_label
    a ? true_label : false_label
    true_label:
    b
    jump test_label
    false_label:
    
    repeat:
    a ? begin: final
    begin:
    b
    until:
    a ? begin: final
    final:



    */
    mir::CX tmpCX1 = exp->toexptyP(label, venv, fenv)->exp->unCX();
    mir::CX tmpCX2 = exp->toexptyP(label, venv, fenv)->exp->unCX();
    utils::Label* begin = new utils::Label();
    utils::Label* final = new utils::Label();
    utils::Label* repeat = new utils::Label();
    utils::Label* until = new utils::Label();

    doPatch(tmpCX1.trues, begin);
    doPatch(tmpCX1.falses, final);
    doPatch(tmpCX2.trues, begin);
    doPatch(tmpCX2.falses, final);
    mir::Stm* repeat_partStm = seq(new mir::Label(repeat), tmpCX1.stm);
    mir::Stm* until_partStm = seq(new mir::Label(until), tmpCX2.stm);
    mir::Stm* loopstm
        = seq(new mir::Label(begin), loop->semanticAnalysis(final, until, label, venv, fenv));
    mir::Stm* finalstm = new mir::Label(final);
    return seq(seq(repeat_partStm, seq(loopstm, until_partStm)), finalstm);
}

mir::Stm* AST::BreakStmt::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    return new mir::Jump(breakL);
}

mir::Stm* AST::ContinueStmt::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                    TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                    TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    return new mir::Jump(continueL);
}

mir::Stm* AST::ReturnStmt::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                    TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                    TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    // 可能要assert
    if (exp == NULL) {
        return new mir::Return(new mir::Name(new utils::Label(new std::string(*label->id + "__voidReturn"))));
    }
    else {
        mir::ExpTy* expty = exp->toexptyP(label, venv, fenv);
        mir::Exp* rightExp = expty->exp->unEX();
        ENTRY::Type_t leftT = fenv->find(*label->id)->ty->point->kind;
        ENTRY::Type_t rightT = expty->ty->kind;
        
        // 隐式转换
        if (leftT == ENTRY::Type_t::Type_FLOAT && rightT == ENTRY::Type_t::Type_INT) {
            rightExp = mir_i2f(rightExp);
        } else if (leftT == ENTRY::Type_t::Type_INT && rightT == ENTRY::Type_t::Type_FLOAT) {
            rightExp = mir_f2i(rightExp);
        }

        return new mir::Return(rightExp);
    }
    
}


mir::Stm* AST::StarttimeStmt::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                    TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                    TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {

    return new mir::ExpStm(
        new mir::Call(new std::string("_sysy_starttime"), 
        new mir::ExpList(), 
        utils::TempType::VOID)
    );
}

mir::Stm* AST::StoptimeStmt::semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                    TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                    TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    
    return new mir::ExpStm(
        new mir::Call(new std::string("_sysy_stoptime"), 
        new mir::ExpList(), 
        utils::TempType::VOID)
    );
}


mir::ExpTy* AST::Lval::toexptyP(utils::Label* label,  
                                    TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                    TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {

    // TODO
    std::string ident = *id->str;
    assert(venv->exist(ident) && "undefined ident");
    ENTRY::Entry* entry = venv->find(ident);
    mir::Exp* tmp;
    if (entry->kind == ENTRY::Entry_t::Entry_LOCAL) {
        tmp = new mir::Temp(static_cast<ENTRY::LocVar*>(entry)->temp);
    }
    else if (entry->kind == ENTRY::Entry_t::Entry_GLOBAL) {
        tmp = new mir::Temp(static_cast<ENTRY::GloVar*>(entry)->temp);
    }

    ENTRY::Type* pT = entry->ty;
    if (arrayindex->list.empty()) {         // not array
        if (pT->isconst) {                  // const
            if (pT->kind == ENTRY::Type_t::Type_INT) {
                return new mir::ExpTy(new mir::Tr_Exp(new mir::Const(*pT->Ivalue)), pT);
            }
            else if (pT->kind == ENTRY::Type_t::Type_FLOAT) {
                return new mir::ExpTy(new mir::Tr_Exp(new mir::Const(*pT->Fvalue)), pT);
            }
            else {
                assert(0);
            }
            
        }
        else {                              // not const
            return new mir::ExpTy(new mir::Tr_Exp(tmp), pT);
        }
    }
    else {                                  // array
        int offset = 0;
        mir::Exp* exp = new mir::Const(0);
        
        for (AST::Exp* i : arrayindex->list) {  // 计算数组偏移
            mir::ExpTy* expty = i->toexptyP(label, venv, fenv);
            int dim = pT->dim;
            exp = new mir::Binop(mir::BinOp::PLUS, 
                  new mir::Binop(mir::BinOp::MUL, 
                  new mir::Const(dim), exp), expty->exp->unEX());
            pT = pT->point;
            offset = offset * dim + *expty->ty->Ivalue;
        }
        // TODO: check again
        if (pT->kind == ENTRY::Type_t::Type_ARRAY) {
            ENTRY::Type* lvalTp = pT;
            return new mir::ExpTy(
                new mir::Tr_Exp(new mir::Binop(mir::BinOp::PLUS, tmp,
                                new mir::Binop(mir::BinOp::MUL, 
                                new mir::Const(SIZEOFINT * pT->arraysize), exp))), lvalTp);
        }
        else {
            // assert(0);
            ENTRY::Type* lvalTy;
            utils::TempType tempty;
            if (pT->kind == ENTRY::Type_t::Type_INT) {
                tempty = utils::TempType::INT;
                lvalTy = ENTRY::createIntInfo(
                    new int(entry->ty->Ivalue[(offset % entry->ty->arraysize + entry->ty->arraysize) % entry->ty->arraysize]), false);
            }
            else if (pT->kind == ENTRY::Type_t::Type_FLOAT) {
                tempty = utils::TempType::FLOAT;
                lvalTy = ENTRY::createFloatInfo(
                    new float(entry->ty->Fvalue[(offset % entry->ty->arraysize + entry->ty->arraysize) % entry->ty->arraysize]), false);
            }
            else {
                assert(0);
            }
            return new mir::ExpTy(new mir::Tr_Exp(
                   new mir::Mem(new mir::Binop(mir::BinOp::PLUS, tmp, 
                                new mir::Binop(mir::BinOp::MUL, new mir::Const(SIZEOFINT), exp)), tempty)), lvalTy);
        }   
    }
}

mir::ExpTy* AST::IntNum::toexptyP(utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    mir::Tr_Exp* exp = new mir::Tr_Exp(new mir::Const(value));
    return new mir::ExpTy(exp, ENTRY::createIntInfo(new int(this->value), 0));
}

mir::ExpTy* AST::FloatNum::toexptyP(utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    mir::Tr_Exp* exp = new mir::Tr_Exp(new mir::Const(constval.val.f));
    return new mir::ExpTy(exp, ENTRY::createFloatInfo(new float(constval.val.f), 0));
}

mir::ExpTy* AST::UnaryExp::toexptyP(utils::Label* label,  
                                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    mir::ExpTy* UnaryExpTy = exp->toexptyP(label, venv, fenv);
    switch (op) {
    case unary_op::ADD: {
        return UnaryExpTy;
    }
    case unary_op::SUB: {
        if (UnaryExpTy->exp->ex && UnaryExpTy->exp->ex->kind == mir::ExpType::CONST)  {
            ENTRY::Type* leftT = ENTRY::createIntInfo(new int(0), false);
            ENTRY::Type* rightT = UnaryExpTy->ty;
            ENTRY::Type* ty = binaryOpExpType(mir::BinOp::MINUS, leftT, rightT);
            auto rightc = static_cast<mir::Const*>(UnaryExpTy->exp->unEX());
            if (rightc->type == utils::TempType::INT) {
                mir::Exp* exp = new mir::Const(-rightc->n.i);
                return new mir::ExpTy(new mir::Tr_Exp(exp), ty);
            }
            else {
                mir::Exp* exp = new mir::Const(-rightc->n.f);
                return new mir::ExpTy(new mir::Tr_Exp(exp), ty);
            }
        }
        else {
            ENTRY::Type* leftT = ENTRY::createIntInfo(new int(0), false);
            ENTRY::Type* rightT = UnaryExpTy->ty;
            mir::Exp* lexp = new mir::Const(0);
            mir::Exp* rexp = UnaryExpTy->exp->unEX();
            mir::Exp* exp = binaryMIRexp(mir::BinOp::MINUS, leftT, lexp, rightT, rexp);
            ENTRY::Type* ty = binaryOpExpType(mir::BinOp::MINUS, leftT, rightT);
            return new mir::ExpTy(new mir::Tr_Exp(exp), ty);
        }
        assert(0);
    }
    case unary_op::NOT: {
        mir::CX cx = UnaryExpTy->exp->unCX();
        return new mir::ExpTy(new mir::Tr_Exp(cx.falses, cx.trues, cx.stm), 
                ENTRY::createIntInfo(new int(0), false)); 
    }
    default: assert(0);
    }
}

mir::ExpTy* AST::MulExp::toexptyP(utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {

    mir::ExpTy* leftExpTy = lexp->toexptyP(label, venv, fenv);
    mir::ExpTy* rightExpTy = rexp->toexptyP(label, venv, fenv);
    mir::Exp* leftExp = leftExpTy->exp->unEX();
    mir::Exp* rightExp = rightExpTy->exp->unEX();
    ENTRY::Type* leftType = leftExpTy->ty;
    ENTRY::Type* rightType = rightExpTy->ty;
    mir::BinOp bop;
    AST::const_op cop;
    
    switch (op) {
        case mul_op::MULT: {
            bop = mir::BinOp::MUL; 
            cop = AST::const_op::MUL;
            break;
        }
        case mul_op::DIV: {
            bop = mir::BinOp::DIV; 
            cop = AST::const_op::DIV;
            break;
        }
        case mul_op::REM: {
            bop = mir::BinOp::MOD; 
            cop = AST::const_op::REM;
            break;
        }
        default: assert(0);
    }
    ENTRY::Type* finalType = binaryOpExpType(bop, leftExpTy->ty, rightExpTy->ty);
    // 常数折叠
    mir::Exp* finalExp;
    // if (0) {
    if (leftExp->kind == mir::ExpType::CONST && rightExp->kind == mir::ExpType::CONST) {
        auto leftc = static_cast<mir::Const*>(leftExp);
        auto rightc = static_cast<mir::Const*>(rightExp);
        auto constval = mircalconst(leftc, bop, rightc);
        if (finalType->kind == ENTRY::Type_t::Type_INT) {
            finalExp = new mir::Const(constval.n.i);
        }
        else if (finalType->kind == ENTRY::Type_t::Type_FLOAT) {
            finalExp = new mir::Const(constval.n.f);
        }
        else {
            assert(0);
        }
        
        return new mir::ExpTy(new mir::Tr_Exp(finalExp), finalType);
    }
    else {
        mir::Exp* finalExp = binaryMIRexp(bop, leftType, leftExp, rightType, rightExp);
        return new mir::ExpTy(new mir::Tr_Exp(finalExp), finalType);
    }
    assert(0);
}

mir::ExpTy* AST::AddExp::toexptyP(utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    mir::ExpTy* leftExpTy = lexp->toexptyP(label, venv, fenv);
    mir::ExpTy* rightExpTy = rexp->toexptyP(label, venv, fenv);
    mir::Exp* leftExp = leftExpTy->exp->unEX();
    mir::Exp* rightExp = rightExpTy->exp->unEX();
    ENTRY::Type* leftType = leftExpTy->ty;
    ENTRY::Type* rightType = rightExpTy->ty;
    mir::BinOp bop;
    AST::const_op cop;
    switch (op) {
        case AST::add_op::ADD: {
            bop = mir::BinOp::PLUS; 
            cop = AST::const_op::ADD;
            break;
        }
        case AST::add_op::SUB: {
            bop = mir::BinOp::MINUS; 
            cop = AST::const_op::SUB;
            break;
        }
        default: assert(0);
    }


    // 常数折叠
    ENTRY::Type* finalType = binaryOpExpType(bop, leftExpTy->ty, rightExpTy->ty);
    mir::Exp* finalExp;
    // if (0) {
    if (leftExp->kind == mir::ExpType::CONST && rightExp->kind == mir::ExpType::CONST) {

        auto leftc = static_cast<mir::Const*>(leftExp);
        auto rightc = static_cast<mir::Const*>(rightExp);
        auto constval = mircalconst(leftc, bop, rightc);
        if (finalType->kind == ENTRY::Type_t::Type_INT) {
            finalExp = new mir::Const(constval.n.i);
        }
        else if (finalType->kind == ENTRY::Type_t::Type_FLOAT) {
            finalExp = new mir::Const(constval.n.f);
        }
        else {
            assert(0);
        }
        
        return new mir::ExpTy(new mir::Tr_Exp(finalExp), finalType);
    }
    else {
        mir::Exp* finalExp = binaryMIRexp(bop, leftType, leftExp, rightType, rightExp);
        return new mir::ExpTy(new mir::Tr_Exp(finalExp), finalType);
    }
    assert(0);
}

mir::ExpTy* AST::EqExp::toexptyP(utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    ENTRY::Type* finalType = ENTRY::createIntInfo(new int(0), false);
    mir::ExpTy* leftExpTy = lexp->toexptyP(label, venv, fenv);
    mir::ExpTy* rightExpTy = rexp->toexptyP(label, venv, fenv);
    mir::Exp* leftExp = leftExpTy->exp->unEX();
    mir::Exp* rightExp = rightExpTy->exp->unEX();
    ENTRY::Type_t* leftKind = &leftExpTy->ty->kind;
    ENTRY::Type_t* rightKind = &rightExpTy->ty->kind;
    mir::RelOp bop;
    if (*leftKind == ENTRY::Type_t::Type_FLOAT || *rightKind == ENTRY::Type_t::Type_FLOAT) {
        if (*leftKind == ENTRY::Type_t::Type_INT) leftExp = mir_i2f(leftExp);
        if (*rightKind == ENTRY::Type_t::Type_INT) rightExp = mir_i2f(rightExp);
        switch (op) {
        case AST::equal_op::EQ: bop = mir::RelOp::EQ; break;
        case AST::equal_op::NE: bop = mir::RelOp::NE; break;
        default: assert(0);
        }
    } else {
        switch (op) {
        case AST::equal_op::EQ: bop = mir::RelOp::EQ; break;
        case AST::equal_op::NE: bop = mir::RelOp::NE; break;
        default: assert(0);
        }
    }
    mir::Cjump* cjumpstm = new mir::Cjump(bop, leftExp, rightExp, new utils::Label(new std::string("")), new utils::Label(new std::string("")));
    mir::patchList* truelist = new mir::patchList(cjumpstm->true_label, NULL);
    mir::patchList* falselist = new mir::patchList(cjumpstm->false_label, NULL);
    return new mir::ExpTy(new mir::Tr_Exp(truelist, falselist, cjumpstm), finalType);
}

mir::ExpTy* AST::RelExp::toexptyP(utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    ENTRY::Type* finalType = ENTRY::createIntInfo(new int(0), false);
    mir::ExpTy* leftExpTy = lexp->toexptyP(label, venv, fenv);
    mir::ExpTy* rightExpTy = rexp->toexptyP(label, venv, fenv);
    mir::Exp* leftExp = leftExpTy->exp->unEX();
    mir::Exp* rightExp = rightExpTy->exp->unEX();
    ENTRY::Type_t* leftKind = &leftExpTy->ty->kind;
    ENTRY::Type_t* rightKind = &rightExpTy->ty->kind;
    mir::RelOp bop;
    if (*leftKind == ENTRY::Type_t::Type_FLOAT || *rightKind == ENTRY::Type_t::Type_FLOAT) {
        if (*leftKind == ENTRY::Type_t::Type_INT) leftExp = mir_i2f(leftExp);
        if (*rightKind == ENTRY::Type_t::Type_INT) rightExp = mir_i2f(rightExp);
        switch (op) {
        case AST::rel_op::GE: bop = mir::RelOp::GE; break;
        case AST::rel_op::GT: bop = mir::RelOp::GT; break;
        case AST::rel_op::LE: bop = mir::RelOp::LE; break;
        case AST::rel_op::LT: bop = mir::RelOp::LT; break;
        default: assert(0);
        }
    } else {
        switch (op) {
        case AST::rel_op::GE: bop = mir::RelOp::GE; break;
        case AST::rel_op::GT: bop = mir::RelOp::GT; break;
        case AST::rel_op::LE: bop = mir::RelOp::LE; break;
        case AST::rel_op::LT: bop = mir::RelOp::LT; break;
        default: assert(0);
        }
    }
    mir::Cjump* cjumpstm = new mir::Cjump(bop, leftExp, rightExp, new utils::Label(new std::string("")), new utils::Label(new std::string("")));
    mir::patchList* truelist = new mir::patchList(cjumpstm->true_label, NULL);
    mir::patchList* falselist = new mir::patchList(cjumpstm->false_label, NULL);
    return new mir::ExpTy(new mir::Tr_Exp(truelist, falselist, cjumpstm), finalType);
}

mir::ExpTy* AST::LAndExp::toexptyP(utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    ENTRY::Type* finalType = ENTRY::createIntInfo(new int(0), false);
    mir::ExpTy* leftExpTy = lexp->toexptyP(label, venv, fenv);
    mir::ExpTy* rightExpTy = rexp->toexptyP(label, venv, fenv);
    mir::CX leftCX = leftExpTy->exp->unCX();
    mir::CX rightCX = rightExpTy->exp->unCX();
    utils::Label* tmpLabel = new utils::Label();
    doPatch(leftCX.trues, tmpLabel);
    return new mir::ExpTy(new mir::Tr_Exp(rightCX.trues, joinPatch(leftCX.falses, rightCX.falses),
                        new mir::Seq(leftCX.stm, 
                        new mir::Seq(new mir::Label(tmpLabel), rightCX.stm))), finalType);
}

mir::ExpTy* AST::LOrExp::toexptyP(utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    ENTRY::Type* finalType = ENTRY::createIntInfo(new int(0), false);
    mir::ExpTy* leftExpTy = lexp->toexptyP(label, venv, fenv);
    mir::ExpTy* rightExpTy = rexp->toexptyP(label, venv, fenv);
    mir::CX leftCX = leftExpTy->exp->unCX();
    mir::CX rightCX = rightExpTy->exp->unCX();
    utils::Label* tmpLabel = new utils::Label();
    doPatch(leftCX.falses, tmpLabel);
    return new mir::ExpTy(new mir::Tr_Exp(joinPatch(leftCX.trues, rightCX.trues), rightCX.falses,
                       new mir::Seq(leftCX.stm, 
                       new mir::Seq(new mir::Label(tmpLabel), rightCX.stm))), finalType);
}

mir::ExpTy* AST::CallExp::toexptyP(utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
    assert(fenv->exist(*id->str));
    ENTRY::EnFunction* func = fenv->find(*id->str);

    mir::ExpList list;
    assert(this->params->list.size() == func->ty->param.size());
    int k = 0;
    for (auto i : this->params->list) {
        ENTRY::Type_t leftTy = func->ty->param[k]->kind;
        mir::ExpTy* expty = i->toexptyP(label, venv, fenv);
        mir::Exp* rightExp = expty->exp->unEX();
        ENTRY::Type_t rightTy = expty->ty->kind;
        // 隐式转换
        if (leftTy == ENTRY::Type_t::Type_FLOAT && rightTy == ENTRY::Type_t::Type_INT) {
            rightExp = mir_i2f(rightExp);
        } else if (leftTy == ENTRY::Type_t::Type_INT && rightTy == ENTRY::Type_t::Type_FLOAT) {
            rightExp = mir_f2i(rightExp);
        }
        list.push_back(rightExp);
        k++;
    }

    return new mir::ExpTy(new mir::Tr_Exp(new mir::Call(new std::string(*func->label->id), new mir::ExpList(list), func->returnKind)), func->ty->point);

}

mir::ExpTy* AST::Exp::arrayAssign(ENTRY::Type* ty, mir::Exp* addr, int offset, 
                            utils::Label* label, 
                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {

    // mark
    mir::ExpTy* expty = toexptyP(label, venv, fenv);
    mir::Exp* exp = expty->exp->unEX();
    ENTRY::Type* newType = expty->ty;
    utils::TempType tt;
    if (traversesArrayType(ty) == ENTRY::Type_t::Type_INT) {
        tt = utils::TempType::INT;
    }
    else if (traversesArrayType(ty) == ENTRY::Type_t::Type_FLOAT) {
        tt = utils::TempType::FLOAT;
    }
    else {
        assert(0);
    }
    if (expty->ty->kind == ENTRY::Type_t::Type_FLOAT && traversesArrayType(ty) == ENTRY::Type_t::Type_INT) {
        exp = mir_f2i(exp);
        newType = ENTRY::createIntInfo(new int(f2i(*expty->ty->Fvalue)), false);
    }
    else if (expty->ty->kind == ENTRY::Type_t::Type_INT && traversesArrayType(ty) == ENTRY::Type_t::Type_FLOAT) {
        exp = mir_i2f(exp);
        newType = ENTRY::createFloatInfo(new float(i2f(*expty->ty->Ivalue)), false);

    }
    mir::Stm* newStm = new mir::Move(
                       new mir::Mem(new mir::Binop(mir::BinOp::PLUS, 
                                                addr, new mir::Const(SIZEOFINT * offset)), tt), exp);
    // todo: mem有副作用应该调用eseq
    return new mir::ExpTy(new mir::Tr_Exp(newStm), newType);
}

