#include "peephole.hpp"
namespace assem{

// bool same_inst(Instr* left, Instr* right, std::unordered_map<Instr*, std::unordered_map<utils::Temp*, int>> use_depth){
//     if(left->type != right->type) return false;
//     if(left->assem != right->assem) return false;     
//     auto left_temp = left->get_def();
//     auto right_temp = right->get_def();
//     auto l = left_temp->begin();
//     auto r = right_temp->begin();
//     for(;;){
//         if(l == left_temp->end() && r == right_temp->end()) break;
//         if(l == left_temp->end()) return false;
//         if(r == right_temp->end()) return false;
//         if(*l != *r) return false;
//         l++;
//         r++;
//     }
//     left_temp = left->get_use();
//     right_temp = right->get_use();
//      l = left_temp->begin();
//      r = right_temp->begin();
//     for(;;){
//         if(l == left_temp->end() && r == right_temp->end()) break;
//         if(l == left_temp->end()) return false;
//         if(r == right_temp->end()) return false;
//         if(*l != *r) return false;
//         if(use_depth[left].at(*l) != use_depth[right].at(*r)) return false;
//         l++;
//         r++;
//     }
// }

void PeepHole::clean_nop_instr(){
    for(auto iter = il->begin(); iter!=il->end();){
        auto i = *iter;
        if(pattern->is_add0(i)) {
            iter = il->erase(iter);
        }
        else if(pattern->is_mul1(i)) iter = il->erase(iter);
        else if(pattern->is_movself(i)) iter = il->erase(iter);
        else iter++;
    }
}

void PeepHole::clean_unused(){
    auto fg = new cfg::FlowGraph(il, utils::TempType::INT);
    fg->Liveness();
    for(auto node: *fg->mynodes){
        bool is_unused = true;
        bool has_side = false;
        auto defs = *fg->def(node);
        auto uses = *fg->def(node);
        auto outs = *fg->FG_Out(node);
        if(defs.empty()) continue;
        for(auto& d: defs){
            if(outs.count(d)) is_unused = false;
        }
        if(is_unused){
             auto info = std::any_cast<cfg::FGinfo*>(node->info);
             if(info->instr->assem[0] == 'b') continue;
             il->remove(info->instr);
        }
    }
}

void PeepHole::clean_mutidef(){
    //def not use
    std::unordered_map<utils::Temp*, Instr*> last_define; 
    std::vector<Instr*> clean_i;
    for(auto iter = il->begin(); iter!=il->end(); iter++){
        auto i = *iter;
        if(i->type == AssemType::Label){
            last_define.clear();
            continue;
        }
        if(i->assem.substr(0, 2) != "bl"){
            last_define.clear();
            continue;
        }
        auto defs = i->get_def();
        auto uses = i->get_use();
        for(auto& u: *uses){
            last_define[u]=0;
        }
        if(defs->size() > 1 || defs->size() == 0) continue;
        
        for(auto& d: *defs){
            if(last_define.count(d)){
                if(last_define[d]) clean_i.push_back(last_define[d]);
            }
            if(i->assem.substr(0, 4) != "movw")last_define[d] = i;
        }
    }
    for(auto i: clean_i){
        il->remove(i);
    }

    //same ldr
    std::unordered_map<std::string, bool> addr_def;
    last_define.clear();
    clean_i.clear();
    for(auto iter = il->begin(); iter!=il->end(); iter++){
        auto i = *iter;
        auto defs = i->get_def();
        auto uses = i->get_use();
        if(i->assem.substr(0, 16) == "ldr `d0, [`s0, #"){
            if(uses->front()->to_string() == "fp"){
                i->print();
                if(last_define.count(defs->front())){
                    if(last_define[defs->front()]->assem == i->assem){
                        if(!addr_def[i->assem]) {
                            i->print();
                            clean_i.push_back(i);
                            continue;
                        }
                    }
                }
                addr_def[i->assem] = false;

            }
        }
        if(i->assem.substr(0, 16) == "str `d0, [`s0, #"){
            if(uses->front()->to_string() == "fp"){
                auto assem = i->assem;
                assem.replace(assem.find("str"), 3, "ldr");
                addr_def[assem] = true;
            }
        }
        if(i->type == AssemType::Label){
            last_define.clear();
            addr_def.clear();
            continue;
        }
        if(i->assem.substr(0, 2) == "bl"){
            last_define.clear();
            addr_def.clear();
            continue;
        }
        for(auto& d: *defs){
            if(i->assem.substr(0, 4) != "movw")last_define[d] = i;
        }
    }

     for(auto i: clean_i){
        il->remove(i);
    }
    //same def
    // last_define.clear(); 
    // clean_i.clear();
    // std::unordered_map<utils::Temp*, int> def_stack; 
    // std::unordered_map<Instr*, std::unordered_map<utils::Temp*, int>> use_depth; 
    // for(auto iter = il->begin(); iter!=il->end(); iter++){
    //     auto i = *iter;
    //     if(i->type == AssemType::Label){
    //         last_define.clear();
    //         def_stack.clear();
    //         use_depth.clear();
    //         continue;
    //     }
    //     if(i->assem.substr(0, 2) == "bl"){
    //         last_define.clear();
    //         def_stack.clear();
    //         use_depth.clear();
    //         continue;
    //     }
    //     auto defs = i->get_def();
    //     auto uses = i->get_use();
    //     for(auto& u: *uses){
    //         use_depth[i].insert(std::make_pair(u, def_stack[u]));
    //     }
    //     for(auto& d: *defs){
    //         def_stack[d]++;
    //     }
    //     if(defs->size() > 1 || defs->size() == 0) continue;
    //     for(auto& d: *defs){
    //         if(last_define.count(d)){
    //             if(last_define[d]){
    //                 if(same_inst(last_define[d], i, use_depth)) {
    //                     if(i->assem.substr(0, 3)!="ldr" && i->assem.substr(0, 3)!="str")clean_i.push_back(i);
    //                     continue;
    //                 }
    //                 else if(i->assem.substr(0, 4) != "movw") last_define[d] = i;
    //             }
    //         }
    //         if(i->assem.substr(0, 4) != "movw") last_define[d] = i;
    //     }
    // }
    // for(auto i: clean_i){
    //     il->remove(i);
    // }
}

}