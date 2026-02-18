#include "../../utils/temp.hpp"
#include "../../utils/utils.hpp"
#include "mir.hpp"
#include <string>
#include <algorithm>
#include <cassert>


namespace mir{
using namespace std;
std::unordered_map<utils::Temp*, mir::GlobalInfo*> GlobalInfo::global_table;
ExpList ExpList_deepcopy(ExpList& el){
    ExpList ret;
    for(auto& e: el){
        ret.emplace_back(e->exp_deepcopy());
    }
    return ret;
}

Stm* Stm::stm_deepcopy_with_label(std::unordered_map<utils::Label*, utils::Label*>& label_map){
    Stm* stm = this;
    Stm* ret = nullptr;
    switch (stm->kind)
    {
    case StmType::CJUMP:
    {
        utils::Label* new_false_label, *new_true_label;
        auto cjumpstm = static_cast<Cjump*>(stm);
        if (label_map.count(cjumpstm->true_label))
            new_true_label = label_map[cjumpstm->true_label];
        else {
            new_true_label = new utils::Label();
            label_map[cjumpstm->true_label] = new_true_label;
        }

        if (label_map.count(cjumpstm->false_label))
            new_false_label = label_map[cjumpstm->false_label];
        else {
            new_false_label = new utils::Label();
            label_map[cjumpstm->false_label] = new_false_label;
        }

        ret = new Cjump(cjumpstm->op, cjumpstm->left->exp_deepcopy(), cjumpstm->right->exp_deepcopy(), new_true_label, new_false_label);
    }
    break;
    case StmType::EXP:
    {
        ret = new ExpStm(*static_cast<ExpStm*>(stm));
    }
    break;
    case StmType::JUMP:
    {
        utils::Label* new_label;
        auto jumpstm = static_cast<Jump*>(stm);
        if (label_map.count(jumpstm->label))
            new_label = label_map[jumpstm->label];
        else {
            new_label = new utils::Label();
            label_map[jumpstm->label] = new_label;
        }
        ret = new Jump(new_label);
    }
    break;
    case StmType::LABEL:
    {
        utils::Label* new_label;
        auto labelstm = static_cast<Label*>(stm);
        if (label_map.count(labelstm->label))
            new_label = label_map[labelstm->label];
        else {
            new_label = new utils::Label();
            label_map[labelstm->label] = new_label;
        }
        ret = new Label(new_label);
    }
    break;
    case StmType::MOVE:
    {
        ret = new Move(*static_cast<Move*>(stm));
    }
    break;
    case StmType::RETURN:
    {
        ret = new Return(*static_cast<Return*>(stm));
    }
    break;
    case StmType::SEQ:
    {
        ret = new Seq(*static_cast<Seq*>(stm));
    }
    break;
    default:
        assert(0);
    break;
    }
    return ret;
}

Stm* Stm::stm_deepcopy(){
    Stm* stm = this;
    Stm* ret = nullptr;
    switch (stm->kind)
    {
    case StmType::CJUMP:
    {
        ret = new Cjump(*static_cast<Cjump*>(stm));
    }
    break;
    case StmType::EXP:
    {
        ret = new ExpStm(*static_cast<ExpStm*>(stm));
    }
    break;
    case StmType::JUMP:
    {
        ret = new Jump(*static_cast<Jump*>(stm));
    }
    break;
    case StmType::LABEL:
    {
        ret = new Label(*static_cast<Label*>(stm));
    }
    break;
    case StmType::MOVE:
    {
        ret = new Move(*static_cast<Move*>(stm));
    }
    break;
    case StmType::RETURN:
    {
        ret = new Return(*static_cast<Return*>(stm));
    }
    break;
    case StmType::SEQ:
    {
        ret = new Seq(*static_cast<Seq*>(stm));
    }
    break;
    default:
        assert(0);
    break;
    }
    return ret;
}
Exp* Exp::exp_deepcopy(){
    Exp* exp = this;
    Exp* ret = nullptr;
    switch (exp->kind)
    {
    case ExpType::BINOP:
    {
        ret = new Binop(*static_cast<Binop*>(exp));
    }
    break;
    case ExpType::CALL:
    {
        ret = new Call(*static_cast<Call*>(exp));
    }
    break;
    case ExpType::CONST:
    {
        ret = new Const(*static_cast<Const*>(exp));
    }
    break;
    case ExpType::ESEQ:
    {
        ret = new Eseq(*static_cast<Eseq*>(exp));
    }
    break;
    case ExpType::MEM:
    {
        ret = new Mem(*static_cast<Mem*>(exp));
    }
    break;
    case ExpType::NAME:
    {
        ret = new Name(*static_cast<Name*>(exp));
    }
    break;
    case ExpType::PHI:
    {
        ret = new Phi(*static_cast<Phi*>(exp));
    }
    break;
    case ExpType::TEMP:
    {
        ret = new Temp(*static_cast<Temp*>(exp));
    }
    break;
    default:
        assert(0);
    break;
    }
    return ret;
}

Unit::Unit(UnitType kind): kind(kind){}

Unit::Unit(Unit& unit): kind(unit.kind){}

Fun::Fun(string* name, utils::TempList* args, Stm* stm):
    name(name), args(args), stm(stm), Unit::Unit(UnitType::FUN){}

Fun::Fun(Fun& fun):
    name(new string(*fun.name)),
    args(new utils::TempList(*fun.args)),
    stm(fun.stm->stm_deepcopy()),
    Unit(UnitType::FUN){
}

Stm::Stm(StmType kind):
    kind(kind), Unit::Unit(UnitType::STM){}

Stm::Stm(Stm& stm):
    kind(stm.kind), Unit(UnitType::STM){}

Exp::Exp(ExpType kind):
    kind(kind){}

Exp::Exp(Exp& exp):
    kind(exp.kind){}

Block::Block(StmListList* stmLists, utils::Label* label):
    stmLists(stmLists), label(label){}

Block::Block(Block& block){
    stmLists = new StmListList(*block.stmLists);
    for(auto& sl: *stmLists){
        sl = new StmList(*sl);
        for(auto& s: *sl){
            s = s->stm_deepcopy();
        }
    }
    label = block.label;
}

Seq::Seq(Stm* left, Stm* right):
    left(left), right(right), Stm::Stm(StmType::SEQ) {}

Seq::Seq(Seq& seq):
    left(seq.left->stm_deepcopy()),
    right(seq.right->stm_deepcopy()),
    Stm(StmType::SEQ){}

Label::Label(utils::Label* label):
    label(label), Stm::Stm(StmType::LABEL) {}

Label::Label(mir::Label& label): 
    label(label.label), Stm(StmType::LABEL){}

Jump::Jump(utils::Label* label):
    label(label), Stm::Stm(StmType::JUMP) {}

Jump::Jump(Jump& jump): 
    label(jump.label), Stm(StmType::JUMP) {}

Cjump::Cjump(RelOp op, Exp* left, Exp* right, 
    utils::Label* true_label, utils::Label* false_label):
    op(op), left(left), right(right), true_label(true_label), false_label(false_label),
    Stm::Stm(StmType::CJUMP) {}

Cjump::Cjump(Cjump& cjump): 
    op(cjump.op), true_label(cjump.true_label), false_label(cjump.false_label),
    left(cjump.left->exp_deepcopy()),
    right(cjump.right->exp_deepcopy()),
    Stm(StmType::CJUMP) {}

Move::Move(Exp* left, Exp* right):
    dst(left), src(right), Stm::Stm(StmType::MOVE) {}

Move::Move(Move& move): 
    dst(move.dst->exp_deepcopy()),
    src(move.src->exp_deepcopy()),
    Stm(StmType::MOVE){}

ExpStm::ExpStm(Exp* exp):
    exp(exp), Stm::Stm(StmType::EXP) {}

ExpStm::ExpStm(ExpStm& expstm):
    exp(expstm.exp->exp_deepcopy()),
    Stm(StmType::EXP){}

Return::Return(Exp* exp):
    exp(exp), Stm::Stm(StmType::RETURN) {}

Return::Return(Return& ret):
    exp(ret.exp->exp_deepcopy()),
    Stm(StmType::RETURN){}

Phi::Phi(utils::TempList* tl, utils::LabelList* ll):
    tl(tl), ll(ll), Exp::Exp(ExpType::PHI) {}

Phi::Phi(Phi& phi):
    tl(new utils::TempList(*phi.tl)),
    ll(new utils::LabelList(*phi.ll)),
    type(phi.type),
    Exp(ExpType::PHI){}

Binop::Binop(BinOp op, Exp* left, Exp* right):
    op(op), left(left), right(right), Exp::Exp(ExpType::BINOP) {}

Binop::Binop(Binop& binop):
    op(binop.op),
    left(binop.left->exp_deepcopy()),
    right(binop.right->exp_deepcopy()),
    type(binop.type),
    Exp(ExpType::BINOP){}

Mem::Mem(Exp* exp, utils::TempType type):
    exp(exp), type(type), Exp::Exp(ExpType::MEM) {}

Mem::Mem(Mem& mem):
    exp(mem.exp->exp_deepcopy()), type(mem.type),
    Exp(ExpType::MEM){}

Temp::Temp(utils::Temp* temp):
    temp(temp), Exp::Exp(ExpType::TEMP), type(temp->type) {}

Temp::Temp(mir::Temp& temp):
    temp(temp.temp),
    type(temp.type),
    Exp(ExpType::TEMP){}

Eseq::Eseq(Stm* stm, Exp* exp):
    stm(stm), exp(exp), Exp::Exp(ExpType::ESEQ) {}

Eseq::Eseq(Eseq& eseq):
    stm(eseq.stm->stm_deepcopy()),
    exp(eseq.exp->exp_deepcopy()),
    type(eseq.type),
    Exp(ExpType::ESEQ){}

Name::Name(utils::Label* label):
    label(label), Exp::Exp(ExpType::NAME),
    type(utils::TempType::INT) {}

Name::Name(Name& name):
    label(name.label),
    type(name.type),
    Exp(ExpType::NAME){}

Format::Format(string* str):
    str(str), Exp(ExpType::FORMAT) {}

Format::Format(Format& format):
    str(new string(*format.str)), Exp(ExpType::FORMAT),
    type(format.type) {}

Const::Const(float f): type(utils::TempType::FLOAT), Exp::Exp(ExpType::CONST){
    n.f = f;
}
Const::Const(int i): type(utils::TempType::INT), Exp::Exp(ExpType::CONST){
    n.i = i;
}
Const::Const(Const& c):
    type(c.type), n(c.n), Exp(ExpType::CONST){}


string Const::to_string(){
    switch (type)
    {
    case utils::TempType::INT:
        return std::to_string(n.i);
    break;
    case utils::TempType::FLOAT:
        return std::to_string(n.i);
    break;
    default:
        assert(0);
    break;
    }
}
std::pair<int,int> Const::get_BigConst(){
    return make_pair((n.i & 0xffff0000) >> 16, n.i & 0xffff);
}

bool Const::isImm16(){
    return n.i <= 0xffff && n.i >= 0;
}

bool Const::isOperand2(){
    int t = n.i;
    // assert(t < 0xffffffff);
    vector<int> fill;
    int valid = 0x00000003;
    fill.resize(16);
    for(int i = 0; i < 16; i++){
        fill[i] = (t & valid) >> (i * 2);
        valid <<= 2;
    }
    // for(auto i: fill) cout << i << "\t";
    // cout << endl;
    int r = 0;
    for(int i = 0; i < 16; i++){
        if(fill[i]){
            r = i;
            break;
        }
    }
    int l = 0;
    for(int i = 15; i >= 0; i--){
        if(fill[i]){
            l = i;
            break;
        }
    }
    assert(l >= r);
    if(l - r + 1 <= 4) return true;
    int max_hole = 0;
    int r_prev = r;
    for(int i = r+1; i < l; i++){
        if(fill[i]){
            max_hole = max(max_hole, i - r_prev - 1);
            r_prev = i;
        }
    }
    if(max_hole >= 12) return true;
    return false;
}

Call::Call(string* name, ExpList* elist, utils::TempType type):
    name(name), args(elist), Exp::Exp(ExpType::CALL), type(type) {}

Call::Call(Call& call):
    name(new string(*call.name)),
    type(call.type),
    Exp(ExpType::CALL){
        args = new ExpList(*call.args);
        for(auto& e: *args){
            e = e->exp_deepcopy();
        }
    }

GlobalInfo::GlobalInfo(utils::Label* label, string* format): 
label(label), type(GlobalInfoType::FORMAT), is_const(false){
    u.format = format;
}
GlobalInfo::GlobalInfo(utils::Label* label, ConstMap* clist):
label(label), type(GlobalInfoType::ARRAY), is_const(false){
    u.clist = clist;
}
GlobalInfo::GlobalInfo(utils::Label* label, mir::Const* c):
    label(label), type(GlobalInfoType::CONST), is_const(false){
    u.c = c;
}

RelOp notRel(RelOp r){
    switch (r)
    {
    case RelOp::EQ:
        return RelOp::NE;
    case RelOp::NE:
        return RelOp::EQ;
    case RelOp::LT:
        return RelOp::GE;
    case RelOp::GT:
        return RelOp::LE;
    case RelOp::LE:
        return RelOp::GT;
    case RelOp::GE:
        return RelOp::LT;
    case RelOp::ULT:
        return RelOp::UGE;
    case RelOp::ULE:
        return RelOp::UGT;
    case RelOp::UGT:
        return RelOp::ULE;
    case RelOp::UGE:
        return RelOp::ULT;
    default:
        assert(0);
    }
}
RelOp commuteRel(RelOp r){
    assert(0);
}


Exp* mir::Tr_Exp::unEX() {
    switch (kind) {
        case Tr_t::EX: {
            return ex;
        } 
        break;
        case Tr_t::CX: {
            utils::Temp* tmp = new utils::Temp();
            utils::Label* trueL = new utils::Label();
            utils::Label* falseL = new utils::Label();
            doPatch(cx.trues, trueL);
            doPatch(cx.falses, falseL);
            return new Eseq(
                new Move(new Temp(tmp), new Const(1)), new Eseq(cx.stm,
                        new Eseq(new Label(falseL), 
                        new Eseq(new Move(new Temp(tmp), new Const(0)),
                        new Eseq(new Label(trueL), new Temp(tmp))))));
        } 
        break;
    default: {
        assert(0);
    }
    }
}

CX mir::Tr_Exp::unCX() {
    switch (kind) {
        case Tr_t::CX : {
            return cx;
        } 
        break;
        case Tr_t::EX: {
            
            Stm* stm = new Cjump(RelOp::NE, ex, new Const(0), 
                    new utils::Label(new std::string("")), new utils::Label(new std::string("")));

            if (ex->kind == mir::ExpType::CONST) {
                auto ex_Const = static_cast<mir::Const*>(ex);
                if (ex_Const->type == utils::TempType::INT) {
                    stm = new Cjump(RelOp::NE, ex, new Const(0), 
                        new utils::Label(new std::string("")), new utils::Label(new std::string("")));
                }
                else if (ex_Const->type == utils::TempType::FLOAT) {
                    stm = new Cjump(RelOp::NE, ex, new Const(float(0.0)), 
                        new utils::Label(new std::string("")), new utils::Label(new std::string("")));
                }
                else {
                    assert(0);
                }
            }
            else if (ex->kind == mir::ExpType::TEMP) {
                auto ex_Temp = static_cast<mir::Temp*>(ex);
                if (ex_Temp->temp->type == utils::TempType::INT) {
                    stm = new Cjump(RelOp::NE, ex, new Const(0), 
                        new utils::Label(new std::string("")), new utils::Label(new std::string("")));
                }
                else if (ex_Temp->temp->type == utils::TempType::FLOAT) {
                    stm = new Cjump(RelOp::NE, ex, new Const(float(0.0)), 
                        new utils::Label(new std::string("")), new utils::Label(new std::string("")));
                }
                else {
                    assert(0);
                }
            }
            else if (ex->kind == mir::ExpType::BINOP) {
                return CX(new patchList(static_cast<Cjump*>(stm)->true_label, NULL), 
                    new patchList(static_cast<Cjump*>(stm)->false_label, NULL), stm);
            }
            else {      // 未定义行为，但是我觉得没有问题，其他的exptype都不用特殊处理
                // assert(0);
            }
            

            return CX(new patchList(static_cast<Cjump*>(stm)->true_label, NULL), 
                    new patchList(static_cast<Cjump*>(stm)->false_label, NULL), stm);
        } 
        break;
    default: assert(0);
    }
}

Stm* mir::Tr_Exp::unNX() {
    switch (kind) {
        case Tr_t::NX: {
            return nx;
        }
        break;
        case Tr_t::EX: {
            return new ExpStm(ex);
        }
        break;
        case Tr_t::CX: {
            return new ExpStm(unEX());
        }
        break;
    }
    assert(0);
}

Tr_Exp::Tr_Exp(Stm* stm) {
    kind = Tr_t::NX;
    nx = stm;
}
Tr_Exp::Tr_Exp(Exp* exp) {
    kind = Tr_t::EX;
    ex = exp;
}
Tr_Exp::Tr_Exp(patchList* trues, patchList* falses, Stm* stm) {
    kind = Tr_t::CX;
    cx.trues = trues;
    cx.falses = falses;
    cx.stm = stm;
}


};


