#pragma once


#include <iostream>
#include <list>
#include <unordered_map>
#include <list>
#include <string>
#include <set>
#include <vector>






namespace utils {
using std::unordered_map;
using std::string;
using std::list;

enum class TempType {
    VOID, INT, FLOAT
};

void print_TempType(TempType tt);



class Temp {
private:
    //静态变量：temp表
    static int tempno;
    
    friend class std::hash<Temp>;

public:

    string* id;
    TempType type;
    //用string构造、直接构造
    Temp();
    Temp(string* id);
    Temp(TempType type);
    Temp(TempType type, string* id);
    static std::unordered_map<std::string, Temp*> int_temp_table;
    static std::unordered_map<std::string, Temp*> float_temp_table;

    static const std::vector<std::pair<string,int>> special_reg;
    static Temp* new_Temp(TempType type, string* id);
        
    bool isRegister()const ;
    string to_string()const ;
    bool operator==(const Temp& t)const;
    bool operator!=(const Temp& t)const;

};




typedef std::list<utils::Temp*> TempList;
TempList* TempList_add(TempList* tl, Temp* t);
TempList* TempList_union(TempList* tl1, TempList* tl2);
TempList* TempList_diff(TempList* tl1, TempList* tl2);
bool TempList_eq(TempList* tl1, TempList* tl2);
TempList* TempList_filter(TempList* tl, TempType type);

typedef std::set<utils::Temp*> TempSet;


enum SpecialLabelType{
    COMMON,
    LC
};
class Label {
private:
    //静态变量：label表
    static int labelno;
    static int LCno;
    friend class std::hash<Label>;
public:

    string* id;
    SpecialLabelType type;
    //用string构造、直接构造
    Label();
    Label(string* id);
    Label(SpecialLabelType type);
    string to_string()const ;
    //运算符判断相等，根据名字
    bool operator==(const Label& l)const ;
    bool operator!=(const Label& l)const ;
    
};
typedef utils::list<utils::Label*> LabelList;

};


namespace std{
    template<>
    class hash<utils::Temp>{
    public:
        size_t operator()(const utils::Temp& tmp)const {
            return hash<string>()(*tmp.id);
        }
    };
    template<>
    class hash<utils::Label>{
    public:
        size_t operator()(const utils::Label& lab)const {
            return hash<string>()(*lab.id);
        }
    };
};
