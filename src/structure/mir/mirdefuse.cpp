#include "mir.hpp"
#include <string>
#include <cassert>

namespace mir{
using namespace std;

utils::TempSet Seq::get_use(){
    assert(0);
}
utils::TempSet Seq::get_def(){
    assert(0);
}
utils::TempSet Label::get_use(){
    return {};
}
utils::TempSet Label::get_def(){
    return {};
}
utils::TempSet Jump::get_use(){
    return {};
}
utils::TempSet Jump::get_def(){
    return {};
}
utils::TempSet Cjump::get_use(){
    utils::TempSet ret;
    switch (left->kind)
    {
    case ExpType::CONST:
    {

    }
    break;
    case ExpType::MEM:
    {
        auto mem = static_cast<Mem*>(left);
        if(mem->exp->kind == ExpType::TEMP){
            ret.emplace(static_cast<Temp*>(mem->exp)->temp);
        }
    }
    break;
    case ExpType::TEMP:
    {
        ret.emplace(static_cast<Temp*>(left)->temp);
    }
    break;
    default:
        print_ExpType(left->kind);
        assert(0);
    break;
    }
    switch (right->kind)
    {
    case ExpType::CONST:
    {

    }
    break;
    case ExpType::MEM:
    {
        auto mem = static_cast<Mem*>(right);
        if(mem->exp->kind == ExpType::TEMP){
            ret.emplace(static_cast<Temp*>(mem->exp)->temp);
        }
    }
    break;
    case ExpType::TEMP:
    {
        ret.emplace(static_cast<Temp*>(right)->temp);
    }
    break;
    default:
        cerr << (int)(right->kind) << endl;
        print_ExpType(right->kind);
        assert(0);
    break;
    }
    return ret;
}
utils::TempSet Cjump::get_def(){
    return {};
}
utils::TempSet Move::get_use(){
    utils::TempSet ret;
    switch (dst->kind)
    {
    case ExpType::MEM:
    {
        auto mem = static_cast<Mem*>(dst);
        if(mem->exp->kind == ExpType::TEMP){
            ret.emplace(static_cast<Temp*>(mem->exp)->temp);
        }
    }
    break;
    case ExpType::TEMP:
    {

    }
    break;
    
    default:
        assert(0);
    break;
    }
    switch (src->kind)
    {
    case ExpType::BINOP:
    {
        auto binop = static_cast<Binop*>(src);
        switch (binop->left->kind)
        {
        case ExpType::CONST:
        case ExpType::MEM:
        case ExpType::NAME:
        {

        }
        break;
        case ExpType::TEMP:
        {
            ret.emplace(static_cast<Temp*>(binop->left)->temp);
        }
        break;
        default:
            assert(0);
        break;
        }
        switch (binop->right->kind)
        {
        case ExpType::CONST:
        case ExpType::MEM:
        case ExpType::NAME:
        {

        }
        break;
        case ExpType::TEMP:
        {
            ret.emplace(static_cast<Temp*>(binop->right)->temp);
        }
        break;
        default:
            assert(0);
        break;
        }
    }
    break;
    case ExpType::CALL:
    {
        auto call = static_cast<Call*>(src);
        for(auto& arg: *call->args){
            switch (arg->kind)
            {
            case ExpType::CONST:
            case ExpType::FORMAT:
            case ExpType::NAME:
            {

            }
            break;
            case ExpType::MEM:
            {
                auto mem = static_cast<Mem*>(arg);
                assert(mem->exp->kind == ExpType::NAME);
            }   
            break;
            case ExpType::TEMP:
            {
                ret.emplace(static_cast<Temp*>(arg)->temp);
            }
            break;
            default:
                assert(0);
            break;
            }
        }
    }
    break;
    case ExpType::CONST:
    {

    }
    break;
    case ExpType::MEM:
    {
        auto mem = static_cast<Mem*>(src);
        if(mem->exp->kind == ExpType::TEMP){
            ret.emplace(static_cast<Temp*>(mem->exp)->temp);
        }
    }
    break;
    case ExpType::PHI:
    {
        for(auto& t: *static_cast<Phi*>(src)->tl){
            ret.emplace(t);
        }
    }
    break;
    case ExpType::TEMP:
    {
        ret.emplace(static_cast<Temp*>(src)->temp);
    }
    break;
    case ExpType::NAME:
    {

    }
    break;
    default:
        assert(0);
    break;
    }
    return ret;
}
utils::TempSet Move::get_def(){
    utils::TempSet ret;
    switch (dst->kind)
    {
    case ExpType::MEM:
    {

    }
    break;
    case ExpType::TEMP:
    {
        ret.emplace(static_cast<Temp*>(dst)->temp);
    }
    break;
    default:
        assert(0);
    break;
    }
    return ret;
}
utils::TempSet ExpStm::get_use(){
    utils::TempSet ret;
    if(exp->kind != ExpType::CALL) return {};
    auto call = static_cast<Call*>(exp);
    for(auto& arg: *call->args){
        switch (arg->kind)
        {
        case ExpType::CONST:
        case ExpType::FORMAT:
        case ExpType::NAME:
        {

        }
        break;
        case ExpType::MEM:
        {
            auto mem = static_cast<Mem*>(arg);
            assert(mem->exp->kind == ExpType::NAME);
        }   
        break;
        case ExpType::TEMP:
        {
            ret.emplace(static_cast<Temp*>(arg)->temp);
        }
        break;
        default:
            assert(0);
        break;
        }
    }
    return ret;
}
utils::TempSet ExpStm::get_def(){
    return {};
}
utils::TempSet Return::get_use(){
    utils::TempSet ret;
    switch (exp->kind)
    {
    case ExpType::CONST:
    {

    }
    break;
    case ExpType::MEM:
    {
        auto mem = static_cast<Mem*>(exp);
        assert(mem->exp->kind == ExpType::NAME);
    }
    break;
    case ExpType::TEMP:
    {
        ret.emplace(static_cast<Temp*>(exp)->temp);
    }
    break;
    case ExpType::NAME:
    {
        
    }
    break;
    default:
        // exp->print();
        assert(0);
    break;
    }
    return ret;
}
utils::TempSet Return::get_def(){
    return {};
}

std::vector<Exp**> Seq::get_use_ref(){
    assert(0);
}
std::vector<Exp**> Seq::get_def_ref(){
    assert(0);
}
std::vector<Exp**> Label::get_use_ref(){
    return {};
}
std::vector<Exp**> Label::get_def_ref(){
    return {};
}
std::vector<Exp**> Jump::get_use_ref(){
    return {};
}
std::vector<Exp**> Jump::get_def_ref(){
    return {};
}
std::vector<Exp**> Cjump::get_use_ref(){
    std::vector<Exp**> ret;
    switch (left->kind)
    {
    case ExpType::CONST:
    {

    }
    break;
    case ExpType::MEM:
    {
        auto mem = static_cast<Mem*>(left);
        if(mem->exp->kind == ExpType::TEMP){
            ret.emplace_back(&mem->exp);
        }
    }
    break;
    case ExpType::TEMP:
    {
        ret.emplace_back(&left);
    }
    break;
    default:
        assert(0);
    break;
    }
    switch (right->kind)
    {
    case ExpType::CONST:
    {

    }
    break;
    case ExpType::MEM:
    {
        auto mem = static_cast<Mem*>(right);
        if(mem->exp->kind == ExpType::TEMP){
            ret.emplace_back(&mem->exp);
        }
    }
    break;
    case ExpType::TEMP:
    {
        ret.emplace_back(&right);
    }
    break;
    default:
        assert(0);
    break;
    }
    return ret;
}
std::vector<Exp**> Cjump::get_def_ref(){
    return {};
}
std::vector<Exp**> Move::get_use_ref(){
    std::vector<Exp**> ret;
    switch (dst->kind)
    {
    case ExpType::MEM:
    {
        auto mem = static_cast<Mem*>(dst);
        if(mem->exp->kind == ExpType::TEMP){
            ret.emplace_back(&mem->exp);
        }
    }
    break;
    case ExpType::TEMP:
    {

    }
    break;
    default:
        assert(0);
    break;
    }
    switch (src->kind)
    {
    case ExpType::BINOP:
    {
        auto binop = static_cast<Binop*>(src);
        switch (binop->left->kind)
        {
        case ExpType::CONST:
        case ExpType::MEM:
        case ExpType::NAME:
        {

        }
        break;
        case ExpType::TEMP:
        {
            ret.emplace_back(&binop->left);
        }
        break;
        default:
            assert(0);
        break;
        }
        switch (binop->right->kind)
        {
        case ExpType::CONST:
        case ExpType::MEM:
        case ExpType::NAME:
        {

        }
        break;
        case ExpType::TEMP:
        {
            ret.emplace_back(&binop->right);
        }
        break;
        default:
            assert(0);
        break;
        }
    }
    break;
    case ExpType::CALL:
    {
        auto call = static_cast<Call*>(src);
        for(auto& arg: *call->args){
            switch (arg->kind)
            {
            case ExpType::CONST:
            case ExpType::FORMAT:
            case ExpType::NAME:
            {

            }
            break;
            case ExpType::MEM:
            {
                auto mem = static_cast<Mem*>(arg);
                assert(mem->exp->kind == ExpType::NAME);
            }   
            break;
            case ExpType::TEMP:
            {
                ret.emplace_back(&arg);
            }
            break;
            default:
                assert(0);
            break;
            }
        }
    }
    break;
    case ExpType::CONST:
    {

    }
    break;
    case ExpType::MEM:
    {
        auto mem = static_cast<Mem*>(src);
        if(mem->exp->kind == ExpType::TEMP){
            ret.emplace_back(&mem->exp);
        }
    }
    break;
    case ExpType::PHI:
    {
        assert(0);
    }
    break;
    case ExpType::TEMP:
    {
        ret.emplace_back(&src);
    }
    break;
    case ExpType::NAME:
    {

    }
    break;
    default:
        assert(0);
    break;
    }
    return ret;
}
std::vector<Exp**> Move::get_def_ref(){
    std::vector<Exp**> ret;
    switch (dst->kind)
    {
    case ExpType::MEM:
    {

    }
    break;
    case ExpType::TEMP:
    {
        ret.emplace_back(&dst);
    }
    break;
    default:
        assert(0);
    break;
    }
    return ret;
}
std::vector<Exp**> ExpStm::get_use_ref(){
    std::vector<Exp**> ret;
    if(exp->kind != ExpType::CALL) return {};
    auto call = static_cast<Call*>(exp);
    for(auto& arg: *call->args){
        switch (arg->kind)
        {
        case ExpType::CONST:
        case ExpType::FORMAT:
        case ExpType::NAME:
        {

        }
        break;
        case ExpType::MEM:
        {
            auto mem = static_cast<Mem*>(arg);
            assert(mem->exp->kind == ExpType::NAME);
        }   
        break;
        case ExpType::TEMP:
        {
            ret.emplace_back(&arg);
        }
        break;
        default:
            assert(0);
        break;
        }
    }
    return ret;
}
std::vector<Exp**> ExpStm::get_def_ref(){
    return {};
}
std::vector<Exp**> Return::get_use_ref(){
    std::vector<Exp**> ret;
    switch (exp->kind)
    {
    case ExpType::CONST:
    {

    }
    break;
    case ExpType::MEM:
    {
        auto mem = static_cast<Mem*>(exp);
        assert(mem->exp->kind == ExpType::NAME);
    }
    break;
    case ExpType::TEMP:
    {
        ret.emplace_back(&exp);
    }
    break;
    case ExpType::NAME:
    {
        
    }
    break;
    default:
        // exp->print();
        assert(0);
    break;
    }
    return ret;
}
std::vector<Exp**> Return::get_def_ref(){
    return {};
}



};
