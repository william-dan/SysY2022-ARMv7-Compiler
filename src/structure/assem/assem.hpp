#pragma once
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>

#include "../../utils/temp.hpp"

namespace assem {
using Temp = utils::Temp;
using Label = utils::Label;
using TempList = utils::TempList;
using LabelList = utils::LabelList;
using std::string;
enum class AssemType { Oper, Move, Label };
class Instr{
public:
    virtual void print()=0;
    virtual TempList* get_use()=0;
    virtual TempList* get_use(utils::TempType temptype)=0;
    virtual TempList* get_def()=0;
    virtual TempList* get_def(utils::TempType temptype)=0;
    AssemType type;
    string assem;
};
using Instrlist = std::list<Instr*>;
using InstrList = std::list<Instr*>;

class OperInstr: public Instr{
public:
    OperInstr(string assem, TempList* src, TempList* dst, LabelList* jumps)
    :src(src)
    ,dst(dst)
    ,jumps(jumps)
    {
        this->assem = assem;
        this->type = AssemType::Oper;
    }
    void print();
    TempList* get_use(){return src;};
    TempList* get_use(utils::TempType temptype){return utils::TempList_filter(src, temptype);}
    TempList* get_def(){return dst;};
    TempList* get_def(utils::TempType temptype){return utils::TempList_filter(dst, temptype);}

    TempList* src;
    TempList* dst;
    LabelList* jumps;
};

class MoveInstr: public Instr{
public:
    MoveInstr(string assem, TempList* src, TempList* dst)
    :src(src)
    ,dst(dst)
    {
        this->assem = assem;
        this->type = AssemType::Move;
    }
    void print();
    TempList* get_use(){return src;};
    TempList* get_use(utils::TempType temptype){return utils::TempList_filter(src, temptype);}
    TempList* get_def(){return dst;};
    TempList* get_def(utils::TempType temptype){return utils::TempList_filter(dst, temptype);}

    TempList* src;
    TempList* dst;
};

class LabelInstr: public Instr{
public:
    LabelInstr(utils::Label* label)
    :label(label)
    {
        this->assem = *label->id + ":";
        this->type = AssemType::Label;
    }
    void print();
    TempList* get_use(){return new utils::TempList();};
    TempList* get_use(utils::TempType temptype){return new utils::TempList();};
    TempList* get_def(){return new utils::TempList();};
    TempList* get_def(utils::TempType temptype){return new utils::TempList();};


    Label* label;
};

//utils
bool isOperand2(int n);
bool isImm16(int n);
bool isImm12(int n);
std::pair<int,int> get_BigConst(int n);
std::vector<int> get_BigConst_8(int n);
bool is2power(int n);

void emit_ldr_back(assem::Instrlist* instrlist, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype);
void emit_ldr(assem::Instrlist* instrlist, assem::Instrlist::iterator& it, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype);
void emit_str_back(assem::Instrlist* instrlist, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype);
void emit_str(assem::Instrlist* instrlist, assem::Instrlist::iterator& it, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype);
void emit_mov_back(assem::Instrlist* instrlist, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype);
void emit_mov(assem::Instrlist* instrlist, assem::Instrlist::iterator& it, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype);
void clean_instr(InstrList* il);
}// namespace assem