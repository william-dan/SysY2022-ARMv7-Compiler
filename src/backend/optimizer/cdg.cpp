#include "cdg.hpp"
namespace ssa{
using namespace std;
    void  ControlDependenceGraph::build(){
        auto newnode = new Node(this, exitnode->info);
        // cerr << "newnode" << endl;
        nodemap[exitnode] = newnode;
        cdg_to_bg[newnode] = exitnode;
        for(auto node: *nodelist){
            // std::cerr << node->mykey << "here" << exitnode->mykey << std::endl;
            if(node->mykey == exitnode->mykey) {
                // std::cerr << "here to continue" << std::endl;
                continue;
            }
            Node* newnode = new Node(this, node->info);
            // nodelist->emplace_back(newnode);
            nodemap[node] = newnode;
            cdg_to_bg[newnode] = node;
        }
        // cerr << "nodelist" << endl;
        entrynode = new Node(this, nullptr);
        assert(nodelist);
        for(auto node: *nodelist){
            Node* newnode = nodemap[node];
            // assert(newnode);
            // assert(node->preds && node->succs);
            for(auto pred: *(node->preds)){
                // assert(nodemap.count(pred));
                addEdge(newnode, nodemap[pred]);
            }
            for(auto succ: *(node->succs)){
                // if(!nodemap.count(succ)) cerr << succ->mykey << endl;
                // assert(nodemap.count(succ));
                addEdge(nodemap[succ], newnode);
            }
        }
        // cerr << "addEdge" << endl;
        addEdge(nodemap[nodelist->front()], entrynode);
        //TODO: add exit node
        addEdge(nodemap[exitnode], entrynode);

        DominatorTree* domitree = new DominatorTree(mynodes);
        // cerr << "domitree" << endl;
        // DomiTable postdomi = compute_domi(mynodes);

        for(auto n: *mynodes){
            n->preds->clear();
            n->succs->clear();            
        }
        // std::cout << "all edge removed"<<std::endl;
        // cerr << "clear" << endl;
        //using df to build cdg
        for(auto& nodey: *mynodes){
            auto node_df = domitree->dominance_frontier[nodey]; 

            for(auto nodex: node_df){
                addEdge(nodex, nodey);
            }
        }
        // std::cout << "cdg added"<<std::endl;
       // show();
    }

    void ControlDependenceGraph::show(){
        std::cout << "cdg has " << nodecount << " nodes" << std::endl;
        for(auto& n: *mynodes){
            
            std::cout << n->mykey << "size: " << n->succs->size() << ":";
            for(auto& succ: *n->succs){
                std::cout << succ->mykey << " ";
            }
            std::cout << std::endl;
            if(n == entrynode|| n == exitnode) continue;
            // for(auto& s: *std::any_cast<mir::StmList*>(n->info)){
            //     s->print();
            // }
            
            
        }
    }

    class BasicGraph: public cfg::Graph{
    public:
        BasicGraph(){
        };
        void show(){
            std::cout << "basic has " << nodecount << " nodes" << std::endl;
            for(auto& n: *mynodes){
                std::cout << n->mykey << ": ";
                for(auto& succ: *n->succs){
                    std::cout << succ->mykey << " ";
                }
                std::cout << std::endl;
            };
    };
    };
    void test_cdg(){
        BasicGraph* graph = new BasicGraph();
        std::vector<Node*> nodes;
        for(int i = 0; i < 7; i++){
            Node* newnode = new Node(graph, nullptr);
            nodes.push_back(newnode);
        }
        graph->addEdge(nodes[0], nodes[1]);
        graph->addEdge(nodes[1], nodes[2]);
        graph->addEdge(nodes[1], nodes[3]);
        graph->addEdge(nodes[2], nodes[4]);
        graph->addEdge(nodes[2], nodes[5]);
        graph->addEdge(nodes[4], nodes[6]);
        graph->addEdge(nodes[5], nodes[6]);
        graph->addEdge(nodes[6], nodes[1]);
        
        Node* exitnode = new Node(graph, nullptr);
        graph->addEdge(nodes[3], exitnode);
        graph->show();
        auto cdg = new ControlDependenceGraph(graph->mynodes, exitnode);
    };
}
