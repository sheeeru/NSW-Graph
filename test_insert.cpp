// test_insert.cpp — Insertion Test File
// Shows the graph building itself step by step through insert()
//
// Compile: g++ -std=c++17 -o test_insert test_insert.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp
// Run:     ./test_insert

#include <iostream>
#include <cassert>
#include "nsw_graph.h"
#include "vectorizer.h"
#include "search.h"
#include "insert.hpp"
using namespace std;

int main() {

    cout << "================================================" << endl;
    cout << "  Insertion Test — NSW Graph" << endl;
    cout << "================================================" << endl;

    NSWGraph graph;

    // -------------------------------------------------------
    // TEST 1: insert into empty graph
    //first node — search never runs, just sets entry point
    // -------------------------------------------------------
    cout << "\n--- TEST 1: first insertion (empty graph) ---" << endl;
    insert(&graph, "data structures");

    assert(graph.getNodeCount() == 1);
    assert(graph.getEntryPoint() != nullptr);
    assert(graph.getEntryPoint()->text == "data structures");
    assert(graph.getEntryPoint()->neighbors.size() == 0);
    cout << "Node count : " << graph.getNodeCount() << endl;
    cout << "Entry point: " << graph.getEntryPoint()->text << endl;
    cout << "[PASS]" << endl;

    // -------------------------------------------------------
    // TEST 2: insert second node
    // search finds first node, they connect to each other
    // -------------------------------------------------------
    cout << "\n--- TEST 2: second insertion ---" << endl;
    insert(&graph, "algorithms");

    assert(graph.getNodeCount() == 2);
    assert(graph.getEntryPoint()->text == "algorithms");

    // both nodes should now be neighbors of each other
    Node* n1 = graph.getAllNodes()[0]; // data structures
    Node* n2 = graph.getAllNodes()[1]; // algorithms
    assert(n1->neighbors.size() == 1);
    assert(n2->neighbors.size() == 1);
    assert(n1->neighbors[0] == n2);
    assert(n2->neighbors[0] == n1);

    cout << "Node count : " << graph.getNodeCount() << endl;
    cout << "Entry point: " << graph.getEntryPoint()->text << endl;
    cout << "n1 neighbors: " << n1->neighbors[0]->text << endl;
    cout << "n2 neighbors: " << n2->neighbors[0]->text << endl;
    cout << "[PASS]" << endl;

    // -------------------------------------------------------
    // TEST 3: insert several semantically similar nodes
    // similar nodes should end up connected to each other
    // -------------------------------------------------------
    cout << "\n--- TEST 3: bulk insertion (8 nodes) ---" << endl;

    insert(&graph, "machine learning");
    insert(&graph, "deep learning");
    insert(&graph, "neural networks");
    insert(&graph, "artificial intelligence");
    insert(&graph, "operating systems");
    insert(&graph, "computer networks");
    insert(&graph, "database systems");

    assert(graph.getNodeCount() == 9);
    cout << "Node count : " << graph.getNodeCount() << endl;
    cout << "Entry point: " << graph.getEntryPoint()->text << endl;
    cout << "[PASS]" << endl;

    // -------------------------------------------------------
    // TEST 4: print graph to show edges formed by insertion
    // similar phrases should cluster together
    // -------------------------------------------------------
    cout << "\n--- TEST 4: graph state after all insertions ---" << endl;
    graph.printGraph();

    // -------------------------------------------------------
    // TEST 5: search after insertion
    // inserted nodes should be findable
    // -------------------------------------------------------
    cout << "\n--- TEST 5: search after insertion ---" << endl;

    string query = "machine learning";
    cout << "Query: \"" << query << "\"" << endl;

    vector<SearchResult> results = search(
        &graph,
        vectorize(query),
        3,
        EF_SEARCH
    );

    cout << "Top 3 results:" << endl;
    for (int i = 0; i < results.size(); i++) {
        cout << "  [" << i+1 << "] \""
             << results[i].text
             << "\" dist=" << results[i].distance << endl;
    }

    // top result should be exact match
    assert(results.size() > 0);
    assert(results[0].text == "machine learning");
    assert(results[0].distance < 0.01);
    cout << "[PASS] exact match found at rank 1" << endl;

    // -------------------------------------------------------
    // TEST 6: insert duplicate text
    // graph should allow it (no duplicate check on text)
    // but both nodes get separate entries
    // -------------------------------------------------------
    cout << "\n--- TEST 6: duplicate text insertion ---" << endl;
    int countBefore = graph.getNodeCount();
    insert(&graph, "machine learning");
    assert(graph.getNodeCount() == countBefore + 1);
    cout << "Node count before: " << countBefore << endl;
    cout << "Node count after : " << graph.getNodeCount() << endl;
    cout << "[PASS] duplicate allowed as separate node" << endl;

    // -------------------------------------------------------
    // TEST 7: insert empty string
    // should be rejected, node count stays same
    // -------------------------------------------------------
    cout << "\n--- TEST 7: empty string insertion ---" << endl;
    int countBeforeEmpty = graph.getNodeCount();
    insert(&graph, "");
    assert(graph.getNodeCount() == countBeforeEmpty);
    cout << "Node count unchanged: " << graph.getNodeCount() << endl;
    cout << "[PASS] empty string rejected" << endl;

    // -------------------------------------------------------
    // TEST 8: entry point always tracks newest node
    // -------------------------------------------------------
    cout << "\n--- TEST 8: entry point tracks newest node ---" << endl;
    insert(&graph, "software engineering");
    assert(graph.getEntryPoint()->text == "software engineering");
    cout << "Entry point: " << graph.getEntryPoint()->text << endl;
    cout << "[PASS]" << endl;

    cout << "\n================================================" << endl;
    cout << "  ALL INSERTION TESTS PASSED" << endl;
    cout << "================================================" << endl;

    cout << "\nDestructor runs now:" << endl;
    return 0;
}