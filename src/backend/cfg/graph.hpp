#pragma once
#include <iostream>
#include <list>
#include <any>
#include <unordered_map>
#include <unordered_set>

namespace cfg {
using std::list;
using std::any;
using std::unordered_map;

class Node;
class Graph;
typedef std::list<cfg::Node*> NodeList;
typedef std::unordered_set<cfg::Node*> NodeSet;
NodeList* NodeList_union(NodeList* nl1, NodeList* nl2);
NodeSet* NodeSet_union(NodeSet* ns1, NodeSet* ns2);
class Node {
public:
    Graph* mygraph;
    int mykey;
    NodeSet *succs, *preds;
    any info;
    Node();
    Node(Graph* g, any i);
    ~Node(){
        delete succs;
        delete preds;
    }
    int degree();
    NodeSet* adj();
};
class Graph {
public:
    
    int nodecount;
    NodeList* mynodes;
    Node *entry, *exit;
    Graph();
    virtual ~Graph(){
        for(auto n: *mynodes){
            delete n;
        }
    }
    void addEdge(Node* from, Node* to);
    void rmEdge(Node* from, Node* to);
    bool goesTo(Node* from, Node* to);
    virtual void show()=0;
};



};