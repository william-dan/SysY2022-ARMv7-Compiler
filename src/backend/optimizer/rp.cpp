#include "rp.hpp"


namespace ssa{
using namespace mir;




ReplicatePropagation::ReplicatePropagation(BlockGraph* bg, bool final_do): bg(bg), final_do(final_do)
{
    if(bg->nodecount > 500) return;
    

   

    for(auto& n: *bg->mynodes){
        single_node_propagate_bg(n);
    }
    bg->Liveness();
    for(auto& n: *bg->mynodes){
        single_node_clean_bg(n);
    }
    bg->clearLiveness();
}
ReplicatePropagation::ReplicatePropagation(FlowGraph* fg): fg(fg)
{
    for(auto& n: *fg->mynodes){
        single_node_propagate_fg(n);
    }
}

static mir::Const* calbinop(mir::Const* lhs, mir::BinOp op, mir::Const* rhs) {
    switch (op) {
    case mir::BinOp::PLUS:
        if (lhs->type == utils::TempType::INT && rhs->type == utils::TempType::INT) {return new mir::Const(lhs->n.i + rhs->n.i);}
        if (lhs->type == utils::TempType::INT && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.i + rhs->n.f);
        if (!(lhs->type == utils::TempType::INT) && rhs->type == utils::TempType::INT) return new mir::Const(lhs->n.f + rhs->n.i);
        if (!(lhs->type == utils::TempType::INT) && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.f + rhs->n.f);
        break;
    case mir::BinOp::MINUS:
        if (lhs->type == utils::TempType::INT && rhs->type == utils::TempType::INT) {return new mir::Const(lhs->n.i - rhs->n.i);}
        if (lhs->type == utils::TempType::INT && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.i - rhs->n.f);
        if (!(lhs->type == utils::TempType::INT) && rhs->type == utils::TempType::INT) return new mir::Const(lhs->n.f - rhs->n.i);
        if (!(lhs->type == utils::TempType::INT) && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.f - rhs->n.f);
        break;
    case mir::BinOp::MUL:
        if (lhs->type == utils::TempType::INT && rhs->type == utils::TempType::INT) {return new mir::Const(lhs->n.i * rhs->n.i);}
        if (lhs->type == utils::TempType::INT && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.i * rhs->n.f);
        if (!(lhs->type == utils::TempType::INT) && rhs->type == utils::TempType::INT) return new mir::Const(lhs->n.f * rhs->n.i);
        if (!(lhs->type == utils::TempType::INT) && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.f * rhs->n.f);
        break;
    case mir::BinOp::DIV:
        if (lhs->type == utils::TempType::INT && rhs->type == utils::TempType::INT) {
            // return nullptr;
            if(lhs->n.i % rhs->n.i){
                return nullptr;
            }
            return new mir::Const(lhs->n.i / rhs->n.i);
        }
        if (lhs->type == utils::TempType::INT && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.i / rhs->n.f);
        if (!(lhs->type == utils::TempType::INT) && rhs->type == utils::TempType::INT) return new mir::Const(lhs->n.f / rhs->n.i);
        if (!(lhs->type == utils::TempType::INT) && !(rhs->type == utils::TempType::INT)) return new mir::Const(lhs->n.f / rhs->n.f);
        break;
    case mir::BinOp::MOD:
        assert(0);
        if (lhs->type == utils::TempType::INT && rhs->type == utils::TempType::INT) return new mir::Const(lhs->n.i % rhs->n.i);
        break;
    default:
        assert(0);
        break;
    }
    return new::mir::Const(0);
}

static BinOp notBop(BinOp op){
    switch (op)
    {
    case BinOp::PLUS:
    return BinOp::MINUS;
    case BinOp::MINUS:
    return BinOp::PLUS;
    case BinOp::MUL:
    return BinOp::DIV;
    case BinOp::DIV:
    return BinOp::MUL;
    default:
    assert(0);
    break;
    }
}



bool ReplicatePropagation::try_merge(Stm* s, Exp* after){
    switch (after->kind)
    {
    case ExpType::TEMP:
    {
        if(s->kind != StmType::MOVE) return true;
        assert(s->kind == StmType::MOVE);
        auto mov = static_cast<Move*>(s);
        if(mov->src->kind == mir::ExpType::BINOP){
            auto bop = static_cast<Binop*>(mov->src);
            if(bop->left->kind == mir::ExpType::TEMP && bop->right->kind == mir::ExpType::TEMP){
                auto ltemp = static_cast<mir::Temp*>(bop->left)->temp;
                auto rtemp = static_cast<mir::Temp*>(bop->right)->temp;
                if(ltemp == rtemp){
                    switch (bop->op)
                    {
                    case BinOp::PLUS:
                    {
                        bop->op = BinOp::MUL;
                        if(ltemp->type == utils::TempType::INT) bop->right = new Const(2);
                        else if(ltemp->type == utils::TempType::FLOAT) bop->right = new Const((float)(2.0));

                    }
                    break;
                    case BinOp::MINUS:
                    {
                        
                        if(ltemp->type == utils::TempType::INT) mov->src = new Const(0);
                        else if(ltemp->type == utils::TempType::FLOAT) mov->src = new Const((float)(0));
                    }
                    break;
                    case BinOp::DIV:
                    {
                        
                        if(ltemp->type == utils::TempType::INT) mov->src = new Const(1);
                        else if(ltemp->type == utils::TempType::FLOAT) mov->src = new Const((float)(1));
                    }
                    break;
                    default:
                    break;
                    }
                }
            }
        }
    }
    return true;
    case ExpType::CALL:
    return false;
    case ExpType::MEM:
    return false;
    default:
    break;
    }
    assert(after->kind == ExpType::BINOP);
    if(s->kind != StmType::MOVE) return final_do;
    assert(s->kind == StmType::MOVE);
    auto mov = static_cast<Move*>(s);
    switch (mov->src->kind)
    {
    case ExpType::BINOP:
    {
        auto bop = static_cast<Binop*>(mov->src);
        assert(!(bop->left->kind == ExpType::BINOP && bop->right->kind == ExpType::BINOP));
        if(bop->left->kind == ExpType::BINOP){
            auto lbop = static_cast<Binop*>(bop->left);
            if(bop->right->kind != ExpType::CONST){
                assert(bop->right->kind == ExpType::TEMP);
                auto rtemp = static_cast<Temp*>(bop->right);
                //(c+a)+a
                if(lbop->left->kind == ExpType::CONST){
                    auto lbop_lconst = static_cast<Const*>(lbop->left);
                    auto lbop_rtemp = static_cast<Temp*>(lbop->right);
                    if(lbop_rtemp->temp == rtemp->temp){
                        //(c+a)-a
                        if((lbop->op == BinOp::PLUS && bop->op == BinOp::MINUS)
                        || (lbop->op == BinOp::MINUS && bop->op == BinOp::PLUS)
                        || (lbop->op == BinOp::DIV && bop->op == BinOp::MUL)
                        || (lbop->op == BinOp::MUL && bop->op == BinOp::DIV)){
                            mov->src = lbop->left;
                            return true;
                        }else if(lbop->op == BinOp::MUL && bop->op == BinOp::PLUS){
                            if(lbop_lconst->type == utils::TempType::INT) lbop_lconst->n.i +=1;
                            else if(lbop_lconst->type == utils::TempType::FLOAT) lbop_lconst->n.f += 1;
                            else assert(0);
                            mov->src = lbop;
                            return true;
                        }else if(lbop->op == BinOp::MUL && bop->op == BinOp::MINUS){
                            if(lbop_lconst->type == utils::TempType::INT) lbop_lconst->n.i -=1;
                            else if(lbop_lconst->type == utils::TempType::FLOAT) lbop_lconst->n.f -= 1;
                            else assert(0);
                            mov->src = lbop;
                            return true;
                        }
                    }
                //(a+c)+a
                }else if(lbop->right->kind == ExpType::CONST){
                    auto lbop_rconst = static_cast<Const*>(lbop->right);
                    auto lbop_ltemp = static_cast<Temp*>(lbop->left);
                    if(lbop_ltemp->temp == rtemp->temp){
                        if(((lbop->op == BinOp::PLUS || lbop->op == BinOp::MINUS) && bop->op == BinOp::MINUS)
                        || (lbop->op == BinOp::MUL && bop->op == BinOp::DIV)){
                            mov->src = lbop->right;
                            return true;
                        }else if(lbop->op == BinOp::DIV && bop->op == BinOp::DIV){
                            if(lbop_rconst->type == utils::TempType::INT) mov->src = new Const((int)(1/lbop_rconst->n.i));
                            else if(lbop_rconst->type == utils::TempType::FLOAT) mov->src = new Const((float)(1.0/lbop_rconst->n.f));
                            else assert(0);
                            
                            return true;
                        }else if(lbop->op == BinOp::MUL && bop->op == BinOp::PLUS){
                            if(lbop_rconst->type == utils::TempType::INT) lbop_rconst->n.i +=1;
                            else if(lbop_rconst->type == utils::TempType::FLOAT) lbop_rconst->n.f += 1;
                            else assert(0);
                            mov->src = lbop;
                            return true;
                        }else if(lbop->op == BinOp::MUL && bop->op == BinOp::MINUS){
                            if(lbop_rconst->type == utils::TempType::INT) lbop_rconst->n.i -=1;
                            else if(lbop_rconst->type == utils::TempType::FLOAT) lbop_rconst->n.f -= 1;
                            else assert(0);
                            mov->src = lbop;
                            return true;
                        }
                    }
                }
                return false;
            }
            auto rconst = static_cast<Const*>(bop->right);
            
            if(!(((lbop->op == BinOp::PLUS || lbop->op == BinOp::MINUS) && (bop->op == BinOp::PLUS || bop->op == BinOp::MINUS))
            || ((lbop->op == BinOp::MUL) && (bop->op == BinOp::MUL)) )) return false;
            mir::Const* newconst = nullptr;
            //(c+a)+c
            if(lbop->left->kind == ExpType::CONST){
                newconst = calbinop(static_cast<Const*>(lbop->left), bop->op, rconst);
                if(!newconst) return false;
                lbop->left = newconst;
                mov->src = lbop;
            //(a+c)+c
            }else if(lbop->right->kind == ExpType::CONST){
                auto op = bop->op;
                if(lbop->op == BinOp::MINUS || lbop->op == BinOp::DIV){
                    op = notBop(op);
                }   
                newconst = calbinop(static_cast<Const*>(lbop->right), op, rconst);
                if(!newconst) return false;
                lbop->right = newconst;
                mov->src = lbop;
            }else{
                return false;
            }
            
        }else if(bop->right->kind == ExpType::BINOP){
            auto rbop = static_cast<Binop*>(bop->right);

            if(bop->left->kind != ExpType::CONST){
                assert(bop->left->kind == ExpType::TEMP);
                auto ltemp = static_cast<Temp*>(bop->left);
                //a+(c+a)
                
                if(rbop->left->kind == ExpType::CONST){
                    auto rbop_lconst = static_cast<Const*>(rbop->left);
                    auto rbop_rtemp = static_cast<Temp*>(rbop->right);
                    if(rbop_rtemp->temp == ltemp->temp){
                        //a+(c+a)
                        if((rbop->op == BinOp::MINUS && bop->op == BinOp::PLUS)
                        || (rbop->op == BinOp::DIV && bop->op == BinOp::MUL)){
                            mov->src = rbop->left;
                            return true;
                        }else if(rbop->op == BinOp::PLUS && bop->op == BinOp::MINUS){
                            if(rbop_lconst->type == utils::TempType::INT) rbop_lconst->n.i *= -1;
                            else if(rbop_lconst->type == utils::TempType::FLOAT) rbop_lconst->n.f *= -1;
                            else assert(0);
                            mov->src = rbop->left;
                            return true;
                        
                        }else if(rbop->op == BinOp::MUL && bop->op == BinOp::PLUS){
                            if(rbop_lconst->type == utils::TempType::INT) rbop_lconst->n.i += 1;
                            else if(rbop_lconst->type == utils::TempType::FLOAT) rbop_lconst->n.f += 1;
                            else assert(0);
                            mov->src = rbop;
                            return true;
                        }else if(rbop->op == BinOp::MUL && bop->op == BinOp::MINUS){
                            if(rbop_lconst->type == utils::TempType::INT) rbop_lconst->n.i = 1 - rbop_lconst->n.i;
                            else if(rbop_lconst->type == utils::TempType::FLOAT) rbop_lconst->n.f = 1.0 - rbop_lconst->n.f;
                            else assert(0);
                            mov->src = rbop;
                            return true;
                        }
                    }
                //a+(a+c)
                }else if(rbop->right->kind == ExpType::CONST){
                    auto rbop_rconst = static_cast<Const*>(rbop->right);
                    auto rbop_ltemp = static_cast<Temp*>(rbop->left);
                    if(rbop_ltemp->temp == ltemp->temp){
                        if(((rbop->op == BinOp::PLUS || rbop->op == BinOp::MINUS) && bop->op == BinOp::MINUS)
                        || (rbop->op == BinOp::DIV && bop->op == BinOp::DIV)){
                            if(rbop_rconst->type == utils::TempType::INT) rbop_rconst->n.i *= -1;
                            else if(rbop_rconst->type == utils::TempType::FLOAT) rbop_rconst->n.f *= -1;
                            else assert(0);
                            mov->src = rbop->right;
                            return true;
                        }else if(rbop->op == BinOp::MUL && bop->op == BinOp::DIV){
                            if(rbop_rconst->type == utils::TempType::INT) mov->src = new Const((int)(1/rbop_rconst->n.i));
                            else if(rbop_rconst->type == utils::TempType::FLOAT) mov->src = new Const((float)(1.0/rbop_rconst->n.f));
                            else assert(0);

                            return true;
                        }else if(rbop->op == BinOp::MUL && bop->op == BinOp::PLUS){
                            if(rbop_rconst->type == utils::TempType::INT) rbop_rconst->n.i += 1;
                            else if(rbop_rconst->type == utils::TempType::FLOAT) rbop_rconst->n.f += 1;
                            else assert(0);
                            mov->src = rbop;
                            return true;
                        }else if(rbop->op == BinOp::MUL && bop->op == BinOp::MINUS){
                            if(rbop_rconst->type == utils::TempType::INT) rbop_rconst->n.i -= 1;
                            else if(rbop_rconst->type == utils::TempType::FLOAT) rbop_rconst->n.f -= 1;
                            else assert(0);
                            mov->src = rbop;
                            return true;
                        }
                    }
                }

                return false;
            }
            auto lconst = static_cast<Const*>(bop->left);
            if(!(((rbop->op == BinOp::PLUS || rbop->op == BinOp::MINUS) && (bop->op == BinOp::PLUS || bop->op == BinOp::MINUS))
            || (rbop->op == BinOp::MUL) && (bop->op == BinOp::MUL))) return false;
            mir::Const* newconst = nullptr;
            //c + (c + a)
            if(rbop->left->kind == ExpType::CONST){
                newconst = calbinop(lconst, bop->op, static_cast<Const*>(rbop->left));
                if(!newconst) return false;
                rbop->left = newconst;
                if(bop->op == BinOp::MINUS || bop->op == BinOp::DIV){
                    rbop->op = notBop(rbop->op);
                }
                mov->src = rbop;
            //c + (a + c)

            }else if(rbop->right->kind == ExpType::CONST){
                auto op = rbop->op;
                if(bop->op == BinOp::MINUS || bop->op == BinOp::DIV){
                    op = notBop(op);
                }   
                newconst = calbinop(lconst, op, static_cast<Const*>(rbop->right));
                if(!newconst) return false;
                bop->left = newconst;
                bop->right = rbop->left;
            }else{
                return false;
            }
        }
        return true;
    }
    break;
    // case ExpType::
    default:
    return final_do;
        break;
    }

}


void ReplicatePropagation::single_node_propagate_bg(Node* node){
    assert(bg);
    
    for(auto& s: *bg->get_info(node)){
        auto defs = s->get_def();
        if(!defs.empty()){
            assert(defs.size() == 1);
            auto def = *defs.begin();
            
            int i = 0;
            auto use_refs = s->get_use_ref();
            // cout << def->to_string() << " has " << endl;

            for(auto& m_use: use_refs){
                assert((*m_use)->kind == ExpType::TEMP);
                auto use = static_cast<Temp*>(*m_use)->temp;
                // cout << use->to_string() << " ";
                if(temp_curr_val[use].ids.size() == 1){
                    // cout << "use have one use ";
                    assert(temp_curr_val[use].s->kind == StmType::MOVE);
                    auto mov = static_cast<Move*>(temp_curr_val[use].s);
                    auto use_use = *mov->get_use().begin();
                    if(temp_curr_val[use_use].id == temp_curr_val[use].ids[0]){
                        // cout << "possible merge ";
                        auto temp_of_use = *use_refs[i];
                        *use_refs[i] = mov->src->exp_deepcopy();
                        if(try_merge(s, *use_refs[i])){
                            // cout << "can merge ";
                        }else{
                            delete *use_refs[i];
                            *use_refs[i] = temp_of_use;
                        }
                    }
                }
                // cout << endl;
                i++;
            }

            // assert(s->kind == mir::StmType::MOVE);
            // auto mov = static_cast<mir::Move*>(s);
            // if(mov->src->kind == mir::ExpType::BINOP){

            // }

            temp_curr_val[def].s = s;
            use_refs = s->get_use_ref();
            temp_curr_val[def].ids.resize(use_refs.size());
            i = 0;
            for(auto& m_use: use_refs){
                auto use = static_cast<Temp*>(*m_use)->temp;
                temp_curr_val[def].ids[i++] = temp_curr_val[use].id;
            }
            temp_curr_val[def].id++;


        }else{
            int i = 0;
            auto use_refs = s->get_use_ref();
            // cout << def->to_string() << " has " << endl;

            for(auto& m_use: use_refs){
                assert((*m_use)->kind == ExpType::TEMP);
                auto use = static_cast<Temp*>(*m_use)->temp;
                // cout << use->to_string() << " ";
                if(temp_curr_val[use].ids.size() == 1){
                    // cout << "use have one use ";
                    assert(temp_curr_val[use].s->kind == StmType::MOVE);
                    auto mov = static_cast<Move*>(temp_curr_val[use].s);
                    auto use_use = *mov->get_use().begin();
                    if(temp_curr_val[use_use].id == temp_curr_val[use].ids[0]){
                        // cout << "possible merge ";
                        auto temp_of_use = *use_refs[i];
                        *use_refs[i] = mov->src->exp_deepcopy();
                        if(try_merge(s, *use_refs[i])){
                            // cout << "can merge ";
                        }else{
                            delete *use_refs[i];
                            *use_refs[i] = temp_of_use;
                        }
                    }
                }
                // cout << endl;
                i++;
            }
        }
    }
    
    temp_curr_val.clear();
}


void ReplicatePropagation::single_node_clean_bg(Node* node){
    auto sl = bg->get_info(node);
    auto out = bg->liveness_info[node]->out;
    for(auto it = sl->rbegin(); it != sl->rend(); ){
        auto s = *it;
        auto defs = s->get_def();
        if(!defs.empty()){
            assert(defs.size() == 1);
            auto def = *defs.begin();
            auto nxt = it.base();
            assert(nxt != sl->end());
            bool useful = false;

            assert(s->kind == StmType::MOVE);
            auto mov = static_cast<Move*>(s);
            if(out->count(def) || mov->src->kind == ExpType::CALL){
                useful = true;
            }else{
                for(; nxt != sl->end(); nxt++){
                    auto n_s = *nxt;
                    auto uses = n_s->get_use();
                    if(uses.count(def)){
                        useful = true;
                        break;
                    }
                }
            }
            // s->print();
            // cout << "useful: " << useful << endl;
            if(!useful){
                auto iter = sl->erase((++it).base());
                it = mir::StmList::reverse_iterator(iter);
                continue;
            }
            
        }   
        it++;
    }
}


void ReplicatePropagation::single_node_propagate_fg(Node* node){

}

}