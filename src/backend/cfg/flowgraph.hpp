#pragma once
#include <iostream>
#include <stack>
#include <set>
#include "graph.hpp"
#include "../../utils/temp.hpp"
#include "../../structure/assem/assem.hpp"


namespace cfg {
using utils::TempSet;
using assem::Instr, assem::InstrList;
using namespace std;



enum class ITType {
    COMMON, JUMP, MOVE
};

class FlowGraph;
class FGinfo;


class FlowGraph: public Graph{
public:
    unordered_map<utils::Label*, Node*> labelTable;

    FlowGraph();
    FlowGraph(InstrList* il, utils::TempType temptype);
    ~FlowGraph();
    TempSet* def(Node* n);
    TempSet* use(Node* n);
    bool isMove(Node* n);
    void show();

    void Liveness();
    void showLiveness();
    TempSet* FG_Out(Node* n);
    TempSet* FG_In(Node* n);
    
};
class FGinfo {
public:
    Instr* instr;
    TempSet *uses, *defs;
    TempSet *in, *out;
    bool isMove;
    FGinfo(){};
    FGinfo(Instr* instr, TempSet* uses, TempSet* defs, bool isMove);
    ~FGinfo(){
        delete uses;
        delete defs;
        delete in;
        delete out;
    }
};



}
