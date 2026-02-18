
#include <stdio.h>
#include <string>
#include <iostream>
#include <cassert>

#include "utils/table.hpp"
#include "structure/ast/ast.hpp"
#include "structure/mir/mir.hpp"
#include "utils/temp.hpp"

#include "backend/canon/canon.hpp"

#include "backend/cfg/bg.hpp"
#include "backend/cfg/assem_bg.hpp"


#include "backend/optimizer/cp.hpp"
#include "backend/optimizer/de.hpp"
#include "backend/optimizer/gcm.hpp"
#include "backend/optimizer/loop.hpp"
#include "backend/optimizer/rp.hpp"
#include "backend/optimizer/gvc.hpp"
#include "backend/optimizer/mc.hpp"
#include "backend/optimizer/gvn.hpp"
#include "backend/optimizer/am.hpp"
#include "backend/optimizer/peephole.hpp"


#include "backend/alloc/regalloc.hpp"
#include "frontend/lexer.hpp"
#include "frontend/parser.hpp"

using namespace std;

// extern int yydebug;
extern int yyparse();
extern FILE *yyin;
extern AST::CompUnits* root;

int main(int argc, char** argv) {
  //compiler testcase.sysy -S -o testcase.s [-O1]
  std::string source_code_path, target_code_path;
  bool is_user_defined_output = false, need_assem_output = false;
  bool functional = true;
  for(int i = 0; i <argc; i++){
    std::string argv_s = argv[i];
    if(argv_s.find(".sy") != std::string::npos){
      if(source_code_path.empty()) source_code_path = argv_s;
    }else if(argv_s.find(".s") != std::string::npos){
      if(target_code_path.empty()) target_code_path = argv_s;
    }else if(argv_s == "-S" || argv_s == "-s"){
      need_assem_output = true;
    }else if(argv_s == "-O" || argv_s == "-o"){
      is_user_defined_output = true;
    }else if(argv_s == "-O1"){
      functional = false;
    }
  }
  if(!is_user_defined_output){
    target_code_path = source_code_path;
    target_code_path.replace(target_code_path.find(".sy"), 3, ".s");
  }

    
  // std::cout << "-------------" << source_code_path << "-------------" << std::endl;
  yyin = fopen(source_code_path.c_str(), "r");
  assert(yyin);
  yyparse();
  fclose(yyin);
  // root->printTree(0);
  auto venv = new TABLE::SymbolTable<ENTRY::Entry*>();
  auto fenv = new TABLE::SymbolTable<ENTRY::EnFunction*>();
  mir::UnitList* newUnitList = root->semanticAnalysis(venv, fenv);
  for(auto& v: *venv) delete v.second.first;
  for(auto& f: *fenv) delete f.second.first;
  delete venv;
  delete fenv;
  auto ul = newUnitList;
  // for(auto& u: *ul) u->print();
  mir::get_global_table(ul);
  auto tot_il = new assem::InstrList();
  std::unordered_map<mir::Fun*, mir::StmList*> fun_ir;
  std::vector<std::string> func_names;
  // cerr<<"get global ok"<<endl;
  for(auto& u: *ul){
      if(u->kind == mir::UnitType::FUN){
          auto fun = static_cast<mir::Fun*>(u);
          fun->stm->fix(true);
          fun->stm->fix(false);
          fun->stm->flatten();
          // fun->stm->print();
          // cerr<<"flatten ok"<<endl;
          auto sl = canon::linearize(fun->stm);
          // for(auto& s: *sl) s->print();
          fun_ir[fun] = sl;
          func_names.push_back(*fun->name);
      }

  }
  // cerr<<"gvc before"<<endl;
  auto gvc = ssa::GlobalVarConst(&fun_ir);
  // cerr << "gvc ok" << endl;
  mir::FunctionInline* funcs = new mir::FunctionInline(fun_ir, func_names);
  // cerr << "inline ok" << endl;
  
  for(auto& fun_info: funcs->func_infos){
      auto fun = fun_info.second->fun;
      // for(auto& s: *fun_info.second->ir) s->print();
      auto am = mir::ArrayMotion(fun_info.second->ir);
      auto b = canon::basicBlocks(fun_info.second->ir, *fun->name);
      cfg::BlockGraph* bg = new cfg::BlockGraph(b);
      delete b;
      // auto looper = ssa::Loop(bg->mynodes);
      // looper.Unroll();
      auto mc = ssa::MeanlessCalculation(bg);
      auto rp = ssa::ReplicatePropagation(bg, false);

    
      auto ssa = ssa::Ssa(bg->mynodes, fun->args);
      auto cp = ssa::ConstPropagation(bg->mynodes);
      auto de = ssa::DeadcodeElimination(bg->mynodes);
      ssa.remove_empty();
      // bg->show();
      // auto gcm = ssa::GlobalCodeMotion(bg->mynodes);
      // bg->show();
      
      
     
      if(!functional){
        // auto gvn = ssa::GlobalValueNumber(bg, &funcs->func_infos);
        mc = ssa::MeanlessCalculation(bg);
        auto gvn = ssa::GlobalValueNumber(bg, &funcs->func_infos);
        mc = ssa::MeanlessCalculation(bg);
        gvn = ssa::GlobalValueNumber(bg, &funcs->func_infos);
        mc = ssa::MeanlessCalculation(bg);
        gvn = ssa::GlobalValueNumber(bg, &funcs->func_infos);
      }
      // bg->show();
      
      
      cp = ssa::ConstPropagation(bg->mynodes);
      de = ssa::DeadcodeElimination(bg->mynodes);
      ssa.back_to_ir();
      ssa.remove_empty();
      
      bg->detach_cond();

      bg->merge_node();
      bg->deadnode_eliminate();

      for(int i = 0; i < 5; i++){
        rp = ssa::ReplicatePropagation(bg, false);
        mc = ssa::MeanlessCalculation(bg);
      } 

      
      // rp = ssa::ReplicatePropagation(bg, true);
      // mc = ssa::MeanlessCalculation(bg);
      
      // bg->show();
      auto sl_after_trace = canon::traceSchedule(bg->toBlock());
      auto il = mir::ir2assem(fun, sl_after_trace);
      // for(auto& i: *il) i->print();

      auto alloc = alloc::Allocator(fun_info.second->recursive);
      alloc.reg_alloc(il);
      tot_il->splice(tot_il->end(), *il);
    }
  // assem::clean_instr(tot_il);
  auto ph = assem::PeepHole(tot_il);
  ph.pass();
  tot_il = mir::assem_add_epilog(tot_il);
  
  FILE* output = freopen(target_code_path.c_str(), "w", stdout);
  for(auto& i: *tot_il) i->print();
  fclose(output);
      
	return 0;
}
