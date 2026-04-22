// ============================================================
// nsw_graph.cpp — Navigable Small World Graph (Implementation)
// Owner: Member 2 (Graph Structure & Memory Specialist)
//
// SKELETON CODE — Member 2 must implement the TODO sections.
// Each function has detailed comments explaining the logic.
//
// DELETION: removeNode() is FULLY IMPLEMENTED below.
//           Member 2 only needs to fill the 3 original TODOs.
// ============================================================

#include "nsw_graph.h"

// ------------------------------------------------------------
// Constructor
// Initialize an empty graph with no nodes.
// ------------------------------------------------------------
NSWGraph::NSWGraph() {
    entryPoint = nullptr;
    nextId = 0;
    // masterList is already empty (default-initialized by vector)
}

// ------------------------------------------------------------
// Destructor
// Free all dynamically allocated nodes to prevent memory leaks.
//
// IMPORTANT: In C++, every 'new' must have a matching 'delete'.
// We keep a masterList of all Node pointers so that when the
// NSWGraph object is destroyed (e.g., program ends), all memory
// is properly freed. This includes lazily-deleted nodes.
//
// Viva Defense:
//   "Since we use 'new Node()', we must manually 'delete'.
//    We keep a masterList in the class to ensure we can free
//    all memory when the program ends, preventing leaks."
// ------------------------------------------------------------
NSWGraph::~NSWGraph() {
    // TODO: Member 2 — Loop through masterList and delete each Node*
    // Hint: use a simple for loop
    //
    // for (Node* node : masterList) {
    //     delete node;
    // }
    // masterList.clear();
}

// ------------------------------------------------------------
// createNode(text, vec)
// Allocate a new Node on the heap, add it to masterList,
// and return a raw pointer to it.
//
// This is the ONLY way nodes should be created in the project.
// Member 4's insert() calls this function.
//
// Each node gets a unique auto-incremented ID starting from 0.
// ------------------------------------------------------------
Node* NSWGraph::createNode(const string& text, const vector<double>& vec) {
    // TODO: Member 2 — Allocate a new Node and track it
    //
    // Node* newNode = new Node(nextId, text, vec);
    // masterList.push_back(newNode);
    // nextId++;
    // return newNode;
    return nullptr; // Placeholder — replace with actual implementation
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
    // TODO: Member 2 — Implement bidirectional edge creation
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
// removeNode(node) — LAZY DELETION
// Mark a node as deleted without physically removing it.
//
// HOW IT WORKS:
//   1. Set node->is_deleted = true
//   2. The node's edges, text, and vector remain in memory
//   3. Search skips deleted nodes automatically
//   4. The entry point is reassigned if it was the deleted node
//
// WHY LAZY DELETION (not hard delete):
//   - Physically removing a node requires rewiring all its
//     neighbors' connections to maintain graph navigability.
//   - This is complex and can disconnect parts of the graph.
//   - Lazy deletion is the same approach used by hnswlib
//     (the reference HNSW implementation by Malkov et al.).
//   - The deleted node's edges still serve as "bridges" —
//     the search can traverse THROUGH it to reach other nodes,
//     but it won't return it as a result.
//
// TRADE-OFFS:
//   Pro: Simple to implement, graph stays connected
//   Con: Deleted nodes waste memory and edge slots
//   Con: If many nodes are deleted, search quality may degrade
//   Con: The deleted node still appears in neighbor lists
//
// Viva Defense:
//   "We use lazy deletion — marking the node as deleted rather
//    than physically removing it. This is the same approach used
//    by hnswlib. The node's edges remain so the graph stays
//    connected and traversable, but search skips deleted nodes
//    when returning results. Hard deletion with neighbor rewiring
//    exists in research (IPGM 2022, Mishra & Naamad 2025) but is
//    significantly more complex and beyond the scope of this project."
// ------------------------------------------------------------
bool NSWGraph::removeNode(Node* node) {
    // Guard: null pointer
    if (node == nullptr) return false;

    // Guard: already deleted
    if (node->is_deleted) return false;

    // Mark the node as deleted
    node->is_deleted = true;

    // NOTE: We do NOT remove the node from neighbors' lists.
    // The edges stay intact so the graph remains navigable.
    // Search checks node->is_deleted and skips it in results.

    // If the entry point was just deleted, find a new one.
    // Search backwards through masterList for the most recent
    // active (non-deleted) node.
    if (entryPoint == node) {
        entryPoint = nullptr;
        for (int i = static_cast<int>(masterList.size()) - 1; i >= 0; i--) {
            if (!masterList[i]->is_deleted) {
                entryPoint = masterList[i];
                break;
            }
        }
        // If all nodes are deleted, entryPoint stays nullptr.
        // getEntryPoint() and search() handle this gracefully.
    }

    return true;
}

// ------------------------------------------------------------
// getEntryPoint()
// Return the current entry point for search.
// If the entry point was deleted, automatically reassigns
// to the most recent active node.
// Returns nullptr if no active nodes exist.
// ------------------------------------------------------------
Node* NSWGraph::getEntryPoint() {
    // If entry point is null or was deleted, find a new one
    if (entryPoint == nullptr || entryPoint->is_deleted) {
        entryPoint = nullptr;
        for (int i = static_cast<int>(masterList.size()) - 1; i >= 0; i--) {
            if (!masterList[i]->is_deleted) {
                entryPoint = masterList[i];
                break;
            }
        }
    }
    return entryPoint;
}

// ------------------------------------------------------------
// setEntryPoint(node)
// Update the entry point to a new node.
// Called by Member 4's insert() after creating a new node.
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
// Return the total number of nodes ever created (includes deleted).
// ------------------------------------------------------------
int NSWGraph::getNodeCount() const {
    return static_cast<int>(masterList.size());
}

// ------------------------------------------------------------
// getActiveNodeCount()
// Return the number of active (non-deleted) nodes.
// ------------------------------------------------------------
int NSWGraph::getActiveNodeCount() const {
    int count = 0;
    for (Node* node : masterList) {
        if (!node->is_deleted) count++;
    }
    return count;
}

// ------------------------------------------------------------
// getDeletedNodeCount()
// Return the number of deleted (lazy-deleted) nodes.
// ------------------------------------------------------------
int NSWGraph::getDeletedNodeCount() const {
    int count = 0;
    for (Node* node : masterList) {
        if (node->is_deleted) count++;
    }
    return count;
}

// ------------------------------------------------------------
// getAllNodes()
// Return a const reference to the master list.
// Used by Member 4 for brute-force baseline comparison.
// NOTE: Includes deleted nodes. Check node->is_deleted.
// ------------------------------------------------------------
const vector<Node*>& NSWGraph::getAllNodes() const {
    return masterList;
}

// ------------------------------------------------------------
// findNodeById(id)
// Find a node by its unique ID.
// Returns nullptr if not found.
// Searches through masterList linearly.
// ------------------------------------------------------------
Node* NSWGraph::findNodeById(int id) const {
    for (Node* node : masterList) {
        if (node->id == id) return node;
    }
    return nullptr;
}
