#include "mir.hpp"
#include <string>
#include <cassert>

namespace mir{
using namespace std;
static unordered_map<string, utils::Label*> fix_table;

void Seq::fix(bool get_label){
    assert(left && right);
    left->fix(get_label);
    right->fix(get_label);
}
void Label::fix(bool get_label){
    assert(label);
    if(get_label){
        fix_table[*label->id] = label;
    }
}
void Jump::fix(bool get_label){
    if(!get_label){
        assert(fix_table.count(*label->id));
        label = fix_table[*label->id];
    }
}
void Cjump::fix(bool get_label){
    assert(left && right && true_label && false_label);
    left->fix(get_label);
    right->fix(get_label);
    if(!get_label){
        assert(fix_table.count(*true_label->id));
        true_label = fix_table[*true_label->id];
        assert(fix_table.count(*false_label->id));
        false_label = fix_table[*false_label->id];
    }
}
void Move::fix(bool get_label){
    assert(dst && src);
    dst->fix(get_label);
    src->fix(get_label);
}
void ExpStm::fix(bool get_label){
    assert(exp);
    exp->fix(get_label);
}
void Return::fix(bool get_label){
    assert(exp);
    exp->fix(get_label);
}
void Phi::fix(bool get_label){
    assert(0);
}
void Binop::fix(bool get_label){
    assert(left && right);
    left->fix(get_label);
    right->fix(get_label);
}
void Mem::fix(bool get_label){
    assert(exp);
    exp->fix(get_label);
}
void Temp::fix(bool get_label){
    assert(temp);
}
void Eseq::fix(bool get_label){
    assert(stm && exp);
    stm->fix(get_label);
    exp->fix(get_label);
}
void Name::fix(bool get_label){
    assert(label);
}
void Format::fix(bool get_label){
    assert(str);
}
void Const::fix(bool get_label){
}
void Call::fix(bool get_label){
    assert(name);
    if(args){
        for(auto& arg: *args){
            arg->fix(get_label);
        }
    }
}


};