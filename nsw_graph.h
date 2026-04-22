#ifndef NSW_GRAPH_H
#define NSW_GRAPH_H

// ============================================================
// nsw_graph.h — Navigable Small World Graph Container
// Owner: Member 2 (Graph Structure & Memory Specialist)
//
// The NSWGraph class is the main container that holds all nodes
// and provides methods to manage the graph structure.
//
// Member 4's insert() function will call createNode() and addEdge().
// Member 3's search() will access the graph via getEntryPoint()
// and node->neighbors to navigate the graph.
//
// DELETION:
//   Lazy deletion is supported via removeNode().
//   The node is marked as deleted but stays in memory with its edges.
//   Search automatically skips deleted nodes.
// ============================================================

#include "node.h"
#include "config.h"

class NSWGraph {
private:
    // Master list of ALL nodes ever created (including deleted ones).
    // Used by the destructor to free memory and prevent leaks.
    vector<Node*> masterList;

    // The starting node for every search operation.
    // Updated to the newest node after each insertion.
    // If the entry point gets deleted, it is reassigned to the
    // most recent active node.
    Node* entryPoint;

    // Auto-incrementing ID counter for uniquely identifying nodes.
    // Every new node gets the next ID. Never decreases.
    int nextId;

public:
    // Constructor: initialize empty graph
    NSWGraph();

    // Destructor: loop through masterList and delete every node.
    // CRITICAL for preventing memory leaks in C++.
    // Frees ALL nodes, including lazily-deleted ones.
    ~NSWGraph();

    // Allocate a new Node, add it to masterList, and return a pointer.
    // This ensures ALL nodes are tracked for cleanup by the destructor.
    // Auto-assigns a unique ID via nextId.
    Node* createNode(const string& text, const vector<double>& vec);

    // Create a bidirectional edge between two nodes.
    // Adds 'b' to 'a's neighbor list and 'a' to 'b's neighbor list.
    // Enforces the max degree limit M (from config.h).
    // If a neighbor list already has M neighbors, the new edge is skipped.
    void addEdge(Node* a, Node* b);

    // --- LAZY DELETION ---
    // Mark a node as deleted (lazy deletion).
    // The node is NOT physically removed from the graph.
    // Its edges and data remain intact in memory.
    // Search will automatically skip this node.
    // Returns true if successfully marked, false if already deleted
    // or node pointer is invalid.
    //
    // Viva Defense:
    //   "We use lazy deletion — marking the node as deleted rather
    //    than physically removing it. This is the same approach used
    //    by hnswlib (the original HNSW library by Malkov et al.).
    //    The node's edges remain so the graph stays connected,
    //    but search skips it in results. This avoids the complexity
    //    of rewiring neighbor connections."
    bool removeNode(Node* node);

    // Get the current entry point (starting node for search).
    // Returns nullptr if the graph is empty or all nodes are deleted.
    // If the current entry point was deleted, this automatically
    // reassigns to the most recent active node.
    Node* getEntryPoint();

    // Update the entry point to a new node.
    // Called by Member 4's insert() after adding a new node.
    void setEntryPoint(Node* node);

    // Return the total number of nodes ever created (including deleted).
    int getNodeCount() const;

    // Return the number of active (non-deleted) nodes.
    int getActiveNodeCount() const;

    // Return the number of deleted nodes.
    int getDeletedNodeCount() const;

    // Return a const reference to the master list.
    // Used by Member 4 for brute-force baseline comparison.
    // NOTE: This includes deleted nodes — brute force should
    //       check node->is_deleted before comparing.
    const vector<Node*>& getAllNodes() const;

    // Find a node by its ID.
    // Returns nullptr if not found.
    Node* findNodeById(int id) const;
};

#endif // NSW_GRAPH_H
