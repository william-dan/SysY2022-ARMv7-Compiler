#include "../ssa/build_ssa.hpp"
#include "../cfg/flowgraph.hpp"
#include <iostream>



namespace ssa{


class TempPack{
public:
    mir::Stm* s;
    vector<int> ids;
    int id;
    TempPack(): id(0), s(nullptr){}
};

class ReplicatePropagation{
public:
ReplicatePropagation(BlockGraph* bg, bool final_do);
ReplicatePropagation(FlowGraph* fg);


bool try_merge(mir::Stm* s, mir::Exp* after);

void single_node_propagate_bg(Node* node);
void single_node_clean_bg(Node* node);



void single_node_propagate_fg(Node* node);



BlockGraph* bg;
FlowGraph* fg;
bool final_do;

unordered_map<utils::Temp*, TempPack> temp_curr_val;



};




}