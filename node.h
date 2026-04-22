#ifndef NODE_H
#define NODE_H

// ============================================================
// node.h — Graph Node Structure
// Owner: Member 2 (Graph Structure & Memory Specialist)
//
// Defines a single node in the NSW graph.
// Each node stores:
//   - A unique integer ID (auto-assigned by the graph)
//   - The original text string
//   - Its numerical vector (output of Member 1's vectorize())
//   - A list of pointers to neighboring nodes (adjacency list)
//   - A deletion flag (for lazy deletion support)
// ============================================================

#include <string>
#include <vector>

using namespace std;

struct Node {
    // Unique ID assigned by NSWGraph::createNode()
    // Used to identify nodes for deletion and display
    int id;

    // Original note text that the user inserted
    string text;

    // Numerical "fingerprint" vector — output of Member 1's vectorize()
    vector<double> numericalVector;

    // Adjacency list — pointers to connected nodes in the graph
    vector<Node*> neighbors;

    // Lazy deletion flag.
    // true  = node has been "deleted" (search skips it)
    // false = node is active and searchable
    // The node's data and edges still physically exist in memory.
    bool is_deleted;

    // Constructor: initializes a node with ID, text, and its vector
    Node(int nodeId, string t, vector<double> vec)
        : id(nodeId), text(t), numericalVector(vec), is_deleted(false) {}
};

#endif // NODE_H
