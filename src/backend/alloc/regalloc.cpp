#include "regalloc.hpp"
#include "../cfg/flowgraph.hpp"
#include "../cfg/graph.hpp"
#include "../cfg/ig.hpp"
#include "../../structure/assem/assem.hpp"
#include "../../structure/mir/mir.hpp"


#include <unordered_map>
#include <string>
#include <cassert>
namespace alloc{
using namespace std;
    // int Allocator::real_spill_n = 0;




    assem::InstrList* Allocator::reg_alloc(assem::Instrlist* instrlist){
        temptype = utils::TempType::FLOAT;
       
        int iteration_n = 0;
        while(1){ 
            // cerr << "regalloc iteration: " << iteration_n << endl;
            auto fg = new cfg::FlowGraph(instrlist, temptype);
            // fg->show();
            fg->Liveness();
            // fg->showLiveness();
            auto ig = new cfg::IG(fg);
            // ig->show();
            auto colorer = new Color(ig, temptype, iteration_n > 0);
            auto result = colorer->color();
            // std::cout << "color over" << std::endl;
            if(!result.spill_table.empty()){
                instrlist = real_spill(instrlist, temptype, result);
                delete fg;
                delete ig;
                delete colorer;
            }
            else {
                instrlist = rename_temp(ig, instrlist, temptype, result);
                delete fg;
                delete ig;
                delete colorer;
                break;
            }

            
            iteration_n++;
        }
        // cerr << "iteration_n: " << iteration_n << endl;
        temptype = utils::TempType::INT;
        iteration_n = 0;
        while(1){ 
            // cerr << "regalloc iteration: " << iteration_n << endl;
            auto fg = new cfg::FlowGraph(instrlist, temptype);
            // fg->show();
            fg->Liveness();
            // fg->showLiveness();
            auto ig = new cfg::IG(fg);
            // ig->show();
            auto colorer = new Color(ig, temptype, iteration_n > 0);
            auto result = colorer->color();
            // std::cout << "color over" << std::endl;
            if(!result.spill_table.empty()){
                instrlist = real_spill(instrlist, temptype, result);
                delete fg;
                delete ig;
                delete colorer;
            }
            else {
                instrlist = rename_temp(ig, instrlist, temptype, result);
                delete fg;
                delete ig;
                delete colorer;
                break;
            }

            
            iteration_n++;
        }
        // cerr << "iteration_n: " << iteration_n << endl;
        // std::cout << "over" << std::endl;
        for(auto it = instrlist->begin(); it != instrlist->end(); it++){
            auto instr = *it;
            if(instr->assem == ""){ // sp increase spill_n
                // cout << "sp increase change" << endl;
                it = instrlist->erase(it);
                if(Color::real_spill_n > 0){
                    int off = 0;
                    if(Color::real_spill_n % 2) off = Color::real_spill_n + 5;
                    else off = Color::real_spill_n + 4;
                    off *= mir::DATA_SIZE;
                    if(assem::isOperand2(off)){
                        instrlist->emplace(it, new assem::OperInstr(
                            "sub sp, sp, #" + std::to_string(off),
                            new utils::TempList(),
                            new utils::TempList(),
                            new utils::LabelList()
                        ));
                    }else{
                        auto pack = assem::get_BigConst_8(off);
                        for(auto& i: pack){
                            if(i){
                                instrlist->emplace(it, new assem::OperInstr(
                                    "sub sp, sp, #" + std::to_string(i),
                                    new utils::TempList(),
                                    new utils::TempList(),
                                    new utils::LabelList()
                                ));
                            }
                        }
                        
                    }
                    
                }
                
            }
        }
        Color::real_spill_n = 0;
        return instrlist;
    };

    assem::Instrlist* Allocator::real_spill(assem::Instrlist* instrlist, utils::TempType temptype, ColorResult& result){
        // cout << "in real spill" << endl;
        for(auto it = instrlist->begin(); it != instrlist->end(); ){
            auto instr = *it;
            utils::TempList *uses = instr->get_use(), *defs = instr->get_def();
            // auto bg = it;
            // bg--;
            bool spilled = false;
            for(auto& temp: *uses){
                if(temp->type != temptype) continue;
                if(!result.spill_table.count(temp)) continue;
                int off = result.spill_table[temp];
                temp = new utils::Temp(temptype);
                // if(off < 16 && !is_recursive && temptype == utils::TempType::INT){
                //     instrlist->emplace(it, 
                //         new assem::OperInstr(
                //             "vmov `d0, d" + to_string(off + 16) + "[0]",
                //             new utils::TempList(),
                //             new utils::TempList{temp},
                //             new utils::LabelList()
                //         )
                //     );
                //     spilled = true;
                // }
                // else if(off < 32 && !is_recursive && temptype == utils::TempType::INT){
                //     instrlist->emplace(it, 
                //         new assem::OperInstr(
                //             "vmov `d0, d" + to_string(off) + "[1]",
                //             new utils::TempList(),
                //             new utils::TempList{temp},
                //             new utils::LabelList()
                //         )
                //     );
                //     spilled = true;
                // }
                // else{
                    off = -((off+1)*mir::DATA_SIZE);
                    spilled = true;
                    assem::emit_ldr(instrlist, it, new utils::TempList{temp}, 
                        new utils::TempList{utils::Temp::new_Temp(utils::TempType::INT, new std::string("fp"))}, off, temptype);
                // }
            }
            auto after = it;
            after++;
            for(auto& temp: *defs){
                if(temp->type != temptype) continue;
                if(!result.spill_table.count(temp)) continue;
                int off = result.spill_table[temp];
                temp = new utils::Temp(temptype);
                // if(off < 16 && !is_recursive && temptype == utils::TempType::INT){
                //     instrlist->emplace(after, 
                //         new assem::OperInstr(
                //             "vmov d" + to_string(off + 16) + "[0], `s0",
                //             new utils::TempList{temp},
                //             new utils::TempList(),
                //             new utils::LabelList()
                //         )
                //     );
                //     spilled = true;
                // }
                // else if(off < 32 && !is_recursive && temptype == utils::TempType::INT){
                //     instrlist->emplace(after, 
                //         new assem::OperInstr(
                //             "vmov d" + to_string(off) + "[1], `s0",
                //             new utils::TempList{temp},
                //             new utils::TempList(),
                //             new utils::LabelList()
                //         )
                //     );
                //     spilled = true;
                // }
                // else{

                    off = -((off+1)*mir::DATA_SIZE);
                    spilled = true;
                    assem::emit_str(instrlist, after, new utils::TempList(), 
                        new utils::TempList{temp, utils::Temp::new_Temp(utils::TempType::INT, new std::string("fp"))}, off, temptype);
                // }
            }
            // if(spilled)
            //     ipos[instr] = make_pair(++bg, after);
            it = after;
        }
        return instrlist;
    };

    assem::Instrlist* Allocator::rename_temp(cfg::IG* ig, assem::Instrlist* instrlist, utils::TempType temptype, ColorResult& result){
        // cout << "in rename temp" << endl;
        for(auto& instr: *instrlist){
            utils::TempList *srcs = instr->get_use(), *defs = instr->get_def();
            for(auto& src: *srcs){
                assert(src->type != utils::TempType::VOID);
                if(src->type != temptype){
                    // cout << "src temptype: " << src->to_string() << endl;
                    continue;
                }
                if(src->isRegister()){
                    // cout << "src reg: " << src->to_string() << endl;
                    continue;
                }
                // std::cout << *src->id << std::endl;
                assert(ig->t2n.count(src) > 0);
                // cout << "check color table before" << endl;
                assert(result.color_table.count(src) > 0);
                // cout << "check color table after" << endl;
                auto id = std::to_string(result.color_table[src]);
                src = utils::Temp::new_Temp(temptype, new std::string(id));
            }
            for(auto& def: *defs){
                assert(def->type != utils::TempType::VOID);
                if(def->type != temptype){
                    // cout << "def temptype: " << def->to_string() << endl;
                    continue;
                }
                if(def->isRegister()){
                    // cout << "def reg: " << def->to_string() << endl;
                    continue;
                }
                // std::cout << *def->id << std::endl;

                assert(ig->t2n.count(def) > 0);
                assert(result.color_table.count(def) > 0);
                auto id = std::to_string(result.color_table[def]);
                def = utils::Temp::new_Temp(temptype, new std::string(id));
            }
        }
        // for(auto& i: *instrlist) i->print();

        for(auto it = instrlist->begin(); it != instrlist->end(); ){
            auto instr = *it;
            if(instr->type == assem::AssemType::Move){
                auto srcs = instr->get_use();
                auto defs = instr->get_def();
                assert(srcs->size() == 1 && defs->size() == 1);
                if(srcs->front() == defs->front()){
                
                    it = instrlist->erase(it);
                    continue;
                }
            }
            it++;
        }
        return instrlist;
    };

    

}
