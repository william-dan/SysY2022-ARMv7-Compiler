#include "de.hpp"
namespace ssa{
bool DeadcodeElimination::has_side_effect(mir::Stm* stm){
    switch (stm->kind) {
        case mir::StmType::MOVE: {
            auto movstm = static_cast<mir::Move*>(stm);
            auto exp = movstm->src;
            switch (exp->kind){
            case mir::ExpType::CALL:{
                return true;
                break;
            }
            default:
                break;
            }
            if (movstm->dst->kind == mir::ExpType::MEM) {
                return true;
            }
            // if (movstm->dst->kind == mir::ExpType::TEMP) {
            //     return false;
            // }
        }
        case mir::StmType::EXP: {
            auto expstm = static_cast<mir::ExpStm*>(stm);
            if (expstm->exp->kind == mir::ExpType::CALL) {
                return true;
            }
            break;
        }
        case mir::StmType::CJUMP: {
            return false;
            break;
        }
        case mir::StmType::RETURN: {
            return true;
            break;
        }
    }
    return false;
}

mir::Stm* get_condition_stm(Node* node){
    mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
    if(!stmlist) return nullptr;
    auto stm = stmlist->back();
    if(stm->kind == mir::StmType::CJUMP){
        return stm;
    }
    return nullptr;
}

mir::Stm* get_label_stm(Node* node){
    mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
    if(!stmlist) return nullptr;
    auto stm = stmlist->front();
    if(stm->kind == mir::StmType::LABEL){
        return stm;
    }
    return nullptr;
}


void DeadcodeElimination::mask(){
    std::set<mir::Stm*> worklist;
    auto cdgnodes = cdg->mynodes;
    for(auto node: *nodelist){
        mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
        if(node == static_cast<cfg::BlockGraph*>(nodelist->front()->mygraph)->exit){
            active_block.insert(node);
            active_stm.insert(stmlist->front());
            continue;
        }
        for(auto& stm: *stmlist){
            // std::cout << "de in work"<< std::endl;
            stm_to_block[stm] = node;
            auto def = get_def(stm);
            if(def){
                def_stm[static_cast<mir::Temp*>(def)->temp] = stm;
            }
            if(has_side_effect(stm)){
                active_stm.insert(stm);
                worklist.insert(stm);
                active_block.insert(node);
            }
        }
    }
    // std::cout << "mask init" << std::endl;
    while (!worklist.empty())
    {
        
        auto stm = *worklist.begin();
        worklist.erase(stm);
        if(!stm) continue;
        assert(stm);
        // std::cout << "de in work"<< std::endl;
        // stm->print();
        auto uses = get_uses(stm);
        for(auto use: uses){
            if(!active_stm.count(def_stm[static_cast<mir::Temp*>(use)->temp])){
                worklist.insert(def_stm[static_cast<mir::Temp*>(use)->temp]);
                active_stm.insert(def_stm[static_cast<mir::Temp*>(use)->temp]);
                active_block.insert(stm_to_block[stm]);
            }
        }

        //condition branch
        auto this_block = stm_to_block[stm];
        auto this_label = get_label_stm(this_block);
        if(this_label) active_stm.insert(this_label);
        for (auto cdg_pred: *cdg->nodemap[this_block]->preds){
            if(cdg->cdg_to_bg.count(cdg_pred) == 0) continue;
            auto pred = cdg->cdg_to_bg[cdg_pred];
            auto con_stm = get_condition_stm(pred);
            auto pred_label = get_label_stm(pred);
            if(pred_label) active_stm.insert(pred_label);
            if(con_stm){
                if(!active_stm.count(con_stm)){
                worklist.insert(con_stm);
                active_stm.insert(con_stm);
                active_block.insert(stm_to_block[con_stm]);
                }
            }
        }

        //phi

        if(stm->kind == mir::StmType::MOVE){
            auto movstm = static_cast<mir::Move*>(stm);
            if(movstm->src->kind == mir::ExpType::PHI){
                auto phiexp = static_cast<mir::Phi*>(movstm->src); 
                for(auto temp: *phiexp->tl){
                    if(def_stm.count(temp) > 0){
                        // std::cerr << *temp->id << std::endl;
                        assert(def_stm.count(temp)>0);
                        if(!active_stm.count(def_stm[temp])){
                            worklist.insert(def_stm[temp]);
                            active_stm.insert(def_stm[temp]);
                            active_block.insert(stm_to_block[stm]);
                        }
                    }
                }

                for (auto phi_pred: *this_block->preds){
                    auto pred_label = get_label_stm(phi_pred);
                    if(pred_label) active_stm.insert(pred_label);
                    for (auto cdg_pred: *cdg->nodemap[phi_pred]->preds){
                        if(cdg->cdg_to_bg.count(cdg_pred) == 0) continue;
                        auto pred = cdg->cdg_to_bg[cdg_pred];
                        auto con_stm = get_condition_stm(pred);
                        auto pred_label = get_label_stm(pred);
                        if(pred_label) active_stm.insert(pred_label);
                        if(con_stm){
                            if(!active_stm.count(con_stm)){
                            worklist.insert(con_stm);
                            active_stm.insert(con_stm);
                            active_block.insert(stm_to_block[con_stm]);
                            }
                        }
                    }
                }
            }
        }
    }
    
}

Node* DeadcodeElimination::next_active_node(Node* node, Node* pred){
    Node* next_node = NULL;
    if(active_block.count(node)){
        node_parent[node] = pred;
        return node;
    }
    if(visited.count(node)) return NULL;
    visited.insert(node);
    for(auto succ: *node->succs){
        next_node = next_active_node(succ, node);
        if(next_node){
            node_parent[node] = pred;
            return node;
        }
    }
}

void DeadcodeElimination::eliminate(){
    // for(auto node: *nodelist){
    //     mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
    //     for(auto &stm: *stmlist){
    //         if(stm->kind == mir::StmType::JUMP || stm->kind == mir::StmType::CJUMP
    //     || stm->kind == mir::StmType::RETURN ||stm->kind == mir::StmType::LABEL) continue;
    //         if(active_stm.count(stm) == 0){
    //             stm = new mir::ExpStm(new mir::Const(0));
    //         }
    //     }
    // }
    auto bg = static_cast<cfg::BlockGraph*>(nodelist->front()->mygraph);
    auto exitnode = bg->exit;

    NodeList worklist;
    worklist.push_back(nodelist->front());
    NodeSet done;
    while(!worklist.empty()){
        
        auto this_node = worklist.front();
        std::cout << "active block" << this_node->mykey << std::endl;
        worklist.pop_front();
        if(done.count(this_node)) continue;
        done.insert(this_node);
        if(this_node == exitnode) continue;

        //stm
        mir::StmList* stmlist = std::any_cast<mir::StmList*>(this_node->info);
        for(auto &stm: *stmlist){
            if(stm->kind == mir::StmType::JUMP || stm->kind == mir::StmType::CJUMP
        || stm->kind == mir::StmType::RETURN ||stm->kind == mir::StmType::LABEL) continue;
            if(active_stm.count(stm) == 0){
                delete stm;
                stm = new mir::ExpStm(new mir::Const(0));
            }
        }

        //node
        auto jump = stmlist->back();
        if(active_stm.count(jump)){
            auto cjumpstm = static_cast<mir::Cjump*>(jump);
            for(auto succ: *this_node->succs){
                mir::StmList* succ_sl = std::any_cast<mir::StmList*>(succ->info);
                auto succ_label_stm = static_cast<mir::Label*>(succ_sl->front());
                if(succ_label_stm->label == cjumpstm->true_label){
                    visited.clear();
                    node_parent.clear();
                    auto next_node = next_active_node(succ, this_node);

                    if(node_parent[next_node] != this_node){
                        auto next_node_pred = node_parent[next_node];
                        cjumpstm->true_label = static_cast<mir::Label*>(get_label_stm(next_node_pred))->label;
                        origin_pred_to_new_pred[next_node_pred].push_back(make_pair(node_parent[next_node_pred], this_node));
                        new_succs[this_node].insert(next_node_pred);
                    }
                    new_succs[node_parent[next_node]].insert(next_node);   
                    worklist.push_back(next_node);                 

                }else{
                    visited.clear();
                    node_parent.clear();
                    auto next_node = next_active_node(succ, this_node);

                    if(node_parent[next_node] != this_node){
                        auto next_node_pred = node_parent[next_node];
                        cjumpstm->false_label = static_cast<mir::Label*>(get_label_stm(next_node_pred))->label;
                        origin_pred_to_new_pred[next_node_pred].push_back(make_pair(node_parent[next_node_pred], this_node));
                        new_succs[this_node].insert(next_node_pred);
                    }
                    new_succs[node_parent[next_node]].insert(next_node);   
                    worklist.push_back(next_node);  

                }

            }
        
        }else{
            Node* next_node = NULL;
            utils::Label* jump_label = NULL;
            for(auto succ: *this_node->succs){
                visited.clear();
                node_parent.clear();
                next_node = next_active_node(succ, this_node);
                if(next_node) break;
            }
            jump_label = static_cast<mir::Label*>(get_label_stm(next_node))->label;
            if(node_parent[next_node] != this_node){
                auto next_node_pred = node_parent[next_node];
                origin_pred_to_new_pred[next_node_pred].push_back(make_pair(node_parent[next_node_pred], this_node));
                new_succs[this_node].insert(next_node_pred);  
                jump_label = static_cast<mir::Label*>(get_label_stm(next_node_pred))->label;     
            }
            new_succs[node_parent[next_node]].insert(next_node);
            worklist.push_back(next_node);  
            stmlist->pop_back();
            stmlist->push_back(new mir::Jump(jump_label));
            
        }

    }
    
    //new bg
    for(auto node: *nodelist){
       node->preds->clear();
       node->succs->clear();
    }

    for(auto node: *nodelist){
        auto succs = new_succs[node];
       for(auto succ: succs){
           bg->addEdge(node, succ);
       }
    }

    //clean phi
    std::cout << "clean phi" << std::endl;
    for(auto node: *nodelist){
       if(node->preds->size() == 1){
            mir::StmList* stmlist = std::any_cast<mir::StmList*>(node->info);
            for(auto &stm: *stmlist){
                if(is_a_phi(stm)){
                    auto movstm = static_cast<mir::Move*>(stm);
                    auto phiexp = static_cast<mir::Phi*>(movstm->src);
                    if(phiexp->temp_const_map.count(phiexp->tl->front())){
                        movstm->src = phiexp->temp_const_map[phiexp->tl->front()]->exp_deepcopy();
                    }else{
                        movstm->src = new mir::Temp(phiexp->tl->front());
                    }
                }
            }
       }
    }

    bg->deadnode_eliminate();
    
};
}//namespace ssa