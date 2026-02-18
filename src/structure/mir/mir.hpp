#pragma once
#include "../../utils/entry.hpp"
#include <iostream>
#include <string>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <vector>
#include "../../utils/temp.hpp"
#include "../assem/assem.hpp"







namespace mir {
using std::string;
using std::list;

const int DATA_SIZE = 4; // 32位

enum class BinOp {
    PLUS, MINUS, MUL, DIV, AND, MOD, OR, LSHIFT, RSHIFT, ARSHIFT, XOR
};
void print_BinOp(BinOp op);
enum class RelOp {
    EQ, NE, LT, GT, LE, GE, ULT, ULE, UGT, UGE
};
void print_RelOp(RelOp op);
enum class StmType {
    SEQ, LABEL, JUMP, CJUMP, MOVE,
    EXP, RETURN
};
void print_StmType(StmType t);
enum class ExpType {
    BINOP, MEM, TEMP, ESEQ, NAME,
    CONST, CALL, PHI, FORMAT
};
void print_ExpType(ExpType t);
enum class UnitType {
    FUN, STM
};
void print_UnitType(UnitType t);


// 分成三个大类，fun / stm / exp
class Unit;
class Fun;
class Stm;
class Exp;
class GlobalInfo;

//list
typedef std::list<mir::Unit*> UnitList;
typedef std::list<mir::Fun*> FunList;
typedef std::list<mir::Stm*> StmList;
typedef std::list<mir::Exp*> ExpList;

ExpList ExpList_deepcopy(ExpList& el);

class Unit {
public:
    UnitType kind;
    Unit(){};
    Unit(UnitType kind);
    Unit(Unit& unit);
    virtual void print()=0;
    virtual void ir2assem(assem::Instrlist* instrlist)=0;
};
class Fun: public Unit {
public:
    std::string* name;
    utils::TempList* args;
    mir::Stm* stm;
    Fun(){};
    Fun(std::string* name, utils::TempList* args, Stm* stm);
    Fun(Fun& fun);
    void print();
    void ir2assem(assem::Instrlist* instrlist);
};
class Stm: public Unit {
public:
    StmType kind;
    Stm(){};
    Stm(StmType kind);
    Stm(Stm& stm);
    Stm* stm_deepcopy();
    Stm* stm_deepcopy_with_label(std::unordered_map<utils::Label*, utils::Label*>& label_map);
    virtual Stm* flatten()=0;
    virtual void fix(bool get_label)=0;
    virtual void print()=0;
    virtual void ir2assem(assem::Instrlist* instrlist)=0;
    virtual utils::TempSet get_use()=0;
    virtual utils::TempSet get_def()=0;
    virtual std::vector<Exp**> get_use_ref()=0;
    virtual std::vector<Exp**> get_def_ref()=0;

};
class Exp {
public:
    ExpType kind;
    Exp(){};
    Exp(ExpType kind);
    Exp(Exp& exp);
    Exp* exp_deepcopy();
    virtual utils::TempType get_type()=0;
    virtual void fix(bool get_label)=0;
    virtual std::pair<Exp*,utils::TempType> flatten()=0;
    virtual void print()=0;
    virtual utils::Temp* ir2assem(assem::Instrlist* instrlist)=0;
};



//canon
typedef std::list<std::list<mir::Stm*>*> StmListList;
class Block {
public:
    StmListList* stmLists;
    utils::Label* label;
    Block(){};
    Block(StmListList* stmLists, utils::Label* label);
    Block(Block& block);
    ~Block(){
        delete stmLists;
    }
};
RelOp notRel(RelOp r);
RelOp commuteRel(RelOp r);



//细分stm
class Seq: public Stm {
public:
    Stm *left, *right;
    Seq(){};
    Seq(Stm* left, Stm* right);
    Seq(Seq& seq);
    void fix(bool get_label);
    void print();
    Stm* flatten();
    void ir2assem(assem::Instrlist* instrlist);
    utils::TempSet get_use();
    utils::TempSet get_def();
    std::vector<Exp**> get_use_ref();
    std::vector<Exp**> get_def_ref();
};
class Label: public Stm {
public:
    utils::Label* label;
    Label(){};
    Label(utils::Label* label);
    Label(mir::Label& label);
    void fix(bool get_label);
    void print();
    Stm* flatten();
    void ir2assem(assem::Instrlist* instrlist);
    utils::TempSet get_use();
    utils::TempSet get_def();
    std::vector<Exp**> get_use_ref();
    std::vector<Exp**> get_def_ref();
};
class Jump: public Stm {
public:
    utils::Label* label;
    Jump(){};
    Jump(utils::Label* label);
    Jump(Jump& jump);
    void fix(bool get_label);
    void print();
    Stm* flatten();
    void ir2assem(assem::Instrlist* instrlist);
    utils::TempSet get_use();
    utils::TempSet get_def();
    std::vector<Exp**> get_use_ref();
    std::vector<Exp**> get_def_ref();

};
class Cjump: public Stm {
public:
    RelOp op;
    Exp *left, *right;
    utils::Label *true_label, *false_label;
    Cjump(){};
    Cjump(RelOp op, Exp* left, Exp* right, 
        utils::Label* true_label, utils::Label* false_label);
    Cjump(Cjump& cjump);
    void fix(bool get_label);
    void print();
    Stm* flatten();
    void ir2assem(assem::Instrlist* instrlist);
    utils::TempSet get_use();
    utils::TempSet get_def();
    std::vector<Exp**> get_use_ref();
    std::vector<Exp**> get_def_ref();
};
class Move: public Stm {
public:
    Exp *dst, *src;
    Move(){};
    Move(Exp* left, Exp* right);
    Move(Move& move);
    void fix(bool get_label);
    void print();
    Stm* flatten();
    void ir2assem(assem::Instrlist* instrlist);
    utils::TempSet get_use();
    utils::TempSet get_def();
    std::vector<Exp**> get_use_ref();
    std::vector<Exp**> get_def_ref();
};
class ExpStm: public Stm {
public:
    Exp* exp;
    ExpStm(){};
    ExpStm(Exp* exp);
    ExpStm(ExpStm& expstm);
    void fix(bool get_label);
    void print();
    Stm* flatten();
    void ir2assem(assem::Instrlist* instrlist);
    utils::TempSet get_use();
    utils::TempSet get_def();
    std::vector<Exp**> get_use_ref();
    std::vector<Exp**> get_def_ref();
};
class Return: public Stm {
public:
    Exp* exp;
    Return(){};
    Return(Exp* exp);
    Return(Return& ret);
    void fix(bool get_label);
    void print();
    Stm* flatten();
    void ir2assem(assem::Instrlist* instrlist);
    utils::TempSet get_use();
    utils::TempSet get_def();
    std::vector<Exp**> get_use_ref();
    std::vector<Exp**> get_def_ref();
};


//细分Exp
class Binop: public Exp {
public:
    BinOp op;
    Exp *left, *right;
    utils::TempType type;
    Binop(){};
    Binop(BinOp op, Exp* left, Exp* right);
    Binop(Binop& binop);
    utils::TempType get_type(){return type;};
    void fix(bool get_label);
    void print();
    std::pair<Exp*,utils::TempType> flatten();
    utils::Temp* ir2assem(assem::Instrlist* instrlist);

};
class Mem: public Exp {
public:
    Exp* exp;
    utils::TempType type;
    Mem(){};
    Mem(Exp* exp, utils::TempType type);
    Mem(Mem& mem);
    utils::TempType get_type(){return type;};
    void fix(bool get_label);
    void print();
    std::pair<Exp*,utils::TempType> flatten();
    utils::Temp* ir2assem(assem::Instrlist* instrlist);
};
class Temp: public Exp {
public:
    utils::Temp* temp;
    utils::TempType type;
    Temp(){};
    Temp(utils::Temp* temp);
    Temp(mir::Temp& temp);
    utils::TempType get_type(){return type;};
    void fix(bool get_label);
    void print();
    std::pair<Exp*,utils::TempType> flatten();
    utils::Temp* ir2assem(assem::Instrlist* instrlist);
};
class Eseq: public Exp {
public:
    Stm* stm;
    Exp* exp;
    utils::TempType type;
    Eseq(){};
    Eseq(Stm* stm, Exp* exp);
    Eseq(Eseq& eseq);
    utils::TempType get_type(){return type;};
    void fix(bool get_label);
    void print();
    std::pair<Exp*,utils::TempType> flatten();
    utils::Temp* ir2assem(assem::Instrlist* instrlist);
};
class Name: public Exp {
public:
    utils::Label* label;
    utils::TempType type;
    Name(){};
    Name(utils::Label* label);
    Name(Name& name);
    utils::TempType get_type(){return utils::TempType::INT;};
    void fix(bool get_label);
    void print();
    std::pair<Exp*,utils::TempType> flatten();
    utils::Temp* ir2assem(assem::Instrlist* instrlist);
};
class Format: public Exp {
public:
    string* str;
    utils::TempType type;
    Format(){};
    Format(string* str);
    Format(Format& format);
    utils::TempType get_type(){return utils::TempType::INT;};
    void fix(bool get_label);
    void print();
    std::pair<Exp*,utils::TempType> flatten();
    utils::Temp* ir2assem(assem::Instrlist* instrlist);
};
class Const: public Exp {
public:
    union {
        int i;
        float f;
    } n;
    utils::TempType type;
    Const(){};
    Const(int i);
    Const(float f);
    Const(Const& c);
    string to_string();
    bool isOperand2();
    bool isImm16();
    std::pair<int,int> get_BigConst();
    utils::TempType get_type(){return type;};
    void fix(bool get_label);
    void print();
    std::pair<Exp*,utils::TempType> flatten();
    utils::Temp* ir2assem(assem::Instrlist* instrlist);
};
class Call: public Exp {
public:
    string* name;
    ExpList* args;
    utils::TempType type;
    Call(){};
    Call(string* name, ExpList* elist, utils::TempType type);
    Call(Call& call);
    utils::TempType get_type(){return type;};
    void fix(bool get_label);
    void print();
    std::pair<Exp*,utils::TempType> flatten();
    utils::Temp* ir2assem(assem::Instrlist* instrlist);
};
class Phi: public Exp {
public:
    utils::TempList* tl;
    utils::LabelList* ll;
    utils::TempType type;
    std::unordered_map<utils::Temp*, mir::Const*> temp_const_map;
    Phi(){};
    Phi(utils::TempList* tl, utils::LabelList* ll);
    Phi(Phi& phi);
    utils::TempType get_type(){return type;}
    void fix(bool get_label);
    void print();
    std::pair<Exp*,utils::TempType> flatten();
    utils::Temp* ir2assem(assem::Instrlist* instrlist);
};
std::string relop2str(RelOp op);
std::string binop2str(BinOp op);
std::string f_binop2str(BinOp op);
assem::Instrlist* ir2assem(mir::Fun* fun, mir::StmList* slist);
void get_global_table(mir::UnitList* ul);
assem::Instrlist* assem_add_epilog(assem::InstrList* instrlist);

// assem::Instrlist* ir2llvm(mir::Fun* fun, mir::StmList* slist);

enum class GlobalInfoType{
    FORMAT, CONST, ARRAY
};
typedef std::vector<mir::Const*> ConstList;
typedef std::unordered_set<mir::Const*> ConstSet;
typedef std::map<int, mir::Const*> ConstMap;
class GlobalInfo {
public:
    static std::unordered_map<utils::Temp*, mir::GlobalInfo*> global_table;
    static void dump_global_table();
    utils::Label* label;
    union{
        string* format;
        ConstMap* clist;
        mir::Const* c;
    } u;
    GlobalInfoType type;
    bool is_const, init_zero;
    GlobalInfo(){};
    GlobalInfo(utils::Label* label, string* format);
    GlobalInfo(utils::Label* label, ConstMap* clist);
    GlobalInfo(utils::Label* label, mir::Const* c);
};


// 设计Tr_exp，解耦合semant和translate模块
// ex 代表有返回值的 exp
// nx 代表无返回值的 stm
// cx 代表条件语句  {2 个 patchList, 1 个 stm}


// Tr_Ex, Tr_Nx, Tr_Cx 将 Tr_exp 转换为 exp, stm, cx


// patchList 是一条链
// patchList = {head, tail}


// expty

enum class Tr_t { EX, NX, CX };

struct patchList {
    utils::Label* head;
    patchList* tail;
    patchList(utils::Label* _head, patchList* _tail) {
        head = _head;
        tail = _tail;
    }
};

struct CX {
    patchList* trues;
    patchList* falses;
    Stm* stm;
    CX(patchList* _trues, patchList* _falses, Stm* _stm) {
        trues = _trues;
        falses = _falses;
        stm = _stm;
    }
    CX() {}
};

struct Tr_Exp {
    Tr_t kind;
    Exp* ex;
    Stm* nx;
    CX cx;
    Tr_Exp(Stm* stm);
    Tr_Exp(patchList* trues, patchList* falses, Stm* stm);
    Tr_Exp(Exp* exp);
    Exp* unEX();
    CX unCX();
    Stm* unNX();
};

class ExpTy {
public:
    Tr_Exp* exp;
    ENTRY::Type* ty;
    ExpTy(Tr_Exp* _exp, ENTRY::Type* _ty) {
        exp = _exp;
        ty = _ty;
    }
};


};


