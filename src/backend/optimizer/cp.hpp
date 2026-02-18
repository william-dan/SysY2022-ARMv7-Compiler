#include "../ssa/build_ssa.hpp"
namespace ssa{
enum class TempStatus{
    Undifined,
    Const,
    Temp
};
//simple constant propagation
class ConstPropagation{
public:
ConstPropagation(NodeList* nodelist):nodelist(nodelist){
    scp();
    // std::cout << "scp pass" << std::endl;
    replace_temp();
}
void scp();
void replace_temp();
void replace_stm_temp(mir::Stm* stm);
std::pair<bool, mir::Const*> is_const(mir::Exp* exp);
NodeList* nodelist;
std::unordered_map<utils::Temp*, TempStatus> temp_status;
std::unordered_map<utils::Temp*, mir::Const*> temp_const;
std::unordered_map<utils::Temp*, std::list<mir::Stm**>> use_stm;
std::set<mir::Stm**> delete_stm;
};

}//namespace ssa