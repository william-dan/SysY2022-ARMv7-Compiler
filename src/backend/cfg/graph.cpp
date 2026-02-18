#include <cassert>
#include <algorithm>
#include "graph.hpp"


namespace cfg {
using std::find;
using std::cout, std::endl;

NodeList* NodeList_union(NodeList* nl1, NodeList* nl2){
    NodeList* ret = new NodeList(*nl1);
    for(auto& n2: *nl2){
        bool found = false;
        for(auto& n1: *nl1){
            if(n1 == n2){
                found = true;
                break;
            }
        }
        if(!found) ret->emplace_back(n2);
    }
    return ret;
}
NodeSet* NodeSet_union(NodeSet* ns1, NodeSet* ns2){
    auto ns = new NodeSet();
    ns->merge(*ns1);
    ns->merge(*ns2);
    delete ns1;
    delete ns2;
    return ns;
}




Node::Node(Graph* g, any i){
    assert(g);
    mygraph = g;
    mykey = g->nodecount++;
    g->mynodes->emplace_back(this);
    succs = new NodeSet();
    preds = new NodeSet();
    info = i;
}

int Node::degree(){
    return succs->size() + preds->size();
}

NodeSet* Node::adj(){
    auto nset = new NodeSet();
    nset->merge(*succs);
    nset->merge(*preds);
    return nset;
}

Graph::Graph(){
    nodecount = 0;
    mynodes = new NodeList();
    entry = exit = nullptr;
}

void Graph::addEdge(Node* from, Node* to){
    // cout << "addedge in" << endl;
    assert(from->mygraph == to->mygraph);
    if(goesTo(from, to)) return;
    to->preds->emplace(from);
    from->succs->emplace(to);
    // cout << "addedge out" << endl;
}
void Graph::rmEdge(Node* from, Node* to){
    // cout << "rmedge in" << endl;
    assert(to && to->preds && from && from->succs);
    to->preds->erase(from);
    from->succs->erase(to);
    // cout << "rmedge out" << endl;
}
bool Graph::goesTo(Node* from, Node* to){
    return from->succs->count(to) > 0;
}


};