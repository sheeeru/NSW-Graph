// ============================================================
// nsw_graph.cpp — Navigable Small World Graph (Implementation)
// Owner: Areeba (Graph Structure & Memory Specialist)
//
// SKELETON CODE — Areeba must implement the TODO sections.
// Each function has detailed comments explaining the logic.
// ============================================================

#include "nsw_graph.h"

// ------------------------------------------------------------
// Constructor
// Initialize an empty graph with no nodes.
// ------------------------------------------------------------
NSWGraph::NSWGraph() {
    entryPoint = nullptr;
    // masterList is already empty (default-initialized by vector)
}

// ------------------------------------------------------------
// Destructor
// Free all dynamically allocated nodes to prevent memory leaks.
//
// IMPORTANT: In C++, every 'new' must have a matching 'delete'.
// We keep a masterList of all Node pointers so that when the
// NSWGraph object is destroyed (e.g., program ends), all memory
// is properly freed.
//
// Viva Defense:
//   "Since we use 'new Node()', we must manually 'delete'.
//    We keep a masterList in the class to ensure we can free
//    all memory when the program ends, preventing leaks."
// ------------------------------------------------------------
NSWGraph::~NSWGraph() {
    // TODO: Areeba — Loop through masterList and delete each Node*
    // Hint: use a simple for loop
    //
    // for (Node* node : masterList) {
    //     delete node;
    // }
    // masterList.clear();
}

// ------------------------------------------------------------
// addEdge(Node* a, Node* b)
// Create a bidirectional (undirected) edge between two nodes.
//
// Steps:
//   1. Add 'b' to 'a's neighbor list
//   2. Add 'a' to 'b's neighbor list
//   3. If either list exceeds M (from config.h), do NOT add.
//      This prevents over-connection which slows down search.
//
// Viva Defense:
//   "Edges are bidirectional pointers. This allows us to traverse
//    from Node A to B and back, which is crucial for the search
//    algorithm to backtrack slightly if needed."
// ------------------------------------------------------------
void NSWGraph::addEdge(Node* a, Node* b) {
    // TODO: Areeba — Implement bidirectional edge creation
    //
    // Guard: don't add edge if either neighbor list is already at max M
    //
    // if (a->neighbors.size() < M) {
    //     a->neighbors.push_back(b);
    // }
    // if (b->neighbors.size() < M) {
    //     b->neighbors.push_back(a);
    // }
}

// ------------------------------------------------------------
// createNode(text, vec)
// Allocate a new Node on the heap, add it to masterList,
// and return a raw pointer to it.
//
// This is the ONLY way nodes should be created in the project.
// Shaheer's insert() calls this function.
// ------------------------------------------------------------
Node* NSWGraph::createNode(const string& text, const vector<double>& vec) {
    // TODO: Areeba — Allocate a new Node and track it
    //
    // Node* newNode = new Node(text, vec);
    // masterList.push_back(newNode);
    // return newNode;
    return nullptr; // Placeholder — replace with actual implementation
}

// ------------------------------------------------------------
// getEntryPoint()
// Return the current entry point for search.
// Returns nullptr if the graph has no nodes yet.
// ------------------------------------------------------------
Node* NSWGraph::getEntryPoint() const {
    return entryPoint;
}

// ------------------------------------------------------------
// setEntryPoint(node)
// Update the entry point to a new node.
// Called by Shaheer's insert() after creating a new node.
//
// Viva Defense:
//   "It is the starting node for the search. We update it to
//    the newest node to ensure recent additions are easily
//    reachable."
// ------------------------------------------------------------
void NSWGraph::setEntryPoint(Node* node) {
    entryPoint = node;
}

// ------------------------------------------------------------
// getNodeCount()
// Return the total number of nodes currently in the graph.
// ------------------------------------------------------------
int NSWGraph::getNodeCount() const {
    return static_cast<int>(masterList.size());
}

// ------------------------------------------------------------
// getAllNodes()
// Return a const reference to the master list.
// Used by Shaheer for brute-force baseline comparison.
// ------------------------------------------------------------
const vector<Node*>& NSWGraph::getAllNodes() const {
    return masterList;
}
