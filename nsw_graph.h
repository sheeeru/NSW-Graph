#ifndef NSW_GRAPH_H
#define NSW_GRAPH_H

// nsw_graph.h — Navigable Small World Graph Container - work (areeba)
// The NSWGraph class holds all nodes (like tree) and methods for the graph
//insert -> addNode and addedge()
//search.cpp -> getEntryPoint()
//traverese to next neighbor by node-> neighbor

#include "node.h"
#include "config.h"

class NSWGraph {
private:
    vector<Node*> masterList;

    //start node (root)
    //changes to the current new updated node.
    Node* entryPoint;

public:
    //Constructor:
    NSWGraph();

    //Destructor: loop and del all nodes. 
    ~NSWGraph();

    //unweightaed bidirectional edge btw two nodes
    //both in each other's neighbor list
    //each node cna have a max degree M 
    //if a's max degree reached node will not be added in enighbor so skip
    void printGraph() const;
    
    void addEdge(Node* a, Node* b);

    //make new node, add it to masterList, and return a pointer. (cater the deletion case for space freeup)
    Node* createNode(const string& text, const vector<double>& vec);

    //deleted node and relinks teh neighbors
    void deleteNode(Node* target);
    //current entry point if there is else nullptr
    Node* getEntryPoint() const;

    //updates entry after new node insertion
    void setEntryPoint(Node* node);

    //total nodes count
    int getNodeCount() const;

    //const refernce to the masterList
    const vector<Node*>& getAllNodes() const;
};

#endif // NSW_GRAPH_H
