#pragma once
#include <iostream>

#include "../../structure/mir/mir.hpp"

namespace canon {
using mir::StmList;
using mir::Stm;
using mir::Block;
using std::string;


StmList* linearize(Stm* stm);
Block* basicBlocks(StmList* stmList, string& funname);
StmList* traceSchedule(Block* b);

};