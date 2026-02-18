#include "assem.hpp"
#include <vector>
#include <algorithm>
#include <string>
#include <cassert>
#include "../mir/mir.hpp"


namespace assem {
using namespace std;

void clean_instr(InstrList* il){
    Instr* prei = NULL;
    for(auto i: *il){
        if(i->type == AssemType::Oper){
            auto operi = static_cast<OperInstr*>(i);
            auto assem = operi->assem;
            auto src = operi->src;
            auto dst = operi->dst;
            if(assem == "add `d0 `s0 #0" ||
            assem == "mul `d0 `s0 #1"){
                if(src->size() == 1 && dst->size() == 1){
                    if(src->front() == dst->front()){
                        il->remove(i);
                    }
                }
            }
        }
    }
}
    void OperInstr::print(){
        assert(src && dst && jumps);
        string assem_copy = assem;
        int i = 0;
        for(auto temp: *src){
            string substr = string("`s")+std::to_string(i);
            if(assem_copy.find(substr) != std::string::npos)
                assem_copy.replace(assem_copy.find(substr), substr.size(), temp->to_string()); 
            i++;
        }

        i = 0;
        for(auto temp: *dst){
            string substr = string("`d")+std::to_string(i);
            if(assem_copy.find(substr) != std::string::npos)
                assem_copy.replace(assem_copy.find(substr), substr.size(), temp->to_string()); 
            i++;
        }

        i = 0;
        for(auto label: *jumps){
            string substr = string("`j")+std::to_string(i);
            if(assem_copy.find(substr) != std::string::npos)
                assem_copy.replace(assem_copy.find(substr), substr.size(), label->to_string()); 
            i++;
        }

        std::cout << assem_copy << std::endl;
    }


    void MoveInstr::print(){
        assert(src && dst);
        
        string assem_copy = assem;
        int i = 0;
        for(auto temp: *src){
            string substr = string("`s")+std::to_string(i);
            if(assem_copy.find(substr) != std::string::npos)
                assem_copy.replace(assem_copy.find(substr), substr.size(), temp->to_string()); 
            i++;
        }

        i = 0;
        for(auto temp: *dst){
            string substr = string("`d")+std::to_string(i);
            if(assem_copy.find(substr) != std::string::npos)
                assem_copy.replace(assem_copy.find(substr), substr.size(), temp->to_string()); 
            i++;
        }

        i = 0;
        std::cout << assem_copy << std::endl;
    }

    void LabelInstr::print(){
        assert(label);
        string assem_copy = label->to_string() + string(":");
        std::cout << assem_copy << std::endl;
    }

    

    bool isOperand2(int n){
        int t = n;
        // assert(t < 0xffffffff);
        vector<int> fill;
        int valid = 0x00000003;
        fill.resize(16);
        for(int i = 0; i < 16; i++){
            fill[i] = (t & valid) >> (i * 2);
            valid <<= 2;
        }
        // for(auto i: fill) cout << i << "\t";
        // cout << endl;
        int r = 0;
        for(int i = 0; i < 16; i++){
            if(fill[i]){
                r = i;
                break;
            }
        }
        int l = 0;
        for(int i = 15; i >= 0; i--){
            if(fill[i]){
                l = i;
                break;
            }
        }
        assert(l >= r);
        if(l - r + 1 <= 4) return true;
        int max_hole = 0;
        int r_prev = r;
        for(int i = r+1; i < l; i++){
            if(fill[i]){
                max_hole = max(max_hole, i - r_prev - 1);
                r_prev = i;
            }
        }
        if(max_hole >= 12) return true;
        return false;
    }
    bool isImm16(int n){
        return n <= 0xffff && n >= 0;
    }
    bool isImm12(int n){
        return n <= 0xfff && n >= -0xfff;
    }

    bool is2power(int n){
        int positive_n = abs(n);
        return (n & (n-1)) == 0;
    }


    std::pair<int,int> get_BigConst(int n){
        return make_pair((n & 0xffff0000) >> 16, n & 0xffff);
    }
    vector<int> get_BigConst_8(int n){
        int hi8 = n & 0xff000000;
        int mhi8 = n & 0x00ff0000;
        int mlo8 = n & 0x0000ff00;
        int lo8 = n & 0x000000ff;
        return {hi8, mhi8, mlo8, lo8};

    }

    void emit_ldr_back(assem::Instrlist* instrlist, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype){
        assert(dsts->size() == 1 && srcs->size() == 1);
        auto dst = dsts->front(), src = srcs->front();
        if(off == 0){
            if(dst->type == utils::TempType::FLOAT){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "vldr.32 `d0, [`s0]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else if(dst->type == utils::TempType::INT){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "ldr `d0, [`s0]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else{
                assert(0);
            }
        }else if((dst->type == utils::TempType::FLOAT && off > -1020 && off < 1020) || 
        (dst->type == utils::TempType::INT && assem::isImm12(off))){
            if(dst->type == utils::TempType::FLOAT){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "vldr.32 `d0, [`s0, #" + std::to_string(off) + "]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else if(dst->type == utils::TempType::INT){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "ldr `d0, [`s0, #" + std::to_string(off) + "]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else{
                assert(0);
            }
        }else{
            auto bigconst_tmp = new utils::Temp(utils::TempType::INT);
            if(assem::isOperand2(-off)){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "sub `d0, `s0, #" + std::to_string(-off),
                        new utils::TempList{src}, new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
            }else{
                auto off_pack = assem::get_BigConst(-off);
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "movw `d0, #:lower16:" + std::to_string(-off),
                        new utils::TempList(), new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "movt `d0, #:upper16:" + std::to_string(-off),
                        new utils::TempList(), new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "sub `d0, `s0, `s1",
                        new utils::TempList{src, bigconst_tmp}, new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
            }
            string assem;
            if(dst->type == utils::TempType::FLOAT){
                assem = "vldr.32 `d0, [`s0]";
                
            }else if(dst->type == utils::TempType::INT){
                assem = "ldr `d0, [`s0]";
            }else{
                assert(0);
            }
            instrlist->emplace_back(
                new assem::OperInstr(
                    assem, new utils::TempList{bigconst_tmp}, 
                    dsts, new utils::LabelList()
                )
            );
        }
    }
    void emit_ldr(assem::Instrlist* instrlist, assem::Instrlist::iterator& it, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype){
        assert(dsts->size() == 1 && srcs->size() == 1);
        auto dst = dsts->front(), src = srcs->front();
        if(off == 0){
            if(dst->type == utils::TempType::FLOAT){
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "vldr.32 `d0, [`s0]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else if(dst->type == utils::TempType::INT){
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "ldr `d0, [`s0]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else{
                assert(0);
            }
        }else if((dst->type == utils::TempType::FLOAT && off > -1020 && off < 1020) || 
        (dst->type == utils::TempType::INT && assem::isImm12(off))){
            if(dst->type == utils::TempType::FLOAT){
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "vldr.32 `d0, [`s0, #" + std::to_string(off) + "]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else if(dst->type == utils::TempType::INT){
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "ldr `d0, [`s0, #" + std::to_string(off) + "]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else{
                assert(0);
            }
        }else{
            auto bigconst_tmp = new utils::Temp(utils::TempType::INT);
            if(assem::isOperand2(-off)){
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "sub `d0, `s0, #" + std::to_string(-off),
                        new utils::TempList{src}, new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
            }else{
                auto off_pack = assem::get_BigConst(-off);
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "movw `d0, #:lower16:" + std::to_string(-off),
                        new utils::TempList(), new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "movt `d0, #:upper16:" + std::to_string(-off),
                        new utils::TempList(), new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "sub `d0, `s0, `s1",
                        new utils::TempList{src, bigconst_tmp}, new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
            }
            string assem;
            if(dst->type == utils::TempType::FLOAT){
                assem = "vldr.32 `d0, [`s0]";
                
            }else if(dst->type == utils::TempType::INT){
                assem = "ldr `d0, [`s0]";
            }else{
                assert(0);
            }
            instrlist->emplace(it,
                new assem::OperInstr(
                    assem, new utils::TempList{bigconst_tmp}, 
                    dsts, new utils::LabelList()
                )
            );
        }
    }
    void emit_str_back(assem::Instrlist* instrlist, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype){
        assert(srcs->size() == 2 && dsts->size() == 0);
        auto dst = srcs->front();
        auto src = srcs->back();
        if(off == 0){
            if(dst->type == utils::TempType::FLOAT){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "vstr.32 `s0, [`s1]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else if(dst->type == utils::TempType::INT){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "str `s0, [`s1]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else{
                assert(0);
            }
        }else if((dst->type == utils::TempType::FLOAT && off > -1020 && off < 1020) || 
        (dst->type == utils::TempType::INT && assem::isImm12(off))){
            if(dst->type == utils::TempType::FLOAT){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "vstr.32 `s0, [`s1, #" + std::to_string(off) + "]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else if(dst->type == utils::TempType::INT){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "str `s0, [`s1, #" + std::to_string(off) + "]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else{
                assert(0);
            }
        }else{
            auto bigconst_tmp = new utils::Temp(utils::TempType::INT);
            if(assem::isOperand2(-off)){
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "sub `d0, `s0, #" + std::to_string(-off),
                        new utils::TempList{src}, new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
            }else{
                auto off_pack = assem::get_BigConst(-off);
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "movw `d0, #:lower16:" + std::to_string(-off),
                        new utils::TempList(), new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "movt `d0, #:upper16:" + std::to_string(-off),
                        new utils::TempList(), new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
                instrlist->emplace_back(
                    new assem::OperInstr(
                        "sub `d0, `s0, `s1",
                        new utils::TempList{src, bigconst_tmp}, new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
            }
            string assem;
            if(dst->type == utils::TempType::FLOAT){
                assem = "vstr.32 `s0, [`s1]";
                
            }else if(dst->type == utils::TempType::INT){
                assem = "str `s0, [`s1]";
            }else{
                assert(0);
            }
            instrlist->emplace_back(
                new assem::OperInstr(
                    assem, new utils::TempList{dst, bigconst_tmp}, 
                    dsts, new utils::LabelList()
                )
            );
        }

    }
    void emit_str(assem::Instrlist* instrlist, assem::Instrlist::iterator& it, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype){
        assert(srcs->size() == 2 && dsts->size() == 0);
        auto dst = srcs->front();
        auto src = srcs->back();
        if(off == 0){
            if(dst->type == utils::TempType::FLOAT){
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "vstr.32 `s0, [`s1]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else if(dst->type == utils::TempType::INT){
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "str `s0, [`s1]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else{
                assert(0);
            }
        }else if((dst->type == utils::TempType::FLOAT && off > -1020 && off < 1020) || 
        (dst->type == utils::TempType::INT && assem::isImm12(off))){
            if(dst->type == utils::TempType::FLOAT){
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "vstr.32 `s0, [`s1, #" + std::to_string(off) + "]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else if(dst->type == utils::TempType::INT){
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "str `s0, [`s1, #" + std::to_string(off) + "]", 
                        srcs, dsts, new utils::LabelList()
                    )
                );
            }else{
                assert(0);
            }
        }else{
            auto bigconst_tmp = new utils::Temp(utils::TempType::INT);
            if(assem::isOperand2(-off)){
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "sub `d0, `s0, #" + std::to_string(-off),
                        new utils::TempList{src}, new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
            }else{
                auto off_pack = assem::get_BigConst(-off);
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "movw `d0, #:lower16:" + std::to_string(-off),
                        new utils::TempList(), new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "movt `d0, #:upper16:" + std::to_string(-off),
                        new utils::TempList(), new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
                instrlist->emplace(it,
                    new assem::OperInstr(
                        "sub `d0, `s0, `s1",
                        new utils::TempList{src, bigconst_tmp}, new utils::TempList{bigconst_tmp},
                        new utils::LabelList()
                    )
                );
            }
            string assem;
            if(dst->type == utils::TempType::FLOAT){
                assem = "vstr.32 `s0, [`s1]";
                
            }else if(dst->type == utils::TempType::INT){
                assem = "str `s0, [`s1]";
            }else{
                assert(0);
            }
            instrlist->emplace(it,
                new assem::OperInstr(
                    assem, new utils::TempList{dst, bigconst_tmp}, 
                    dsts, new utils::LabelList()
                )
            );
        }


    }
    void emit_mov_back(assem::Instrlist* instrlist, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype){
        assert(dsts->size() == 1);
        auto dst = dsts->front();
        if(!srcs->empty()){
            assert(srcs->size() == 1);
            auto src = srcs->front();
            assert(dst->type == src->type);
            if(dst->type == utils::TempType::INT){
                instrlist->emplace_back(
                    new assem::MoveInstr(
                        "mov `d0, `s0", srcs, dsts
                    )
                );
            }else{
                instrlist->emplace_back(
                    new assem::MoveInstr(
                        "vmov.f32 `d0, `s0", srcs, dsts
                    )
                );
            }
        }else{
            if(isImm12(off) || isOperand2(off)){      
                if(dst->type == utils::TempType::INT){
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "mov `d0, #" + to_string(off), srcs, dsts, new utils::LabelList()
                        )
                    );
                }else{
                    auto int_tmp = new utils::Temp(utils::TempType::INT);  
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "mov `d0, #" + to_string(off), srcs, new utils::TempList{int_tmp}, new utils::LabelList()
                        )
                    );
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "vmov `d0, `s0", new utils::TempList{int_tmp}, new utils::TempList{dst}, new utils::LabelList()
                        )
                    );
                }
            }else{
                auto bigconst = get_BigConst(off);
                if(dst->type == utils::TempType::INT){                    
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "movw `d0, #:lower16:" + std::to_string(off),
                            new utils::TempList(), new utils::TempList{dst},
                            new utils::LabelList()
                        )
                    );
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "movt `d0, #:upper16:" + std::to_string(off),
                            new utils::TempList(), new utils::TempList{dst},
                            new utils::LabelList()
                        )
                    );
                }else{
                    auto int_tmp = new utils::Temp(utils::TempType::INT);
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "movw `d0, #:lower16:" + std::to_string(off),
                            new utils::TempList(), new utils::TempList{int_tmp},
                            new utils::LabelList()
                        )
                    );
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "movt `d0, #:upper16:" + std::to_string(off),
                            new utils::TempList(), new utils::TempList{int_tmp},
                            new utils::LabelList()
                        )
                    );
                    instrlist->emplace_back(
                        new assem::OperInstr(
                            "vmov `d0, `s0",
                            new utils::TempList{int_tmp}, new utils::TempList{dst},
                            new utils::LabelList()
                        )
                    );
                }
                
                
            }
        }

    }
    void emit_mov(assem::Instrlist* instrlist, assem::Instrlist::iterator& it, utils::TempList* dsts,  utils::TempList* srcs, int off, utils::TempType temptype){
        assert(dsts->size() == 1);
        auto dst = dsts->front();
        if(!srcs->empty()){
            assert(srcs->size() == 1);
            auto src = srcs->front();
            assert(dst->type == src->type);
            if(dst->type == utils::TempType::INT){
                instrlist->emplace(it,
                    new assem::MoveInstr(
                        "mov `d0, `s0", srcs, dsts
                    )
                );
            }else{
                instrlist->emplace(it,
                    new assem::MoveInstr(
                        "vmov.f32 `d0, `s0", srcs, dsts
                    )
                );
            }
        }else{
            if(isImm12(off) || isOperand2(off)){      
                if(dst->type == utils::TempType::INT){
                    instrlist->emplace(it,
                        new assem::OperInstr(
                            "mov `d0, #" + to_string(off), srcs, dsts, new utils::LabelList()
                        )
                    );
                }else{
                    auto int_tmp = new utils::Temp(utils::TempType::INT);  
                    instrlist->emplace(it,
                        new assem::OperInstr(
                            "mov `d0, #" + to_string(off), srcs, new utils::TempList{int_tmp}, new utils::LabelList()
                        )
                    );
                    instrlist->emplace(it,
                        new assem::OperInstr(
                            "vmov `d0, `s0", new utils::TempList{int_tmp}, new utils::TempList{dst}, new utils::LabelList()
                        )
                    );
                }
            }else{
                auto bigconst = get_BigConst(off);
                if(dst->type == utils::TempType::INT){                    
                    instrlist->emplace(it,
                        new assem::OperInstr(
                            "movw `d0, #:lower16:" + std::to_string(off),
                            new utils::TempList(), new utils::TempList{dst},
                            new utils::LabelList()
                        )
                    );
                    instrlist->emplace(it,
                        new assem::OperInstr(
                            "movt `d0, #:upper16:" + std::to_string(off),
                            new utils::TempList(), new utils::TempList{dst},
                            new utils::LabelList()
                        )
                    );
                }else{
                    auto int_tmp = new utils::Temp(utils::TempType::INT);
                    instrlist->emplace(it,
                        new assem::OperInstr(
                            "movw `d0, #:lower16:" + std::to_string(off),
                            new utils::TempList(), new utils::TempList{int_tmp},
                            new utils::LabelList()
                        )
                    );
                    instrlist->emplace(it,
                        new assem::OperInstr(
                            "movt `d0, #:upper16:" + std::to_string(off),
                            new utils::TempList(), new utils::TempList{int_tmp},
                            new utils::LabelList()
                        )
                    );
                    instrlist->emplace(it,
                        new assem::OperInstr(
                            "vmov `d0, `s0",
                            new utils::TempList{int_tmp}, new utils::TempList{dst},
                            new utils::LabelList()
                        )
                    );
                }
                
                
            }
        }
    }
  
}
