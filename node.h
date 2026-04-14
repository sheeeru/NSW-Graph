#ifndef NODE_H
#define NODE_H

// ============================================================
// node.h — Graph Node Structure
// Owner: Member 2 (Graph Structure & Memory Specialist)
//
// Defines a single node in the NSW graph.
// Each node stores:
//   - The original text string
//   - Its numerical vector (output of Member 1's vectorize())
//   - A list of pointers to neighboring nodes (adjacency list)
// ============================================================

#include <string>
#include <vector>

using namespace std;

struct Node {
    // Original note text that the user inserted
    string text;

    // Numerical "fingerprint" vector — output of Member 1's vectorize()
    vector<double> numericalVector;

    // Adjacency list — pointers to connected nodes in the graph
    vector<Node*> neighbors;

    // Constructor: initializes a node with text and its vector
    Node(string t, vector<double> vec)
        : text(t), numericalVector(vec) {}
};

#endif // NODE_H
