#ifndef NSW_GRAPH_H
#define NSW_GRAPH_H

// ============================================================
// nsw_graph.h — Navigable Small World Graph Container
// Owner: Areeba (Graph Structure & Memory Specialist)
//
// The NSWGraph class is the main container that holds all nodes
// and provides methods to manage the graph structure.
//
// Shaheer's insert() function will call addNode() and addEdge().
// Hira's search() will access the graph via getEntryPoint()
// and node->neighbors to navigate the graph.
// ============================================================

#include "node.h"
#include "config.h"

class NSWGraph {
private:
    // Master list of ALL nodes ever created.
    // Used by the destructor to free memory and prevent leaks.
    vector<Node*> masterList;

    // The starting node for every search operation.
    // Updated to the newest node after each insertion.
    Node* entryPoint;

public:
    // Constructor: initialize empty graph
    NSWGraph();

    // Destructor: loop through masterList and delete every node.
    // CRITICAL for preventing memory leaks in C++.
    ~NSWGraph();

    // Create a bidirectional edge between two nodes.
    // Adds 'b' to 'a's neighbor list and 'a' to 'b's neighbor list.
    // Enforces the max degree limit M (from config.h).
    // If a neighbor list already has M neighbors, the new edge is skipped.
    void addEdge(Node* a, Node* b);

    // Allocate a new Node, add it to masterList, and return a pointer.
    // This ensures ALL nodes are tracked for cleanup by the destructor.
    Node* createNode(const string& text, const vector<double>& vec);

    // Get the current entry point (starting node for search).
    // Returns nullptr if the graph is empty.
    Node* getEntryPoint() const;

    // Update the entry point to a new node.
    // Called by Shaheer's insert() after adding a new node.
    void setEntryPoint(Node* node);

    // Return the total number of nodes in the graph.
    int getNodeCount() const;

    // Return a const reference to the master list.
    // Used by Shaheer for brute-force baseline comparison.
    const vector<Node*>& getAllNodes() const;
};

#endif // NSW_GRAPH_H
