#include "mir.hpp"
#include <string>
#include <cassert>

namespace mir{
using namespace std;

void GlobalInfo::dump_global_table(){
    for(auto& kv: global_table){
        cout << "temp: " << kv.first->to_string() << endl;
        cout << "global info: " << kv.second->label->to_string() << endl;
    }
}
void print_BinOp(BinOp op){
    switch (op)
    {
    case BinOp::PLUS:
        cout << "+";
    break;
    case BinOp::MINUS:
        cout << "-";
    break;
    case BinOp::MUL:
        cout << "*";
    break;
    case BinOp::DIV:
        cout << "/";
    break;
    case BinOp::AND:
        cout << "&&";
    break;
    case BinOp::OR:
        cout << "||";
    break;
    case BinOp::MOD:
        cout << "%";
    break;
    case BinOp::LSHIFT:
        cout << "<<";
    break;
    case BinOp::RSHIFT:
        cout << ">>";
    break;
    case BinOp::ARSHIFT:
        cout << ">>>";
    break;
    case BinOp::XOR:
        cout << "~";
    break;
    default:
        assert(0);
    break;
    }
}
void print_RelOp(RelOp op){
    switch (op)
    {
    case RelOp::EQ:
        cout << "==";
    break;
    case RelOp::NE:
        cout << "!=";
    break;
    case RelOp::LT:
        cout << "<";
    break;
    case RelOp::GT:
        cout << ">";
    break;
    case RelOp::LE:
        cout << "<=";
    break;
    case RelOp::GE:
        cout << ">=";
    break;
    case RelOp::ULT:
        cout << "<";
    break;
    case RelOp::ULE:
        cout << "<=";
    break;
    case RelOp::UGT:
        cout << ">";
    break;
    case RelOp::UGE:
        cout << ">=";
    break;
    default:
        assert(0);
    break;
    }
}
void print_StmType(StmType t){
    switch (t)
    {
    case StmType::SEQ:
       cout << "SEQ";
    break;
    case StmType::LABEL:
        cout << "LABEL";
    break;
    case StmType::JUMP:
        cout << "JUMP";
    break;
    case StmType::CJUMP:
        cout << "CJUMP";
    break;
    case StmType::MOVE:
        cout << "MOVE";
    break;
    case StmType::EXP:
        cout << "EXP";
    break;
    case StmType::RETURN:
        cout << "RETURN";
    break;
    default:
        assert(0);
    break;
    }
}
void print_ExpType(ExpType t){
    switch (t)
    {
    case ExpType::BINOP:
        cout << "BINOP";
    break;
    case ExpType::MEM:
        cout << "MEM";
    break;
    case ExpType::TEMP:
        cout << "TEMP";
    break;
    case ExpType::ESEQ:
        cout << "ESEQ";
    break;
    case ExpType::NAME:
        cout << "NAME";
    break;
    case ExpType::CONST:
        cout << "CONST";
    break;
    case ExpType::CALL:
        cout << "CALL";
    break;
    case ExpType::PHI:
        cout << "PHI";
    break;
    default:
        assert(0);
    break;
    }
}
void print_UnitType(UnitType t){
    switch (t)
    {
    case UnitType::FUN:
        cout << "FUN";
    break;
    case UnitType::STM:
        cout << "STM";
    break;
    default:
        assert(0);
    break;
    }

}




void Fun::print(){
    cout << "function: " << *name << endl;
    if(args){
        for(auto& arg: *args){
            cout << *(arg->id) << endl;
        }
    }
    assert(stm);
    stm->print();
}
void Seq::print(){
    assert(left && right);
    cout << "seq(" << endl;
    left->print();
    right->print();
    cout << ")" << endl;
}
void Label::print(){
    assert(label);
    cout << "label(" << *label->id << "):" << endl;
}
void Jump::print(){
    assert(label);
    cout << "jump " << *label->id << endl;
}
void Cjump::print(){
    assert(left && right && true_label && false_label);
    left->print();
    cout << " ";
    print_RelOp(op);
    cout << " ";
    right->print();
    cout << "? " << *true_label->id << ": " << *false_label->id << endl;
}
void Move::print(){
    assert(dst && src);
    dst->print();
    cout << " = ";
    src->print();
    cout << endl;
}
void ExpStm::print(){
    assert(exp);
    cout << "expstm(";
    exp->print();
    cout << ")" << endl;
}
void Return::print(){
    cout << "return ";
    if(exp){
        exp->print();
    }else{
        cout << "void";
    }
    cout << endl;
}
void Phi::print(){
    // assert(0);
    cout<<"phi";
    auto temp_it = tl->begin();
    auto label_it = ll->begin();
    for(; temp_it!=tl->end()&&label_it!=ll->end();){
        cout<<"("<<*(*temp_it)->id<<","<<*(*label_it)->id<<")"<<",";
        temp_it++;
        label_it++;
    }
    
}
void Binop::print(){
    assert(left && right);
    cout << "(";
    if(left)
        left->print();
    cout << " ";
    print_BinOp(op);
    cout << " ";
    if(right)
        right->print();
    cout << ")";

}
void Mem::print(){
    assert(exp);
    cout << "mem(";
    exp->print();
    cout << " type: ";
    utils::print_TempType(type);
    cout << ")";
}
void Temp::print(){
    assert(temp);
    cout << "temp(" << *temp->id << " type: ";
    utils::print_TempType(type);
    cout << ")";
}
void Eseq::print(){
    assert(stm && exp);
    cout << "eseq(";
    stm->print();
    cout << ", ";
    exp->print();
    cout << ")";
}
void Name::print(){
    assert(label);
    cout << "label(" << *label->id << ")";
}
void Format::print(){
    assert(str);
    cout << "format(" << *str << ")";
}
void Const::print(){
    string c;
    if(this->type == utils::TempType::INT){
        c = std::to_string(this->n.i);
    }else if(this->type == utils::TempType::FLOAT){
        c = std::to_string(this->n.f);
    }else{
        assert(0);
    }
    cout << "const(" << c << " type: ";
    utils::print_TempType(type);
    cout << ")";
}
void Call::print(){
    assert(name);
    cout << *name << "(";
    if(args){
        for(auto& arg: *args){
            arg->print();
            cout << ", ";
        }
    }
    cout << " type: ";
    utils::print_TempType(type);
    cout << ")";
}


}