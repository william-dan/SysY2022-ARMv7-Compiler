#ifndef __ENTRY_H
#define __ENTRY_H

/*
TODO:
type    ->      entry information
symbol  ->      symbol_table

*/



#include "temp.hpp"

#include <vector>
#include <string>
#include <assert.h>


namespace ENTRY {

enum class Type_t { Type_VOID, Type_ARRAY, Type_INT, Type_FLOAT, Type_FUNCTION };

struct Type {
    Type_t kind;
    Type* point;
    std::vector<Type*> param;
    int dim;
    int arraysize;
    bool isconst;
    // int 和 float 都是4字节，应该可以通过 union 转换
    int* Ivalue;
    float* Fvalue;

    Type() {}
    ~Type() {}
    Type(Type* p, Type_t k, int* _value, bool _isconst) : arraysize(1), point(p), kind(k), dim(1), isconst(_isconst) {
        Ivalue = _value;
    }
    Type(Type* p, Type_t k, int* _value, int _dim, bool _isconst) : arraysize(_dim*p->arraysize), point(p), kind(k), dim(_dim), isconst(_isconst) {
        Ivalue = _value;
    }
    Type(Type* p, Type_t k, float* _value, bool _isconst) : arraysize(1), point(p), kind(k), dim(1), isconst(_isconst) {
        Fvalue = _value;
    }
    Type(Type* p, Type_t k, float* _value, int _dim, bool _isconst) : arraysize(_dim*p->arraysize), point(p), kind(k), dim(_dim), isconst(_isconst) {
        Fvalue = _value;
    }
    
    // 可能要先让 ep = e, 再让 ep = rtn
    Type(Type* p, Type_t k, std::vector<Type*> _param, Type* rtn) :
        point(rtn), 
        kind(k), 
        param(_param) {}
};


static inline Type* createVoidInfo() { 
    // return new ENTRY::Type(NULL, ENTRY::Type_t::Type_VOID, NULL, false); 
    return new ENTRY::Type(NULL, ENTRY::Type_t::Type_VOID, (int*)NULL, false);
}

static inline Type* createIntInfo(int* val, bool _isconst) {
    return new ENTRY::Type(NULL, ENTRY::Type_t::Type_INT, val, _isconst);
}

static inline Type* createFloatInfo(float* val, bool _isconst) {
    return new ENTRY::Type(NULL, ENTRY::Type_t::Type_FLOAT, val, _isconst);
}

static inline Type* createArrayInfo(Type* p, int _dim, bool _isconst) {
    return new ENTRY::Type(p, ENTRY::Type_t::Type_ARRAY, (int*)NULL, _dim, _isconst);
}

static inline Type* createFunctionInfo(Type* rtn, std::vector<Type*> _param) {
    return new ENTRY::Type(0, ENTRY::Type_t::Type_FUNCTION, _param, rtn);
}

enum class Entry_t { Entry_LOCAL, Entry_GLOBAL, Entry_FUNCTION };
struct Entry {
    Entry_t kind;
    Type* ty;
    virtual ~Entry(){
        delete ty;
    }
};
struct LocVar : public Entry {
    utils::Temp* temp;
    ~LocVar(){}
    LocVar(Type* _ty, utils::Temp* _temp) {
        assert(_ty->kind != Type_t::Type_FUNCTION && _ty->kind != Type_t::Type_VOID);
        kind = Entry_t::Entry_LOCAL;
        ty = _ty;
        temp = _temp;
    }
};
struct GloVar : public Entry {
    utils::Temp* temp;
    ~GloVar(){}
    GloVar(Type* _ty, utils::Temp* _temp) {
        assert(_ty->kind != Type_t::Type_FUNCTION && _ty->kind != Type_t::Type_VOID);
        kind = Entry_t::Entry_GLOBAL;
        ty = _ty;
        temp = _temp;
    }
};
struct EnFunction : public Entry {
    utils::Label* label;
    utils::TempType returnKind;
    EnFunction(Type* _ty, utils::Label* _label, utils::TempType _returnKind) {
        assert(_ty->kind == Type_t::Type_FUNCTION);
        kind = Entry_t::Entry_FUNCTION;
        returnKind = _returnKind;
        ty = _ty;
        label = _label;
    }
};
};

#endif // __ENTRY_H