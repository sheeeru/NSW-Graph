#include "nsw_graph.h"
#include <iostream>
#include <vector>
using namespace std;


NSWGraph::NSWGraph() {
    entryPoint = nullptr;
    //default masterList is already empty
}


NSWGraph::~NSWGraph() {// loop and delete all nodes frfom master list
    for (int i=0; i<masterList.size(); i++){
        delete masterList[i];
    }
    masterList.clear();
    entryPoint = nullptr;
    cout << "graph empty " << endl;
}

// -------------------------------------------------------
// printGraph
// -------------------------------------------------------
// Loops through every node in masterList and prints
// its text and the text of each of its neighbors.
// Used for debugging and viva demonstration.

void NSWGraph::printGraph() const {
    if (masterList.empty()) {
        cout << "Graph is empty." << endl;
        return;
    }
    cout << "--- Graph State ---" << endl;
    cout << "Total nodes: " << masterList.size() << endl << endl;

    for (int i = 0; i < masterList.size(); i++) {
        Node* current = masterList[i];

        cout << "[" << i << "] " << current->text << endl;
        cout << "Neighbors (" << current->neighbors.size() << "): ";

        for (int j = 0; j < current->neighbors.size(); j++) {
            cout << current->neighbors[j]->text;
            if (j < current->neighbors.size() - 1) {
                cout << ", "; }
        }
        cout << endl << endl;
    }

    cout << "Entry Point: ";
    if (entryPoint != nullptr) {
        cout << entryPoint->text << endl;
    } else {
        cout << "none" << endl; }
    cout << "-------------------" << endl;
}

//Creates a bidirectional (undirected) edge between two nodes.

//1.Add 'b' to 'a's neighbor list
//2.Add 'a' to 'b's neighbor list
//3.If either list exceeds M (from config.h), do NOT add. (prevents overconnection)

// ------------------------------------------------------------
void NSWGraph::addEdge(Node* a, Node* b) {
    // guard 1: no self-edge
    if (a == b) {
        return;
    }

    // dupecheck if already in enighbor list
    for (int i = 0; i < a->neighbors.size(); i++) {
        if (a->neighbors[i] == b) {
            return; // already connected, skip
        }
    }

    //both ahve size less than M
    if (a->neighbors.size() < M && b->neighbors.size() < M) {
        a->neighbors.push_back(b);
        b->neighbors.push_back(a);
        cout << "Node edge from " << a->text << " to " << b->text << " created" << endl;}
    else 
        cout << "edge addition failed! Size exceeds Max nighbors M = " << M << endl;
}

// ------------------------------------------------------------
// createNode(text, vec)
// Allocate a new Node on the heap, add it to masterList,

// ------------------------------------------------------------
Node* NSWGraph::createNode(const string& text, const vector<double>& vec) {
    
    Node* newNode = new Node(text, vec);
    masterList.push_back(newNode); // this catering addition to the list so that when we delete and destructr gets called ALL nodes delete 
    return newNode;
}

void NSWGraph::deleteNode(Node* target) {
    if (target == nullptr) {
        return; }

    vector<Node*>& nb = target->neighbors;

    //relink neighbors to each other
    //addEdge checks M limit, so no neighbor gets over-connected. 
    for (int i = 0; i < nb.size(); i++) {
        for (int j = i + 1; j < nb.size(); j++) {
            addEdge(nb[i], nb[j]);
        } }

    //remove target from every neighbor's own list
    for (int i = 0; i < nb.size(); i++) {
        vector<Node*>& theirNeighbors = nb[i]->neighbors;
        for (int j = 0; j < theirNeighbors.size(); j++) {
            if (theirNeighbors[j] == target) {
                theirNeighbors.erase(theirNeighbors.begin() + j);
                break; //only appears once because add edge ensures no dupe copies ar made in neighbor list
            }
        }
    }

    //remove from masterList
    for (int i = 0; i < masterList.size(); i++) {
        if (masterList[i] == target) {
            masterList.erase(masterList.begin() + i);
            break;
        }
    }

    //if target was entry point, pick a new one
    if (entryPoint == target) {
        if (!masterList.empty()) {
            entryPoint = masterList[0];
        } else {
            entryPoint = nullptr;
        }
    }

    //free memory
    delete target;
}
// getEntryPoint()
// Return the current entry point for search.
Node* NSWGraph::getEntryPoint() const {
    return entryPoint;
}


// setEntryPoint(node)
// Update the entry point to a currernt node.

void NSWGraph::setEntryPoint(Node* node) {
    entryPoint = node;
}

// getNodeCount()
// Return the total number of nodes currently in the graph.

int NSWGraph::getNodeCount() const {
    return static_cast<int>(masterList.size()); //static cast to keep global count
    //return masterList.size();
}

// getAllNodes()
// Return a const reference to the master list.

const vector<Node*>& NSWGraph::getAllNodes() const {
    return masterList;
}

