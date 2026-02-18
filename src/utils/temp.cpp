#include "temp.hpp"
#include <string>
#include <cctype>
#include <vector>
#include <cassert>

namespace utils {
int Temp::tempno = 100;
int Label::labelno = 100;
int Label::LCno = 0;
const std::vector<std::pair<string,int>> Temp::special_reg = {
    std::make_pair("sp", 13), std::make_pair("fp", 11), std::make_pair("lr", 14)
};
std::unordered_map<std::string, Temp*> Temp::int_temp_table;
std::unordered_map<std::string, Temp*> Temp::float_temp_table;



static bool is_number(string& str){
    if(str == "") return false;
    bool is_num = true;
    for(auto& ch: str){
        if(!(ch >= '0' && ch <= '9')){
            is_num = false;
            break;
        }
    }
    return is_num;
}
void print_TempType(TempType tt) {
    switch (tt)
    {
    case TempType::INT:
        std::cout << "int" << std::endl;
        break;
    case TempType::FLOAT:
        std::cout << "float" << std::endl;
        break;
    default:
        std::cout << (int)tt << std::endl;
        // assert(0);
        break;
    }
}
Temp::Temp(){
    type = TempType::INT;
    id = new std::string(std::to_string(Temp::tempno++));
    Temp::int_temp_table[*id] = this;
}
Temp::Temp(string* id){
    assert(0);
    for(auto& sr: Temp::special_reg){
        if(sr.first == *id){
            this->id = new string(std::to_string(sr.second));
            Temp::int_temp_table[*this->id] = this;
            return;
        }
    }
    this->id = id;
    Temp::int_temp_table[*this->id] = this;

    
}
Temp::Temp(TempType type): type(type){
    switch (type)
    {
    case TempType::INT:
    {
        id = new std::string(std::to_string(Temp::tempno++));
        assert(!Temp::int_temp_table.count(*id));
        Temp::int_temp_table[*this->id] = this;
    }
    break;
    case TempType::FLOAT:
    {
        id = new std::string(std::to_string(Temp::tempno++));
        assert(!Temp::float_temp_table.count(*id));
        Temp::float_temp_table[*this->id] = this;
    }
    break;
   
    default:
    assert(0);
    break;
    }
    

}
Temp::Temp(TempType type, string* id): type(type), id(id){
    switch (type)
    {
    case TempType::INT:
    {
        assert(!Temp::int_temp_table.count(*id));
        Temp::int_temp_table[*id] = this;
    }
    break;
    case TempType::FLOAT:
    {
        assert(!Temp::float_temp_table.count(*id));
        Temp::float_temp_table[*id] = this;
    }
    break;
    default:
    assert(0);
    break;
    }
    
}

bool Temp::isRegister()const {
    if(is_number(*id)){
        return std::stoi(*id) < 100;
    }else{
        return false;
    }
}

string Temp::to_string()const {
    if(isRegister()){
        if(type == TempType::INT){
            for(auto& r: Temp::special_reg){
                if(std::to_string(r.second) == *id){
                    return r.first;
                }
            }
            return "r" + *id;
        }else if(type == TempType::FLOAT){
            return "s" + *id;
        }else{
            assert(0);
        }
    }else{
        return "t" + *id;
    }
}


bool Temp::operator==(const Temp& t)const {
    return *id == *t.id;
}
bool Temp::operator!=(const Temp& t)const {
    return !(*id == *t.id);
}

TempList* TempList_add(TempList* tl, Temp* t){
    for(auto& tmp: *tl){
        if(*tmp == *t) return tl;
    }
    tl->emplace_back(t);
    return tl;
}
TempList* TempList_union(TempList* tl1, TempList* tl2){
    TempList* ret = new TempList(*tl1);
    for(auto& t: *tl2){
        ret = TempList_add(ret, t);
    }
    return ret;
}
TempList* TempList_diff(TempList* tl1, TempList* tl2){
    TempList* ret = new TempList();
    for(auto& t1: *tl1){
        bool found = false;
        for(auto& t2: *tl2){
            if(*t1 == *t2){
                found = true;
                break;
            }
        }
        if(!found){
            ret->emplace_back(t1);
        }
    }
    return ret;
}
bool TempList_eq(TempList* tl1, TempList* tl2){
    return TempList_diff(tl1, tl2)->empty() && TempList_diff(tl2, tl1)->empty();
}
TempList* TempList_filter(TempList* tl, TempType type){
    TempList* ret = new TempList();
    for(auto& t: *tl){
        if(t->type == type) ret->emplace_back(t);
    }
    return ret;
}


Label::Label(): type(SpecialLabelType::COMMON){
    id = new std::string(std::to_string(Label::labelno++));
}
Label::Label(string* id): id(id), type(SpecialLabelType::COMMON){
    assert(!is_number(*id));
}
Label::Label(SpecialLabelType type): type(type){
    switch (type)
    {
    case SpecialLabelType::LC:
    {
        id = new std::string(".LC" + std::to_string(LCno++));
    }
    break;
    case SpecialLabelType::COMMON:
    {
        id = new std::string(std::to_string(Label::labelno++));
    }
    break;
    default:
        assert(0);
    break;
    }
}
string Label::to_string()const {
    switch (type)
    {
    case SpecialLabelType::LC:
    {
        return *id;
    }
    break;
    case SpecialLabelType::COMMON:
    {
        if(is_number(*id)) return "l" + *id;
        else return *id;
    }
    break;
    default:
        assert(0);
    break;
    }
}



bool Label::operator==(const Label& l)const {
    return id == l.id;
}
bool Label::operator!=(const Label& l)const {
    return id != l.id;
}

Temp* Temp::new_Temp(TempType type, string* id){
    switch (type)
    {
    case TempType::INT:
    {
        if(Temp::int_temp_table.count(*id) > 0) return Temp::int_temp_table[*id];
        for(auto& sr: Temp::special_reg){
            if(sr.first == *id){
                if(Temp::int_temp_table.count(std::to_string(sr.second)) > 0) return Temp::int_temp_table[std::to_string(sr.second)];
                else{
                    return new Temp(type, new string(std::to_string(sr.second)));
                }
            }
        }
    }
    break;
    case TempType::FLOAT:
    {
        if(Temp::float_temp_table.count(*id) > 0) return Temp::float_temp_table[*id];
        for(auto& sr: Temp::special_reg){
            assert(sr.first != *id);
        }
    }
    break;
    default:
    assert(0);
    break;
    }

    
    
    return new Temp(type, id);
}



};


