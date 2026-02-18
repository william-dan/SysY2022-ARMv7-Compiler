#include "../../structure/assem/assem.hpp"
#include "../cfg/flowgraph.hpp"
namespace assem{

class Pattern{
public:
    bool is_add0(Instr* i);
    bool is_mul1(Instr* i);
    bool is_movself(Instr* i);
    bool has_side_effect(Instr* i);
    bool str_ldr(Instr* first, Instr* second);

};



class PeepHole{
public:
    assem::InstrList* il;
    Pattern* pattern;

    PeepHole(InstrList* il):il(il){
        pattern = new Pattern();
    }
    void clean_nop_instr();
    void clean_unused();
    void clean_mutidef();
    void pass(){
        clean_nop_instr();
        clean_mutidef();
        // clean_unused();
    }
};
}