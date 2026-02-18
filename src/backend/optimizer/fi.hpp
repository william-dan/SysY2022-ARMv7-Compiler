#include "mir.hpp"
namespace mir{
using namespace std;
class Func_Info{
public:
    Func_Info(Fun* fun, StmList* ir)
    :fun(fun)
    ,ir(ir){
        stksize = fun->args->size();
        call_num = called_num = 0;
        length = ir->size();
        has_malloc = false;
        recursive = false;
        side_effect = false;
    };
    int stksize;
    int called_num;
    int call_num;
    int length;
    Fun* fun;
    StmList* ir;
    bool recursive;
    bool side_effect;
    bool has_malloc;
    void show(){
        cerr << "fun: " << *fun->name << endl;
        cerr << "stksize: " << stksize << endl;
        cerr << "called_num: " << called_num << endl;
        cerr << "call_num: " << call_num << endl;
        cerr << "length: " << length << endl;
        cerr << "recursive: " << recursive << endl;
        cerr << "has_malloc: " << has_malloc << endl;
    }

};


class FunctionInline{
public:
    FunctionInline(std::unordered_map<mir::Fun*, mir::StmList*> fun_ir, std::vector<std::string> func_names){
        funcs = func_names;
        for(auto fun_ir_map: fun_ir){
            auto fun = fun_ir_map.first;
            auto sl = fun_ir_map.second;
            func_infos[*fun->name] = new Func_Info(fun, sl);
        }
        // std::cerr << "all_funcs" << std::endl;
        // for(auto fun_info: func_infos){
        //     // std::cerr << fun_info.first << std::endl;
        // }
        fun_analysis();
        // std::cout << "fun analysis pass" <<std::endl;
        fun_inline();
    }
    void show_after_inline(){
        for(auto fun_info: func_infos){
            std::cout << fun_info.first << std::endl;
            auto sl = fun_info.second->ir;
            for(auto stm: *sl){
                stm->print();
            }
        }
    }
    void fun_analysis();
    void fun_inline();
    bool valid_inline(std::string fun_name);
    std::unordered_map<std::string, Func_Info*> func_infos;
    std::vector<std::string> funcs;
    int length_threshold = 8192;
};


}