#include "insert.hpp"
#include <iostream>
using namespace std;

//insert(graph, text)
//Full insertion pipeline for one text entry.
//
//vectorize  — turn text into 300-dim vector
//createNode   — allocate node, push to masterList
//guard     — if first node, just set entry point
//search    — find M closest existing nodes
//addEdge   — connect new node to those closest nodes
// setEntryPoint— new node becomes the start for next search

void insert(NSWGraph* graph, const string& text) {

    //dont insert empty text
    if (text.empty()) {
        cout << "Insert failed: empty text." << endl;
        return;
    }

    //vectorize
    vector<double> vec = vectorize(text);

    //create node and track in masterList 
    Node* newNode = graph->createNode(text, vec);
    cout << "[insert] node created. total nodes: " << graph->getNodeCount() << endl;

    //if this is the very first node, just set entry point
    if (graph->getNodeCount() == 1) {
        graph->setEntryPoint(newNode);
        cout << "[insert] first node. entry point set to: \"" << text << "\"" << endl;
        return;
    }

    //search for M closest existing nodes 
    //search runs BEFORE we update entry point (see note above)
    vector<SearchResult> closest = search(graph, vec, M, EF_SEARCH);
    cout << "[insert] found " << closest.size() << " closest neighbors:" << endl;
    for (int i = 0; i < closest.size(); i++) {
        cout << " [" << i << "] \"" << closest[i].text
             << "\" dist=" << closest[i].distance << endl;
    }

    //connect new node to each closest neighbor 
    //we need the actual Node* pointer, not just the text string
    //so we match by text through getAllNodes()
    const vector<Node*>& allNodes = graph->getAllNodes();
    for (int i = 0; i < closest.size(); i++) {
        for (int j = 0; j < allNodes.size(); j++) {
            if (allNodes[j]->text == closest[i].text && allNodes[j] != newNode) {
                graph->addEdge(newNode, allNodes[j]);
                break;
            }
        }
    }

    cout << "[insert] \"" << text << "\" connected to "
         << newNode->neighbors.size() << " neighbors." << endl;

    //new node becomes entry point for next insertion/search
    graph->setEntryPoint(newNode);
    cout << "[insert] entry point updated to: \"" << text << "\"" << endl;
}