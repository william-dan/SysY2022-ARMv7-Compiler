#include "canon.hpp"
#include "../../structure/mir/mir.hpp"
#include "../../utils/temp.hpp"
#include <cassert>

namespace canon {
using namespace mir;
using std::list;
using std::unordered_map;
using std::cout, std::endl;



/// ############# this block is linearize ############# /// 
typedef list<Exp**> ExpRefList;
class StmExp
{
public:
    Stm* s;
    Exp* e;
    StmExp(Stm* s, Exp* e): s(s), e(e){}
};

static bool isNop(Stm* x);
static bool commute(Stm* x, Exp* y);
static Stm* reorder(ExpRefList* el);
static ExpRefList* get_call_el(Exp* e);
static StmExp* do_exp(Exp* e);
static Stm* do_stm(Stm* s);


static bool isNop(Stm* x){
    assert(x);
    return x->kind == StmType::EXP && static_cast<ExpStm*>(x)->exp->kind == ExpType::CONST;
}

static bool commute(Stm* x, Exp* y){
    assert(x && y);
    return isNop(x) || (y->kind == ExpType::NAME || y->kind == ExpType::CONST);
}

static Stm* reorder(ExpRefList* el){
    assert(el);
    // for(auto& e: *el){
    //     (*e)->print();
    //     cout << endl;
    // }
    // std::cout << "in reorder" << std::endl;
    if(el->empty()){
        return new ExpStm(new Const(0));
    }
    if((*el->front())->kind == ExpType::CALL){
        auto call = static_cast<Call*>(*el->front());
        utils::Temp* t = new utils::Temp(call->type);
        *el->front() = new Eseq(new Move(new Temp(t), (**el->front()).exp_deepcopy()), new Temp(t));
        return reorder(el);
    }else{
        StmExp* hd = do_exp(*el->front());
        auto front = el->front();
        el->pop_front();
        Stm* s = reorder(el);
        el->emplace_front(front);
        if(commute(s, hd->e)) {
            // cout<<"commute"<<endl;
            *el->front() = hd->e->exp_deepcopy();
            return new Seq(hd->s, s);
        }else{
            utils::TempType temptype;
            
            // cout<<"not commute"<<endl;
            utils::Temp* t = new utils::Temp(hd->e->get_type());
            *el->front() = new Temp(t);
            return new Seq(hd->s, new Seq(new Move(new Temp(t), hd->e), s));
        }
    }
}

static ExpRefList* get_call_el(Exp* e){
    assert(e);
    assert(e->kind == ExpType::CALL);
    ExpRefList* el = new ExpRefList();
    for(auto& arg: *(static_cast<Call*>(e)->args)){
        el->emplace_back(&arg);
    }
    return el;
}
static StmExp* do_exp(Exp* exp){
    assert(exp);
    switch (exp->kind)
    {
    case ExpType::BINOP:
    {
        Binop* e = static_cast<Binop*>(exp);
        return new StmExp(
            reorder(new ExpRefList{&e->left, &e->right}),
            e
        );
    }
    break;
    case ExpType::MEM:
    {
        Mem* e = static_cast<Mem*>(exp);
        return new StmExp(
            reorder(new ExpRefList{&e->exp}),
            e
        );
    }
    break;
    case ExpType::ESEQ:
    {
        Eseq* e = static_cast<Eseq*>(exp);
        StmExp* x = do_exp(e->exp);
        return new StmExp(new Seq(do_stm(e->stm), x->s), x->e);
    }
    break;
    case ExpType::CALL:
    {
        Call* e = static_cast<Call*>(exp);
        utils::Temp* t = new utils::Temp(e->type);
        return new StmExp(
            new Seq(reorder(get_call_el(e)), new Move(new Temp(t), e)),
            new Temp(t)
        );
    }
    break;
    default:
    {
        return new StmExp(
            reorder(new ExpRefList()),
            exp 
        );
    }
    break;
    }
}
static Stm* do_stm(Stm* stm){
    assert(stm);
    // std::cout << "in do_stm" << std::endl;
    switch (stm->kind)
    {
    case StmType::SEQ:
    {
        Seq* s = static_cast<Seq*>(stm);
        return new Seq(do_stm(s->left), do_stm(s->right));
    }
    break;
    case StmType::JUMP:
    {
        Jump* s = static_cast<Jump*>(stm);
        return new Seq(reorder(new ExpRefList()), s);
    }
    break;
    case StmType::CJUMP:
    {
        Cjump* s = static_cast<Cjump*>(stm);
        return new Seq(reorder(new ExpRefList{&s->left, &s->right}), s);
    }
    break;
    case StmType::MOVE:
    {
        Move* s = static_cast<Move*>(stm);
        // cout << "after static_cast" << endl;
        if(s->dst->kind == ExpType::TEMP && s->src->kind == ExpType::CALL){
            return new Seq(reorder(get_call_el(s->src)), s);
        }else if(s->dst->kind == ExpType::TEMP){
            // cout << "in 2temp" << endl;
            return new Seq(reorder(new ExpRefList{&s->src}), s);
        }else if(s->dst->kind == ExpType::MEM){
            Mem* dst = static_cast<Mem*>(s->dst);
            return new Seq(reorder(new ExpRefList{&dst->exp, &s->src}), s);
        }else if(s->dst->kind == ExpType::ESEQ){
            Eseq* dst = static_cast<Eseq*>(s->dst);
            s->dst = dst->exp;
            return do_stm(new Seq(dst->stm, s));
        }
        assert(0);
    }
    break;
    case StmType::EXP:
    {
        ExpStm* s = static_cast<ExpStm*>(stm);
        if(s->exp->kind == ExpType::CALL){
            return new Seq(reorder(get_call_el(s->exp)), s);
        }else{
            return new Seq(reorder(new ExpRefList{&s->exp}), s);
        }
    }
    break;
    case StmType::RETURN:
    {
        Return* s = static_cast<Return*>(stm);
        return new Seq(reorder(new ExpRefList{&s->exp}), s);
    }
    break;
    default:
        return stm;
    break;
    }
}

static StmList* linear(Stm* stm, StmList* r){
    assert(stm && r);
    // std::cout << "in linear" << std::endl;
    if(stm->kind == StmType::SEQ){
        Seq* s = static_cast<Seq*>(stm);
        return linear(s->left, linear(s->right, r));
    }else{
        r->emplace_front(stm);
        return r;
    }
}

StmList* linearize(Stm* stm){
    assert(stm);
    StmList* sl = linear(do_stm(stm), new StmList());
    // std::cerr << "linearize over" << endl;
    for(auto s = sl->begin(); s != sl->end(); ){
        // (*s)->print();
        if(isNop(*s)){
            s = sl->erase(s);
        }else{
            s++;
        }
    }
    return sl;
}

/// ############# end of linearize ############# /// 




/// ############# this block is basicBlocks ############# /// 

Block* basicBlocks(StmList* stmList, string& funname){
    assert(stmList);
    
    Block* b = new Block();
    b->label = new utils::Label(new string("EXIT_"+funname));
    b->stmLists = new mir::StmListList();
    StmList* curr = new StmList();
    for(auto& s: *stmList){
        // s->print();
        if(curr->empty() && s->kind != StmType::LABEL){
            curr->emplace_back(new Label(new utils::Label()));
        }
        if(s->kind == StmType::JUMP || s->kind == StmType::CJUMP
        || s->kind == StmType::RETURN){
            curr->emplace_back(s);
            b->stmLists->emplace_back(curr);
            curr = new StmList();
        }else if(s->kind == StmType::LABEL && !curr->empty()){
            Label* lab = static_cast<Label*>(s);
            curr->emplace_back(new Jump(lab->label));
            b->stmLists->emplace_back(curr);
            curr = new StmList();
            curr->emplace_back(s);
        }else{
            curr->emplace_back(s);
        }
    }
    if(!curr->empty()){
        curr->emplace_back(
            new Jump(b->label)
        );
        b->stmLists->emplace_back(curr);
    }
    return b;
}

/// ############# end of basicBlocks ############# /// 



/// ############# this block is traceSchedule ############# /// 

static unordered_map<utils::Label*, StmList*> env;
static StmList* ret;
static void trace(StmList* sl){
    assert(sl && ret);
    for(auto& s: *sl) ret->emplace_back(s);
    // cout << "pass ret check" << endl;
    assert(sl && sl->size() > 0);
    assert(sl->front()->kind == StmType::LABEL);
    Label* lab = static_cast<Label*>(sl->front());
    env.erase(lab->label);
    Stm* last = sl->back();
    if(last->kind == StmType::JUMP){
        Jump* j = static_cast<Jump*>(last);
        if(env.count(j->label) > 0){
            ret->pop_back();
            trace(env[j->label]);
        }
    }else if(last->kind == StmType::CJUMP){
        Cjump* cj = static_cast<Cjump*>(last);
        if(env.count(cj->false_label) > 0){
            trace(env[cj->false_label]);
        }else if(env.count(cj->true_label)){
            RelOp notOp = notRel(cj->op);
            sl->back() = new Cjump(notOp, cj->left, cj->right, cj->false_label, cj->true_label);
            trace(env[cj->true_label]);
        }else{
            utils::Label* false_lab = new utils::Label();
            sl->back() = new Cjump(cj->op, cj->left, cj->right, cj->true_label, false_lab);
            ret->emplace_back(new Label(false_lab));
        }
        
    }else if(last->kind == StmType::RETURN){
        Return* r = static_cast<Return*>(last);
    }else{
        // assert(0);
    }
}

StmList* traceSchedule(Block* b){
    assert(b && b->stmLists);
    ret = new StmList();
    for(auto& sl: *b->stmLists){
        assert(sl->front()->kind == StmType::LABEL);
        Label* lab = static_cast<Label*>(sl->front());
        env[lab->label] = sl;
    }
    
    for(auto& sl: *b->stmLists){
        assert(sl->front()->kind == StmType::LABEL);
        Label* lab = static_cast<Label*>(sl->front());
        if(env.count(lab->label) <= 0) continue;
        trace(sl);
        // cout << "one pass" <<endl;
    }
    if(b->label){
        ret->emplace_back(new mir::Label(b->label));
        ret->emplace_back(new mir::Return(new mir::Const(1)));
    }
    return ret;
}


/// ############# end of traceSchedule ############# /// 

};