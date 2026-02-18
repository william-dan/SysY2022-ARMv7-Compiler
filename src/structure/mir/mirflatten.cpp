#include "mir.hpp"
#include <string>
#include <cassert>

namespace mir{
using namespace std;

//1. 展开深层嵌套ir结构
//2. 如果是全局变量，那么在右边
//3. 查找malloc



Stm* Seq::flatten(){
    left = left->flatten();
    right = right->flatten();
    return this;
}

Stm* Label::flatten(){
    return this;
}

Stm* Jump::flatten(){
    return this;
}

Stm* Cjump::flatten(){
    auto l_pack = left->flatten();
    auto r_pack = right->flatten();
    left = l_pack.first;
    right = r_pack.first;
    assert(l_pack.second == r_pack.second);
    return this;
}

Stm* Move::flatten(){
    if(dst->kind == ExpType::MEM){
        Mem* mem_dst = static_cast<Mem*>(dst);
        auto m_pack = mem_dst->exp->flatten();
        mem_dst->exp = m_pack.first;
        assert(m_pack.second == utils::TempType::INT);
        auto s_pack = src->flatten();
        src = s_pack.first;
        assert(s_pack.second == mem_dst->type);
    }else{
        assert(dst->kind == ExpType::TEMP);        
        if(src->kind == ExpType::BINOP){
            Binop* binop_src = static_cast<Binop*>(src);
            auto l_pack = binop_src->left->flatten();
            auto r_pack = binop_src->right->flatten();
            binop_src->left = l_pack.first;
            binop_src->right = r_pack.first;
        }else{
            auto s_pack = src->flatten();
            src = s_pack.first;
            // assert(d_pack.second == s_pack.second);
        }
        
        // if(d_pack.second != s_pack.second){
        //     print();
        //     utils::print_TempType(d_pack.second);
        //     utils::print_TempType(s_pack.second);
        // }

        
    }
    return this;
}

Stm* ExpStm::flatten(){
    auto e_pack = exp->flatten();
    exp = e_pack.first;
    return this;
}

Stm* Return::flatten(){
    auto e_pack = exp->flatten();
    exp = e_pack.first;
    return this;
}

std::pair<Exp*,utils::TempType> Phi::flatten(){
    assert(tl->size());
    return make_pair(this, tl->front()->type);
}

// assert: a = b op c
std::pair<Exp*,utils::TempType> Binop::flatten(){
    auto l_pack = left->flatten();
    auto r_pack = right->flatten();
    assert(l_pack.second == r_pack.second);
    left = l_pack.first;
    right = r_pack.first;
    utils::Temp* tmp = new utils::Temp(l_pack.second);    
    auto eseq = new Eseq(
        new Move(new Temp(tmp), this),
        new Temp(tmp)
    );
    eseq->type = l_pack.second;
    return make_pair(eseq, l_pack.second);

}

// assert: a = mem()
std::pair<Exp*,utils::TempType> Mem::flatten(){
    auto e_pack = exp->flatten();
    exp = e_pack.first;
    assert(e_pack.second == utils::TempType::INT);
    utils::Temp* tmp = new utils::Temp(type);

    return make_pair(new Eseq(
        new Move(new Temp(tmp), this),
        new Temp(tmp)
    ), type);
}

std::pair<Exp*,utils::TempType> Temp::flatten(){
    type = temp->type;
    if(GlobalInfo::global_table.count(temp)){
        auto info = GlobalInfo::global_table[temp];
        auto newtmp = new utils::Temp(type);
        return make_pair(
            new Eseq(new Move(new Temp(newtmp), this), new Temp(newtmp)),
            type
        );
    }else{
        return make_pair(this, temp->type);
    }
}


std::pair<Exp*,utils::TempType> Eseq::flatten(){
    stm = stm->flatten();
    auto e_pack = exp->flatten();
    exp = e_pack.first;
    type = e_pack.second;
    return make_pair(this, e_pack.second);
}

std::pair<Exp*,utils::TempType> Name::flatten(){
    // assert(0);
    type = utils::TempType::INT;
    return make_pair(this, utils::TempType::INT);
}

std::pair<Exp*,utils::TempType> Format::flatten(){
    type = utils::TempType::INT;
    return make_pair(this, utils::TempType::INT);
}

std::pair<Exp*,utils::TempType> Const::flatten(){
    return make_pair(this, type);
}

// assert: a = call()
std::pair<Exp*,utils::TempType> Call::flatten(){
    for(auto& arg: *args){
        auto a_pack = arg->flatten();
        arg = a_pack.first;
    }
    if(this->type == utils::TempType::VOID){
        return make_pair(this, type);
    }else{
        // cout << "in call flatten" << endl;
        // utils::print_TempType(type);
        utils::Temp* tmp = new utils::Temp(type);
        return make_pair(new Eseq(
            new Move(new Temp(tmp), this),
            new Temp(tmp)
        ), type);
    }
}





}