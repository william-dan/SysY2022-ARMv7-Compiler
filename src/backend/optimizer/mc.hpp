#include "../ssa/build_ssa.hpp"

namespace ssa{

class MeanlessCalculation{
public:
MeanlessCalculation(BlockGraph* bg);
void handle_single_stm(mir::Stm* s);

};


}