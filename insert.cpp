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

    //cout << "\n[insert] inserting: \"" << text << "\"" << endl;

    // step 1: vectorize (Arqish's function)
    vector<double> vec = vectorize(text);
    cout << "[insert] vectorized." << endl;

    // step 2: create node and track in masterList (Areeba's function)
    Node* newNode = graph->createNode(text, vec);
    cout << "[insert] node created. total nodes: " << graph->getNodeCount() << endl;

    // step 3: if this is the very first node, just set entry point
    if (graph->getNodeCount() == 1) {
        graph->setEntryPoint(newNode);
        cout << "[insert] first node. entry point set to: \"" << text << "\"" << endl;
        return;
    }

    // step 4: search for M closest existing nodes (Hira's function)
    // search runs BEFORE we update entry point (see note above)
    vector<SearchResult> closest = search(graph, vec, M, EF_SEARCH);
    cout << "[insert] found " << closest.size() << " closest neighbors:" << endl;
    for (int i = 0; i < closest.size(); i++) {
        cout << "         [" << i << "] \"" << closest[i].text
             << "\" dist=" << closest[i].distance << endl;
    }

    // step 5: connect new node to each closest neighbor (Areeba's function)
    // we need the actual Node* pointer, not just the text string
    // so we match by text through getAllNodes()
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

    // step 6: new node becomes entry point for next insertion/search
    graph->setEntryPoint(newNode);
    cout << "[insert] entry point updated to: \"" << text << "\"" << endl;
}