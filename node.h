#ifndef NODE_H
#define NODE_H

// node.h — Graph Node Structure - Areeba part
//
// Defines a single node in the NSW graph.
// Each node stores:
// The original text string
// Its numerical vector (output from vectorize)
// A list of pointers to neighboring nodes (adjacency list)

#include <string>
#include <vector>

using namespace std;

class Node{
    // user input for that node (a word)
    public:
        string text;

        //vectorized  output of the text
        vector<double> numericalVector;

        //adjacency list of neighbors of node
        vector<Node*> neighbors;

        // Constructor: initializes a node with text and its vector
        Node(const string& inputText, const vector<double>& inputVector) {
        text = inputText;
        numericalVector = inputVector;
    }
};

#endif 
