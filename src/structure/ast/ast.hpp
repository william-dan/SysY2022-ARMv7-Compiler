#ifndef __AST_HPP
#define __AST_HPP

#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

#include "../../utils/utils.hpp"
#include "../../utils/entry.hpp"
#include "../../utils/table.hpp"
#include "../../utils/temp.hpp"


namespace AST {


enum class equal_op { EQ, NE };
enum class rel_op { LE, LT, GE, GT };
enum class type { VOID = 0, INT, FLOAT };
enum class mul_op { MULT, DIV, REM };
enum class add_op { ADD, SUB };
enum class const_op { ADD, SUB, MUL, DIV, REM, NOT };
enum class unary_op { ADD, SUB, NOT };


void printBlock(int depth);

//  ComUnit
struct CompUnits;
struct CompUnit;
//  Declaration
struct Decl;
struct ConstDecl;
struct VarDecl;
//  Definition
struct ConstDefs;
struct ConstDef;
struct VarDefs;
struct VarDef;
//  Initial
struct Arr_Index;
struct InitVals;
struct InitVal;
struct Arr_Init;
//  Functions
struct FuncDef;
struct Parameters;
struct Parameter;
//  Block
struct Block;
struct BlockItems;
struct BlockItem;
//  Statement
struct Stmt;
struct AssignStmt;
struct ExpStmt;
struct IfStmt;
struct WhileStmt;
struct BreakStmt;
struct ContinueStmt;
struct ReturnStmt;
//  Lval
struct Lval;
//  Express
struct Exps;
struct Exp;
struct IdExp;
struct PrimaryExp;
struct UnaryExp;
struct CallExp;
struct MulExp;
struct AddExp;
struct RelExp;
struct EqExp;
struct LAndExp;
struct LOrExp;
struct FormatExp;
//  Number
struct Number;
struct IntNum;
struct FloatNum;



struct ConstVal {
    bool is_const;
    bool is_int;
    bool can_cal;
    
    union {
        int i;
        float f;
    } val;

    ConstVal()
        : is_const(false) {}
    ConstVal(int i)
        : is_const(true)
        , is_int(true)
        , can_cal(true) {
        val.i = i;
    }
    ConstVal(float f)
        : is_const(true)
        , is_int(false)
        , can_cal(true) {
        val.f = f;
    }
    ConstVal(void* x)
        : is_const(true)
        , can_cal(false) {}
};

static ConstVal calconst(const ConstVal& lexp, const_op op, const ConstVal& rexp) {
    if (!lexp.is_const || !rexp.is_const) { return ConstVal(); }

    if (!lexp.can_cal || !rexp.can_cal) { return ConstVal(nullptr); }

    switch (op) {
    case const_op::ADD:
        if (lexp.is_int && rexp.is_int) {return ConstVal(lexp.val.i + rexp.val.i);}
        if (lexp.is_int && !rexp.is_int) return ConstVal(lexp.val.i + rexp.val.f);
        if (!lexp.is_int && rexp.is_int) return ConstVal(lexp.val.f + rexp.val.i);
        if (!lexp.is_int && !rexp.is_int) return ConstVal(lexp.val.f + rexp.val.f);
        break;
    case const_op::SUB:
        if (lexp.is_int && rexp.is_int) return ConstVal(lexp.val.i - rexp.val.i);
        if (lexp.is_int && !rexp.is_int) return ConstVal(lexp.val.i - rexp.val.f);
        if (!lexp.is_int && rexp.is_int) return ConstVal(lexp.val.f - rexp.val.i);
        if (!lexp.is_int && !rexp.is_int) return ConstVal(lexp.val.f - rexp.val.f);
        break;
    case const_op::MUL:
        if (lexp.is_int && rexp.is_int) return ConstVal(lexp.val.i * rexp.val.i);
        if (lexp.is_int && !rexp.is_int) return ConstVal(lexp.val.i * rexp.val.f);
        if (!lexp.is_int && rexp.is_int) return ConstVal(lexp.val.f * rexp.val.i);
        if (!lexp.is_int && !rexp.is_int) return ConstVal(lexp.val.f * rexp.val.f);
        break;
    case const_op::DIV:
        if (lexp.is_int && rexp.is_int) return ConstVal(lexp.val.i / rexp.val.i);
        if (lexp.is_int && !rexp.is_int) return ConstVal(lexp.val.i / rexp.val.f);
        if (!lexp.is_int && rexp.is_int) return ConstVal(lexp.val.f / rexp.val.i);
        if (!lexp.is_int && !rexp.is_int) return ConstVal(lexp.val.f / rexp.val.f);
        break;
    case const_op::REM:
        if (lexp.is_int && rexp.is_int) return ConstVal(lexp.val.i % rexp.val.i);
        break;
    case const_op::NOT:
        if (rexp.is_int) return ConstVal(int(!lexp.val.i));
        break;
    default:;
    }
    return ConstVal();
}


template <typename T> struct PotentialNullList {
    std::vector<T> list;
    int lineno;

    PotentialNullList() {}
    PotentialNullList(int _lineno)
        : lineno(_lineno) {}
    PotentialNullList(T x, int _lineno)
        : lineno(_lineno) {
        list.push_back(x);
    }
};


struct CompUnits {
    std::vector<CompUnit*> list;
    int lineno;

    CompUnits(int _lineno)
        : lineno(_lineno) {
        list.reserve(1);
    }

    // print
    std::string name = "CompUnits";
    void printTree(int depth);

    // semantic analysis
    mir::UnitList* semanticAnalysis(TABLE::SymbolTable<ENTRY::Entry*>* venv,
                          TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};


struct CompUnit {
    std::string name = "CompUnit";
    virtual void printTree(int depth) {
        assert(0);
    }
    virtual mir::Unit* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
        assert(0);
    }
};

struct BlockItem {
    std::string name = "BlockItem";
    virtual void printTree(int depth) {
        assert(0);
    }
    virtual mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
        assert(0);
    }
};

struct Decl : CompUnit, BlockItem {
    std::string name = "Decl";
    virtual void printTree(int depth) {
        assert(0);
    }
    virtual mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
        assert(0);
    }
};

struct ConstDecl : Decl {
    type t;
    ConstDefs* defs;
    int lineno;
    ConstDecl(type _t, ConstDefs* _defs, int _lineno)
        : t(_t)
        , defs(_defs)
        , lineno(_lineno) {}
    // print
    std::string name = "ConstDecl";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct ConstDefs : public PotentialNullList<ConstDef*> {
    ConstDefs()
        : PotentialNullList() {}
    ConstDefs(int _lineno)
        : PotentialNullList(_lineno) {}
    ConstDefs(ConstDef* x, int _lineno)
        : PotentialNullList(x, _lineno) {}
    // print
    std::string name = "ConstDefs";
    void printTree(int depth);
};

struct ConstDef {
    IdExp* id;
    Arr_Index* index_list;
    InitVal* initval;
    int lineno;

    ConstDef(IdExp* _id, Arr_Index* _index_list, InitVal* _val, int _lineno)
        : id(_id)
        , index_list(_index_list)
        , initval(_val)
        , lineno(_lineno) {}
    // print
    std::string name = "ConstDef";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(type t, utils::Label* label, 
                               TABLE::SymbolTable<ENTRY::Entry*>* venv,
                               TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};


struct VarDecl : public Decl {
    type t;
    VarDefs* defs;
    int lineno;

    VarDecl(type _t, VarDefs* _defs, int _lineno)
        : t(_t)
        , defs(_defs)
        , lineno(_lineno) {}
    //print
    std::string name = "VarDecl";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,
                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct VarDefs : public PotentialNullList<VarDef*> {
    VarDefs()
        : PotentialNullList() {}
    VarDefs(int _lineno)
        : PotentialNullList(_lineno) {}
    VarDefs(VarDef* x, int _lineno)
        : PotentialNullList(x, _lineno) {}
    //print
    std::string name = "VarDefs";
    void printTree(int depth);

};

struct VarDef {
    IdExp* id;
    Arr_Index* index_list;
    InitVal* initval;
    int lineno;

    VarDef(IdExp* _id, Arr_Index* _index_list, InitVal* _initval, int _lineno)
        : id(_id)
        , index_list(_index_list)
        , initval(_initval)
        , lineno(_lineno) {}
    // print
    std::string name = "VarDef";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(type t, utils::Label* label,  
                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct Arr_Index : PotentialNullList<Exp*> {
    Arr_Index()
        : PotentialNullList() {}
    Arr_Index(int _lineno)
        : PotentialNullList(_lineno) {}
    Arr_Index(Exp* x, int _lineno)
        : PotentialNullList(x, _lineno) {}
    // print
    std::string name = "Arr_Index";
    void printTree(int depth);

};

struct InitVal {
    // print
    std::string name = "InitVal";
    virtual void printTree(int depth) {
        printBlock(depth);
        std::cout << name << "\n";
    }

    // semantic analysis 
    virtual mir::ExpTy* toexptyP(utils::Label* label, 
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
        assert(0);
    }
    virtual mir::ExpTy* arrayAssign(ENTRY::Type* ty, mir::Exp* addr, int offset, 
                                    utils::Label* label, 
                                    TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                    TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
        assert(0);
    }
};

struct Arr_Init : public InitVal {
    // print
    std::string name = "Arr_Init";
    virtual void printTree(int depth) {
        printBlock(depth);
        std::cout << name << "\n";
    }

    // semantic analysis 
    virtual mir::ExpTy* toexptyP(TABLE::SymbolTable<ENTRY::Entry*>* venv,
                          TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
        assert(0);
    }
    virtual mir::ExpTy* arrayAssign(ENTRY::Type* ty, mir::Exp* addr, int offset, 
                                utils::Label* label, 
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
        assert(0);
    }
};

struct InitVals : public Arr_Init, PotentialNullList<InitVal*> {
    InitVals()
        : PotentialNullList() {}
    InitVals(int _lineno)
        : PotentialNullList(_lineno) {}
    InitVals(InitVal* x, int _lineno)
        : PotentialNullList(x, _lineno) {}
    // print
    std::string name = "InitVals";
    virtual void printTree(int depth) {
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

    mir::ExpTy* arrayAssign(ENTRY::Type* ty, mir::Exp* addr, int offset, 
                            utils::Label* label, 
                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct FuncDef : public CompUnit {
    type t;
    IdExp* id;
    Parameters* parameters;
    Block* block;
    int lineno;

    FuncDef(type _t, IdExp* _id, Parameters* _parameters, Block* _block, int _lineno)
        : t(_t)
        , id(_id)
        , parameters(_parameters)
        , block(_block)
        , lineno(_lineno) {}
    // print
    std::string name = "FuncDef";
    void printTree(int depth);

    // semantic analysis 
    mir::Fun* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct Parameters : public PotentialNullList<Parameter*> {
    Parameters()
        : PotentialNullList() {}
    Parameters(int _lineno)
        : PotentialNullList(_lineno) {}
    Parameters(Parameter* x, int _lineno)
        : PotentialNullList(x, _lineno) {}

    std::string name = "Parameters";
    void printTree(int depth);

};

struct Parameter {
    type t;
    IdExp* id;
    Arr_Index* arrayindex;
    int lineno;

    Parameter(type _t, IdExp* _id, Arr_Index* _arrayindex, int _lineno)
        : t(_t)
        , id(_id)
        , arrayindex(_arrayindex)
        , lineno(_lineno) {}
    
    // print
    std::string name = "Parameter";
    void printTree(int depth);

};

struct Stmt : public BlockItem {
    // print
    std::string name = "Stmt";
    virtual void printTree(int depth) {
        printBlock(depth);
        std::cout << name << "\n";
    }

    // semantic analysis 
    virtual mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv){
        assert(0);
    }
};

struct BlockItems : public PotentialNullList<BlockItem*> {
    BlockItems()
        : PotentialNullList() {}
    BlockItems(int _lineno)
        : PotentialNullList(_lineno) {}
    BlockItems(BlockItem* x, int _lineno)
        : PotentialNullList(x, _lineno) {}
        std::string name = "BlockItems";
    void printTree(int depth);
};

struct Block : public Stmt {
    BlockItems* items;
    int lineno;

    Block(BlockItems* _items, int _lineno)
        : items(_items)
        , lineno(_lineno) {}
    // print
    std::string name = "Block";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct AssignStmt : public Stmt {
    Lval* lval;
    Exp* exp;
    int lineno;

    AssignStmt(Lval* _lval, Exp* _exp, int _lineno)
        : lval(_lval)
        , exp(_exp)
        , lineno(_lineno) {}
    // print
    std::string name = "AssignStmt";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct ExpStmt : public Stmt {
    Exp* exp;
    int lineno;

    ExpStmt(Exp* _exp, int _lineno)
        : exp(_exp)
        , lineno(_lineno) {}
    // print
    std::string name = "ExpStmt";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct IfStmt : public Stmt {
    Exp* exp;
    Stmt* if_part;
    Stmt* else_part;
    int lineno;

    IfStmt(Exp* _exp, Stmt* _if_part, Stmt* _else_part, int _lineno)
        : exp(_exp)
        , if_part(_if_part)
        , else_part(_else_part)
        , lineno(_lineno) {}
    // print
    std::string name = "IfStmt";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct WhileStmt : public Stmt {
    Exp* exp;
    Stmt* loop;
    int lineno;

    WhileStmt(Exp* _exp, Stmt* _loop, int _lineno)
        : exp(_exp)
        , loop(_loop)
        , lineno(_lineno) {}
    // print
    std::string name = "WhileStmt";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct BreakStmt : public Stmt {
    int lineno;

    BreakStmt(int _lineno)
        : lineno(_lineno) {}
    // print
    std::string name = "BreakStmt";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                    TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                    TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct ContinueStmt : public Stmt {
    int lineno;

    ContinueStmt(int _lineno)
        : lineno(_lineno) {}
    // print
    std::string name = "ContinueStmt";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct ReturnStmt : public Stmt {
    Exp* exp;
    int lineno;

    ReturnStmt(Exp* _exp, int _lineno)
        : exp(_exp)
        , lineno(_lineno) {}
    // print
    std::string name = "ReturnStmt";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct Exp : InitVal {
    ConstVal constval;

    bool ast_isconst() { return constval.is_const; }
    // print
    std::string name = "Exp";
    virtual void printTree(int depth) {
        printBlock(depth);
        std::cout << name << "\n";
    }

    virtual mir::ExpTy* toexptyP(utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {
        assert(0);
    }

    mir::ExpTy* arrayAssign(ENTRY::Type* ty, mir::Exp* addr, int offset, 
                            utils::Label* label, 
                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};


struct FormatExp : public Exp {
    std::string* str;
    int lineno;

    FormatExp(std::string* _str, int _lineno)
        : str(_str)
        , lineno(_lineno) {}
    // print
    std::string name = "FormatExp";
    void printTree(int depth);

};


struct IdExp : public Exp {
    std::string* str;
    int lineno;

    IdExp(std::string* _str, int _lineno)
        : str(_str)
        , lineno(_lineno) {}
    // print
    std::string name = "IdExp";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(TABLE::SymbolTable<ENTRY::Entry*>* venv,
                          TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct PrimaryExp : public Exp {
    // print
    std::string name = "PrimaryExp";
    virtual void printTree(int depth) {
        printBlock(depth);
        std::cout << name << "\n";
    }

    // semantic analysis 
    virtual mir::ExpTy* toexptyP(utils::Label* label,  
                                    TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                    TABLE::SymbolTable<ENTRY::EnFunction*>* fenv){

        assert(0);
    }
};

struct Lval : public PrimaryExp {
    IdExp* id;
    Arr_Index* arrayindex;
    int lineno;

    Lval(IdExp* _id, Arr_Index* _arrayindex, int _lineno)
        : id(_id)
        , arrayindex(_arrayindex)
        , lineno(_lineno) {
        constval = ConstVal(nullptr);
    }
    // print
    std::string name = "Lval";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct Number : public PrimaryExp {
    // print
    std::string name = "Number";
    virtual void printTree(int depth) {
        printBlock(depth);
        std::cout << name << "\n";
    }

    // semantic analysis 
    virtual mir::ExpTy* toexptyP(utils::Label* label, 
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv) {

        assert(0);
    }
};

struct IntNum : Number {
    int value;
    int lineno;

    IntNum(int _value, int _lineno)
        : value(_value)
        , lineno(_lineno) {
        constval = ConstVal(value);
    }
    // print
    std::string name = "IntNum";
    void printTree(int depth);
    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label, 
                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct FloatNum : Number {
    std::string* str;
    int lineno;

    FloatNum(std::string* _str, int _lineno)
        : str(_str)
        , lineno(_lineno) {
        constval = ConstVal(std::stof(*str));
    }
    // print
    std::string name = "FloatNum";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label, 
                            TABLE::SymbolTable<ENTRY::Entry*>* venv,
                            TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct UnaryExp : public Exp {
    unary_op op;
    Exp* exp;
    int lineno;

    UnaryExp(unary_op _op, Exp* _exp, int _lineno)
        : op(_op)
        , exp(_exp)
        , lineno(_lineno) {
        if (exp->ast_isconst()) {
            switch (op) {
            case unary_op::ADD:
                constval = calconst(ConstVal(0), const_op::ADD, exp->constval);
                break;
            case unary_op::SUB:
                constval = calconst(ConstVal(0), const_op::SUB, exp->constval);
                break;
            case unary_op::NOT:
                constval = calconst(ConstVal(0), const_op::NOT, exp->constval);
                break;
            default:;
            }
        }
    }
    // print
    std::string name = "UnaryExp";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct CallExp : public Exp {
    IdExp* id;
    Exps* params;
    int lineno;

    CallExp(IdExp* _id, Exps* _params, int _lineno)
        : id(_id)
        , params(_params)
        , lineno(_lineno) {}
    // print
    std::string name = "CallExp";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label,  
                                        TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                        TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct Exps : public PotentialNullList<Exp*> {
    Exps()
        : PotentialNullList() {}
    Exps(int _lineno)
        : PotentialNullList(_lineno) {}
    Exps(Exp* x, int _lineno)
        : PotentialNullList(x, _lineno) {}
    // print
    std::string name = "Exps";
    void printTree(int depth);

    // semantic analysis 
    void semanticAnalysis(TABLE::SymbolTable<ENTRY::Entry*>* venv,
                          TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct MulExp : public Exp {
    Exp* lexp;
    mul_op op;
    Exp* rexp;
    int lineno;

    MulExp(Exp* _lexp, mul_op _op, Exp* _rexp, int _lineno)
        : lexp(_lexp)
        , op(_op)
        , rexp(_rexp)
        , lineno(_lineno) {
        if (lexp->ast_isconst() && rexp->ast_isconst()) {
            switch (op) {
            case mul_op::MULT: constval = calconst(lexp->constval, const_op::MUL, rexp->constval); break;
            case mul_op::DIV: constval = calconst(lexp->constval, const_op::DIV, rexp->constval); break;
            case mul_op::REM: constval = calconst(lexp->constval, const_op::REM, rexp->constval); break;

            default:;
            }
        }
    }
    // print
    std::string name = "MulExp";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct AddExp : public Exp {
    Exp* lexp;
    add_op op;
    Exp* rexp;
    int lineno;

    AddExp(Exp* _lexp, add_op _op, Exp* _rexp, int _lineno)
        : lexp(_lexp)
        , op(_op)
        , rexp(_rexp)
        , lineno(_lineno) {
        if (lexp->ast_isconst() && rexp->ast_isconst()) {
            switch (op) {
            case add_op::ADD: {constval = calconst(lexp->constval, const_op::ADD, rexp->constval);break;}
            case add_op::SUB: constval = calconst(lexp->constval, const_op::SUB, rexp->constval); break;
            default:;
            }
        }
    }
    // print
    std::string name = "AddExp";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct RelExp : public Exp {
    Exp* lexp;
    rel_op op;
    Exp* rexp;
    int lineno;

    RelExp(Exp* _lexp, rel_op _op, Exp* _rexp, int _lineno)
        : lexp(_lexp)
        , op(_op)
        , rexp(_rexp)
        , lineno(_lineno) {}
    // print
    std::string name = "RelExp";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct EqExp : public Exp {
    Exp* lexp;
    equal_op op;
    Exp* rexp;
    int lineno;

    EqExp(Exp* _lexp, equal_op _op, Exp* _rexp, int _lineno)
        : lexp(_lexp)
        , op(_op)
        , rexp(_rexp)
        , lineno(_lineno) {}
    // print
    std::string name = "EqExp";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct LAndExp : public Exp {
    Exp* lexp;
    Exp* rexp;
    int lineno;

    LAndExp(Exp* _lexp, Exp* _rexp, int _lineno)
        : lexp(_lexp)
        , rexp(_rexp)
        , lineno(_lineno) {}
    // print
    std::string name = "LAndExp";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct LOrExp : public Exp {
    Exp* lexp;
    Exp* rexp;
    int lineno;

    LOrExp(Exp* _lexp, Exp* _rexp, int _lineno)
        : lexp(_lexp)
        , rexp(_rexp)
        , lineno(_lineno) {}
    // print
    std::string name = "LOrExp";
    void printTree(int depth);

    // semantic analysis 
    mir::ExpTy* toexptyP(utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct StarttimeStmt : public Stmt {
    int lineno;

    StarttimeStmt(int _lineno)
        : lineno(_lineno) {}
    // print
    std::string name = "StarttimeStmt";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

struct StoptimeStmt : public Stmt {
    int lineno;

    StoptimeStmt(int _lineno)
        : lineno(_lineno) {}
    // print
    std::string name = "StoptimeStmt";
    void printTree(int depth);

    // semantic analysis 
    mir::Stm* semanticAnalysis(utils::Label* breakL, utils::Label* continueL, utils::Label* label,  
                                TABLE::SymbolTable<ENTRY::Entry*>* venv,
                                TABLE::SymbolTable<ENTRY::EnFunction*>* fenv);
};

};  // namespace AST

#endif // __AST_H
