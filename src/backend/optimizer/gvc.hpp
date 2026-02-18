#include "../ssa/build_ssa.hpp"
#include "../../structure/mir/mir.hpp"

namespace ssa{

class GlobalVarConst{
public:
    GlobalVarConst(std::unordered_map<mir::Fun*, mir::StmList*>* fun_ir);
    std::unordered_map<mir::Fun*, mir::StmList*>* fun_ir;
    bool is_init_zero(utils::Temp* ga, mir::Exp* e);
};

}