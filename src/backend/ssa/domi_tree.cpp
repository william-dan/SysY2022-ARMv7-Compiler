#include "domi_tree.hpp"
#include <assert.h>

namespace ssa{

void DominatorTree::dfs(Node* p, Node* n){
    if(dfnum.count(n) == 0){
        dfnum[n] = count;
        v[count] = n;
        parent[n] = p;
        count++;
        for(auto succ: *n->succs){
            dfs(n, succ);
        }
    }
}
void DominatorTree::link(Node* p, Node* n){
    ancestor[n] = p;
    best[n] = n;
}

Node* DominatorTree::awls(Node* n){

    auto a = ancestor[n];
    if(ancestor[a]!= NULL){
        auto b = awls(a);
        ancestor[n] = ancestor[a];
        if(dfnum[semi[b]] < dfnum[semi[best[n]]]){
            best[n] = b;
        }
    }
    return best[n];
}

void DominatorTree::build(){
    count = 0;
    dfs(NULL,nodelist->front());
    for(int i = count - 1; i > 0; i--){
        // std::cerr<<"node "<<i<<std::endl;
        Node* s_;
        Node* n = v[i];
        auto p = parent[n];
        auto s = p;
        for(auto pred: *n->preds){
            if(dfnum[pred] <= dfnum[n]) s_ = pred;
            else s_ = semi[awls(pred)];
            if(dfnum[s_] < dfnum[s]) s = s_;
        }
        semi[n] = s;
        bucket[s].insert(n);
        link(p,n);
        for(auto v: bucket[p]){
            auto y = awls(v);
            assert(semi[v] || semi[y]);
            if(semi[y] == semi[v]) idom[v] = p;
            else samedom[v] = y;
        }
        bucket[p].clear();
    }
    for(int i = 1; i < count; i++){
        auto n = v[i];
        if(samedom[n] != NULL) idom[n] = idom[samedom[n]];
    }
    for (int i = 1; i < count; i++) {
        auto n = v[i];
        children[idom[n]].insert(n);
    }
}

Node* DominatorTree::find_root(Node* n) {
    if (ancestor[n] == n) return n;
    ancestor[n] = find_root(ancestor[n]);
    return ancestor[n];
}

void DominatorTree::get_dominance_frontier(Node* n) {
    for (auto succ : *n->succs) {
        if (idom[succ] != n) dominance_frontier[n].insert(succ);
    }
    for (auto succ : children[n]) {
        get_dominance_frontier(succ);
        ancestor[succ] = n;
    }
    for (auto succ : children[n]) {
        for (auto w : dominance_frontier[succ]) {
            if (n == w || find_root(w) != n) { dominance_frontier[n].insert(w); }
        }
    }
}

void DominatorTree::compute_depth(Node* n, int depth){
    if(domi_depth.count(n) == 0){
        domi_depth[n] = depth;
        for(auto succ: children[n]){
            compute_depth(succ, depth+1);
        }
    }
}

}