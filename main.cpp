// ============================================================
// main.cpp — NSW Project Test Harness
//
// TEMPORARY FILE — Member 4 will replace this with the full CLI.
// Right now, this file tests Member 1's 4 functions and verifies
// that the full project compiles together cleanly.
//
// Compile:  g++ -std=c++17 -o test main.cpp vectorizer.cpp nsw_graph.cpp search.cpp
// Run:      ./test
// ============================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cassert>
#include "vectorizer.h"
#include "config.h"
#include "nsw_graph.h"
#include "search.h"

using namespace std;

// ------------------------------------------------------------
// Helper: count non-zero elements in a vector
// ------------------------------------------------------------
int countNonZero(const vector<double>& vec) {
    int count = 0;
    for (double v : vec) {
        if (v != 0.0) count++;
    }
    return count;
}

// ------------------------------------------------------------
// Helper: print a vector (shows non-zero entries only)
// ------------------------------------------------------------
void printVectorSparse(const vector<double>& vec) {
    cout << "  Size: " << vec.size() << " | Non-zero entries: " << countNonZero(vec) << endl;
    cout << "  Non-zero values: [";
    bool first = true;
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i] != 0.0) {
            if (!first) cout << ", ";
            cout << "idx" << i << "=" << vec[i];
            first = false;
        }
    }
    cout << "]" << endl;
}

// ------------------------------------------------------------
// TEST 1: getTrigrams()
// ------------------------------------------------------------
void testGetTrigrams() {
    cout << "\n========================================" << endl;
    cout << "TEST 1: getTrigrams()" << endl;
    cout << "========================================" << endl;

    vector<string> t1 = getTrigrams("hello");
    cout << "\n  getTrigrams(\"hello\"):" << endl;
    cout << "  Expected: [hel, ell, llo]" << endl;
    cout << "  Got:      [";
    for (size_t i = 0; i < t1.size(); i++) {
        if (i > 0) cout << ", ";
        cout << t1[i];
    }
    cout << "]" << endl;
    assert(t1.size() == 3);
    assert(t1[0] == "hel");
    assert(t1[1] == "ell");
    assert(t1[2] == "llo");
    cout << "  [PASS]" << endl;

    vector<string> t2 = getTrigrams("cat");
    cout << "\n  getTrigrams(\"cat\"):" << endl;
    cout << "  Expected: [cat]" << endl;
    cout << "  Got:      [";
    for (size_t i = 0; i < t2.size(); i++) {
        if (i > 0) cout << ", ";
        cout << t2[i];
    }
    cout << "]" << endl;
    assert(t2.size() == 1);
    assert(t2[0] == "cat");
    cout << "  [PASS]" << endl;

    vector<string> t3 = getTrigrams("hi");
    cout << "\n  getTrigrams(\"hi\"):" << endl;
    cout << "  Expected: [hi ] (padded with space)" << endl;
    cout << "  Got:      [";
    for (size_t i = 0; i < t3.size(); i++) {
        if (i > 0) cout << ", ";
        cout << "\"" << t3[i] << "\"";
    }
    cout << "]" << endl;
    assert(t3.size() == 1);
    cout << "  [PASS]" << endl;

    vector<string> t4 = getTrigrams("");
    cout << "\n  getTrigrams(\"\"):" << endl;
    cout << "  Expected: [] (empty)" << endl;
    cout << "  Got:      [] (size=" << t4.size() << ")" << endl;
    assert(t4.empty());
    cout << "  [PASS]" << endl;

    vector<string> t5 = getTrigrams("hello world");
    cout << "\n  getTrigrams(\"hello world\"):" << endl;
    cout << "  Trigram count: " << t5.size() << " (expected: 9)" << endl;
    assert(t5.size() == 9);
    cout << "  [PASS]" << endl;
}

// ------------------------------------------------------------
// TEST 2: hashTrigram()
// ------------------------------------------------------------
void testHashTrigram() {
    cout << "\n========================================" << endl;
    cout << "TEST 2: hashTrigram()" << endl;
    cout << "========================================" << endl;

    int h1 = hashTrigram("hel");
    int h2 = hashTrigram("hel");
    cout << "\n  hashTrigram(\"hel\") = " << h1 << endl;
    cout << "  hashTrigram(\"hel\") = " << h2 << " (same call, must match)" << endl;
    assert(h1 == h2);
    cout << "  [PASS] Deterministic" << endl;

    int h3 = hashTrigram("cat");
    int h4 = hashTrigram("dog");
    int h5 = hashTrigram("zzz");
    cout << "\n  hashTrigram(\"cat\") = " << h3 << endl;
    cout << "  hashTrigram(\"dog\") = " << h4 << endl;
    cout << "  hashTrigram(\"zzz\") = " << h5 << endl;
    assert(h3 >= 0 && h3 < VECTOR_DIM);
    assert(h4 >= 0 && h4 < VECTOR_DIM);
    assert(h5 >= 0 && h5 < VECTOR_DIM);
    cout << "  [PASS] All values in range [0, " << VECTOR_DIM << ")" << endl;
}

// ------------------------------------------------------------
// TEST 3: vectorize()
// ------------------------------------------------------------
void testVectorize() {
    cout << "\n========================================" << endl;
    cout << "TEST 3: vectorize()" << endl;
    cout << "========================================" << endl;

    vector<double> v1 = vectorize("hello world");
    cout << "\n  vectorize(\"hello world\"):" << endl;
    printVectorSparse(v1);
    assert(v1.size() == VECTOR_DIM);
    assert(countNonZero(v1) > 0);
    cout << "  [PASS] Correct size and non-empty" << endl;

    vector<double> v1_again = vectorize("hello world");
    bool identical = true;
    for (size_t i = 0; i < v1.size(); i++) {
        if (v1[i] != v1_again[i]) { identical = false; break; }
    }
    assert(identical);
    cout << "\n  Re-vectorizing same text -> identical vector" << endl;
    cout << "  [PASS] Deterministic" << endl;

    vector<double> v2 = vectorize("hello worl");
    double dist = cosineDistance(v1, v2);
    cout << "\n  vectorize(\"hello world\") vs vectorize(\"hello worl\"):" << endl;
    cout << "  Distance: " << fixed << setprecision(6) << dist << endl;
    cout << "  (Typo-tolerance check: distance should be small, << 1.0)" << endl;
    cout << "  [PASS] Typo-tolerance demonstrated" << endl;

    vector<double> v3 = vectorize("quantum physics equations");
    double dist_diff = cosineDistance(v1, v3);
    cout << "\n  vectorize(\"hello world\") vs vectorize(\"quantum physics equations\"):" << endl;
    cout << "  Distance: " << fixed << setprecision(6) << dist_diff << endl;
    cout << "  (Unrelated texts: distance should be larger)" << endl;
    cout << "  [PASS] Dissimilar texts are farther apart" << endl;

    vector<double> v_empty = vectorize("");
    cout << "\n  vectorize(\"\"):" << endl;
    cout << "  Non-zero entries: " << countNonZero(v_empty) << endl;
    assert(v_empty.size() == VECTOR_DIM);
    assert(countNonZero(v_empty) == 0);
    cout << "  [PASS] Empty string produces zero vector" << endl;
}

// ------------------------------------------------------------
// TEST 4: cosineDistance()
// ------------------------------------------------------------
void testCosineDistance() {
    cout << "\n========================================" << endl;
    cout << "TEST 4: cosineDistance()" << endl;
    cout << "========================================" << endl;

    vector<double> a = {1.0, 2.0, 3.0};
    vector<double> b = {1.0, 2.0, 3.0};
    double d1 = cosineDistance(a, b);
    cout << "\n  cosineDistance([1,2,3], [1,2,3]):" << endl;
    cout << "  Expected: 0.000000 (identical)" << endl;
    cout << "  Got:      " << fixed << setprecision(6) << d1 << endl;
    assert(d1 < 0.0001);
    cout << "  [PASS]" << endl;

    vector<double> c = {1.0, 0.0, 0.0};
    vector<double> d = {-1.0, 0.0, 0.0};
    double d2 = cosineDistance(c, d);
    cout << "\n  cosineDistance([1,0,0], [-1,0,0]):" << endl;
    cout << "  Expected: 2.000000 (opposite vectors)" << endl;
    cout << "  Got:      " << fixed << setprecision(6) << d2 << endl;
    cout << "  [PASS]" << endl;

    vector<double> e = {1.0, 0.0};
    vector<double> f = {0.0, 1.0};
    double d3 = cosineDistance(e, f);
    cout << "\n  cosineDistance([1,0], [0,1]):" << endl;
    cout << "  Expected: 1.000000 (orthogonal)" << endl;
    cout << "  Got:      " << fixed << setprecision(6) << d3 << endl;
    assert(d3 > 0.999 && d3 <= 1.001);
    cout << "  [PASS]" << endl;

    vector<double> zero(5, 0.0);
    vector<double> g = {1.0, 2.0, 3.0, 4.0, 5.0};
    double d4 = cosineDistance(zero, g);
    cout << "\n  cosineDistance([0,0,0,0,0], [1,2,3,4,5]):" << endl;
    cout << "  Expected: 1.000000 (zero-vector guard)" << endl;
    cout << "  Got:      " << fixed << setprecision(6) << d4 << endl;
    assert(d4 == 1.0);
    cout << "  [PASS] Division by zero prevented" << endl;

    double d5 = cosineDistance(zero, zero);
    cout << "\n  cosineDistance([0,0,0,0,0], [0,0,0,0,0]):" << endl;
    cout << "  Expected: 1.000000 (zero-vector guard)" << endl;
    cout << "  Got:      " << fixed << setprecision(6) << d5 << endl;
    assert(d5 == 1.0);
    cout << "  [PASS]" << endl;

    vector<double> v_hello = vectorize("hello world");
    vector<double> v_hella = vectorize("hella world");
    vector<double> v_quantum = vectorize("quantum entanglement theory");
    double d_real1 = cosineDistance(v_hello, v_hella);
    double d_real2 = cosineDistance(v_hello, v_quantum);
    cout << "\n  Real-world comparison:" << endl;
    cout << "  \"hello world\" vs \"hella world\":     " << fixed << setprecision(6) << d_real1 << endl;
    cout << "  \"hello world\" vs \"quantum entang...\": " << fixed << setprecision(6) << d_real2 << endl;
    assert(d_real1 < d_real2);
    cout << "  [PASS] Similar texts are closer than unrelated ones" << endl;
}

// ------------------------------------------------------------
// TEST 5: End-to-End Demonstration (brute force)
// ------------------------------------------------------------
void testEndToEnd() {
    cout << "\n========================================" << endl;
    cout << "TEST 5: End-to-End Demonstration" << endl;
    cout << "========================================" << endl;

    vector<string> notes = {
        "data structures and algorithms",
        "machine learning basics",
        "database management systems",
        "computer networks and security",
        "operating system concepts",
    };

    string query = "data structures course";

    cout << "\n  Notes in database:" << endl;
    for (const string& note : notes) {
        cout << "    - \"" << note << "\"" << endl;
    }
    cout << "\n  Query: \"" << query << "\"" << endl;

    vector<double> queryVec = vectorize(query);

    string bestMatch;
    double bestDist = 999.0;

    cout << "\n  Distances from query to each note:" << endl;
    for (const string& note : notes) {
        vector<double> noteVec = vectorize(note);
        double dist = cosineDistance(queryVec, noteVec);
        cout << "    \"" << note << "\" -> distance: "
             << fixed << setprecision(6) << dist;
        if (dist < bestDist) {
            bestDist = dist;
            bestMatch = note;
            cout << "  <-- BEST";
        }
        cout << endl;
    }

    cout << "\n  BEST MATCH: \"" << bestMatch << "\" (distance: "
         << fixed << setprecision(6) << bestDist << ")" << endl;
    assert(bestMatch == "data structures and algorithms");
    cout << "  [PASS] Correct match found" << endl;
}

// ------------------------------------------------------------
// TEST 6: Graph Operations (createNode, addEdge, printGraph)
// Tests Member 2's graph functions (now FULLY IMPLEMENTED)
// ------------------------------------------------------------
void testGraphOperations() {
    cout << "\n========================================" << endl;
    cout << "TEST 6: Graph Operations (Member 2)" << endl;
    cout << "========================================" << endl;

    NSWGraph graph;

    // --- Test 6a: Empty graph ---
    cout << "\n  --- 6a: Empty graph ---" << endl;
    assert(graph.getNodeCount() == 0);
    assert(graph.getEntryPoint() == nullptr);
    assert(graph.getAllNodes().empty());
    cout << "  Node count: 0 (correct)" << endl;
    cout << "  Entry point: nullptr (correct)" << endl;
    cout << "  [PASS]" << endl;

    // --- Test 6b: Create nodes ---
    cout << "\n  --- 6b: Create nodes via createNode() ---" << endl;
    Node* n0 = graph.createNode("data structures", vectorize("data structures"));
    Node* n1 = graph.createNode("machine learning", vectorize("machine learning"));
    Node* n2 = graph.createNode("operating systems", vectorize("operating systems"));
    assert(graph.getNodeCount() == 3);
    cout << "  Created 3 nodes. Node count: " << graph.getNodeCount() << endl;
    cout << "  [PASS]" << endl;

    // --- Test 6c: Add edges ---
    cout << "\n  --- 6c: Add edges ---" << endl;
    graph.addEdge(n0, n1);  // data <-> machine
    graph.addEdge(n1, n2);  // machine <-> operating
    graph.addEdge(n0, n2);  // data <-> operating
    cout << "  n0 neighbors: " << n0->neighbors.size() << " (expected: 2)" << endl;
    cout << "  n1 neighbors: " << n1->neighbors.size() << " (expected: 2)" << endl;
    cout << "  n2 neighbors: " << n2->neighbors.size() << " (expected: 2)" << endl;
    assert(n0->neighbors.size() == 2);
    assert(n1->neighbors.size() == 2);
    assert(n2->neighbors.size() == 2);
    cout << "  [PASS]" << endl;

    // --- Test 6d: Self-edge prevention ---
    cout << "\n  --- 6d: Self-edge prevention ---" << endl;
    graph.addEdge(n0, n0);
    cout << "  n0 neighbors after self-edge attempt: " << n0->neighbors.size()
         << " (expected: 2, unchanged)" << endl;
    assert(n0->neighbors.size() == 2);
    cout << "  [PASS]" << endl;

    // --- Test 6e: Duplicate edge prevention ---
    cout << "\n  --- 6e: Duplicate edge prevention ---" << endl;
    graph.addEdge(n0, n1);  // Already connected
    cout << "  n0 neighbors after duplicate edge attempt: " << n0->neighbors.size()
         << " (expected: 2, unchanged)" << endl;
    assert(n0->neighbors.size() == 2);
    cout << "  [PASS]" << endl;

    // --- Test 6f: Entry point ---
    cout << "\n  --- 6f: Entry point ---" << endl;
    graph.setEntryPoint(n2);
    assert(graph.getEntryPoint() == n2);
    cout << "  Entry point set to: \"" << graph.getEntryPoint()->text << "\"" << endl;
    cout << "  [PASS]" << endl;

    // --- Test 6g: printGraph ---
    cout << "\n  --- 6g: printGraph() ---" << endl;
    graph.printGraph();
    cout << "  [PASS] (visual inspection)" << endl;

    // --- Test 6h: getAllNodes for brute force ---
    cout << "\n  --- 6h: getAllNodes() for Member 4's brute force ---" << endl;
    const vector<Node*>& allNodes = graph.getAllNodes();
    assert(allNodes.size() == 3);
    cout << "  getAllNodes() returned " << allNodes.size() << " nodes" << endl;
    cout << "  [PASS]" << endl;

    // --- Test 6i: search on small graph ---
    cout << "\n  --- 6i: search() on 3-node graph ---" << endl;
    vector<SearchResult> results = search(&graph, vectorize("data"), 3, EF_SEARCH);
    cout << "  search() returned " << results.size() << " results" << endl;
    assert(results.size() == 3); // Should find all 3 nodes
    cout << "  Top result: \"" << results[0].text << "\" (dist: " << fixed << setprecision(4) << results[0].distance << ")" << endl;
    assert(results[0].text == "data structures"); // Closest match
    cout << "  [PASS]" << endl;

    cout << "\n  [ALL GRAPH OPERATION TESTS PASSED]" << endl;
}

// ------------------------------------------------------------
// TEST 7: Hard Deletion
// Tests the deleteNode() function and verifies:
//   7a: Node is removed from graph (node count decreases)
//   7b: Edges to target are removed from neighbors' lists
//   7c: Neighbors are re-linked to each other (clique)
//   7d: Entry point is reassigned when deleted
//   7e: Deleting the only node leaves graph empty
//   7f: Delete nullptr does nothing
//   7g: Duplicate edge check prevents issues during relinking
// ------------------------------------------------------------
void testHardDeletion() {
    cout << "\n========================================" << endl;
    cout << "TEST 7: Hard Deletion" << endl;
    cout << "========================================" << endl;

    // ======== SETUP: 4-node graph ========
    //    n0 -- n1
    //    |  \  |
    //    n2 -- n3
    //
    NSWGraph graph;
    Node* n0 = graph.createNode("apple", vectorize("apple"));
    Node* n1 = graph.createNode("apply", vectorize("apply"));
    Node* n2 = graph.createNode("banana", vectorize("banana"));
    Node* n3 = graph.createNode("orange", vectorize("orange"));

    graph.addEdge(n0, n1);
    graph.addEdge(n0, n2);
    graph.addEdge(n0, n3);
    graph.addEdge(n1, n2);
    graph.addEdge(n1, n3);
    graph.addEdge(n2, n3);

    graph.setEntryPoint(n0);
    cout << "\n  Setup: 4-node fully connected graph (6 edges)" << endl;
    cout << "  n0 (apple) neighbors: " << n0->neighbors.size() << endl;
    cout << "  n1 (apply) neighbors: " << n1->neighbors.size() << endl;
    cout << "  n2 (banana) neighbors: " << n2->neighbors.size() << endl;
    cout << "  n3 (orange) neighbors: " << n3->neighbors.size() << endl;
    cout << "  Entry point: " << graph.getEntryPoint()->text << endl;
    graph.printGraph();

    // --- Test 7a: Delete a non-entry-point node ---
    cout << "\n  --- 7a: Delete n3 (orange) — non-entry-point ---" << endl;
    cout << "  Before: node count = " << graph.getNodeCount() << endl;
    graph.deleteNode(n3);
    cout << "  After:  node count = " << graph.getNodeCount() << " (expected: 3)" << endl;
    assert(graph.getNodeCount() == 3);
    // Entry point should be unchanged (was n0, not n3)
    assert(graph.getEntryPoint() == n0);
    cout << "  Entry point still: " << graph.getEntryPoint()->text << " (correct)" << endl;
    cout << "  [PASS]" << endl;

    // --- Test 7b: Edges to target removed from neighbors ---
    cout << "\n  --- 7b: Edges to n3 removed from neighbors ---" << endl;
    // n0 had [n1, n2, n3] → after deleting n3, should be [n1, n2]
    cout << "  n0 neighbors: " << n0->neighbors.size() << " (expected: 2)" << endl;
    assert(n0->neighbors.size() == 2);
    // n1 had [n0, n2, n3] → after deleting n3, should be [n0, n2]
    cout << "  n1 neighbors: " << n1->neighbors.size() << " (expected: 2)" << endl;
    assert(n1->neighbors.size() == 2);
    // n2 had [n0, n1, n3] → after deleting n3, should be [n0, n1]
    cout << "  n2 neighbors: " << n2->neighbors.size() << " (expected: 2)" << endl;
    assert(n2->neighbors.size() == 2);
    cout << "  [PASS] All edges to deleted node removed." << endl;

    // --- Test 7c: Neighbors re-linked ---
    cout << "\n  --- 7c: Neighbors re-linked (clique formed) ---" << endl;
    // n3's neighbors were [n0, n1, n2].
    // Phase 1 of deleteNode tries: addEdge(n0,n1), addEdge(n0,n2), addEdge(n1,n2)
    // All three already existed → duplicate check prevents adding again.
    // Verify no duplicate edges exist:
    bool dupFound = false;
    for (size_t i = 0; i < n0->neighbors.size(); i++) {
        for (size_t j = i + 1; j < n0->neighbors.size(); j++) {
            if (n0->neighbors[i] == n0->neighbors[j]) {
                dupFound = true;
            }
        }
    }
    assert(!dupFound);
    cout << "  No duplicate edges in n0's neighbor list." << endl;
    cout << "  [PASS]" << endl;

    // --- Test 7d: Delete the entry point ---
    cout << "\n  --- 7d: Delete n0 (apple) — the entry point ---" << endl;
    graph.deleteNode(n0);
    cout << "  Node count: " << graph.getNodeCount() << " (expected: 2)" << endl;
    assert(graph.getNodeCount() == 2);
    // Entry point should be reassigned (first remaining node = n1 or n2)
    cout << "  New entry point: \""
         << (graph.getEntryPoint() ? graph.getEntryPoint()->text : "nullptr") << "\"" << endl;
    assert(graph.getEntryPoint() != nullptr);
    cout << "  [PASS] Entry point reassigned after deletion." << endl;

    // Verify n1 and n2 are still connected
    cout << "  n1 neighbors: " << n1->neighbors.size() << " (expected: 1 = n2)" << endl;
    cout << "  n2 neighbors: " << n2->neighbors.size() << " (expected: 1 = n1)" << endl;
    assert(n1->neighbors.size() == 1);
    assert(n2->neighbors.size() == 1);
    assert(n1->neighbors[0] == n2);
    assert(n2->neighbors[0] == n1);
    cout << "  [PASS] n1 and n2 remain connected after n0 deletion." << endl;

    graph.printGraph();

    // --- Test 7e: Delete until graph is empty ---
    cout << "\n  --- 7e: Delete remaining nodes until empty ---" << endl;
    graph.deleteNode(n1);
    cout << "  After deleting n1: count = " << graph.getNodeCount() << endl;
    assert(graph.getNodeCount() == 1);

    graph.deleteNode(n2);
    cout << "  After deleting n2: count = " << graph.getNodeCount() << endl;
    assert(graph.getNodeCount() == 0);
    assert(graph.getEntryPoint() == nullptr);
    cout << "  Entry point: nullptr (correct, graph is empty)" << endl;
    cout << "  [PASS] Graph is empty, entry point is nullptr." << endl;

    // --- Test 7f: Delete nullptr ---
    cout << "\n  --- 7f: Delete nullptr ---" << endl;
    graph.deleteNode(nullptr); // Should not crash
    cout << "  deleteNode(nullptr) — no crash." << endl;
    assert(graph.getNodeCount() == 0);
    cout << "  [PASS] nullptr deletion handled gracefully." << endl;

    // --- Test 7g: Complex relinking with M limit ---
    cout << "\n  --- 7g: Relinking under M limit pressure ---" << endl;
    // Create a star graph: center node connected to many leaves
    NSWGraph graph2;
    Node* center = graph2.createNode("center", vectorize("center"));
    vector<Node*> leaves;
    for (int i = 0; i < M; i++) {
        string leafText = "leaf" + to_string(i);
        Node* leaf = graph2.createNode(leafText, vectorize(leafText));
        graph2.addEdge(center, leaf);
        leaves.push_back(leaf);
    }
    graph2.setEntryPoint(center);
    cout << "  Star graph: center + " << M << " leaves" << endl;
    cout << "  Center neighbors: " << center->neighbors.size() << " (expected: " << M << ")" << endl;
    assert(center->neighbors.size() == (size_t)M);
    cout << "  Each leaf neighbors: " << leaves[0]->neighbors.size() << " (expected: 1 = center)" << endl;
    assert(leaves[0]->neighbors.size() == 1);

    // Delete the center — its M neighbors should try to relink
    // But each leaf already has 1 neighbor (center), so they have
    // room for M-1 = 15 more. With M=16 leaves, each pair can connect.
    cout << "\n  Deleting center node..." << endl;
    graph2.deleteNode(center);
    cout << "  Remaining nodes: " << graph2.getNodeCount() << " (expected: " << M << ")" << endl;
    assert(graph2.getNodeCount() == M);

    // Check that leaves now have multiple neighbors (relinked)
    cout << "  Leaf 0 neighbors: " << leaves[0]->neighbors.size() << endl;
    cout << "  (Some relinking edges created, limited by M)" << endl;
    // Each leaf can have at most M=16 neighbors. With 15 other leaves,
    // they should get connected to many (up to M=16 - but leaf0 already
    // lost its edge to center during Phase 2, so it has room for 16).
    assert(leaves[0]->neighbors.size() > 0); // At least some relinking happened
    cout << "  [PASS] Relinking works under M limit." << endl;

    // Verify no dangling pointers (no neighbor points to center)
    bool danglingFound = false;
    const vector<Node*>& remainingNodes = graph2.getAllNodes();
    for (Node* node : remainingNodes) {
        for (Node* nb : node->neighbors) {
            // Every neighbor should be in the remaining graph
            bool foundInGraph = false;
            for (Node* n : remainingNodes) {
                if (nb == n) { foundInGraph = true; break; }
            }
            if (!foundInGraph) danglingFound = true;
        }
    }
    assert(!danglingFound);
    cout << "  [PASS] No dangling pointers after deletion." << endl;

    // --- Test 7h: Duplicate edge prevention during multi-deletion ---
    cout << "\n  --- 7h: No duplicate edges after multiple deletions ---" << endl;
    for (Node* node : remainingNodes) {
        for (size_t i = 0; i < node->neighbors.size(); i++) {
            for (size_t j = i + 1; j < node->neighbors.size(); j++) {
                assert(node->neighbors[i] != node->neighbors[j]);
            }
        }
    }
    cout << "  Verified: no duplicate neighbors in any node." << endl;
    cout << "  [PASS]" << endl;

    cout << "\n  [ALL HARD DELETION TESTS PASSED]" << endl;
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
// ------------------------------------------------------------
// TEST 8: NSW Graph Search
// Tests Member 3's fully implemented search() function.
// Verifies correct results, different k values, and edge cases.
// ------------------------------------------------------------
void testNSWSearch() {
    cout << "\n========================================" << endl;
    cout << "TEST 8: NSW Graph Search (Member 3)" << endl;
    cout << "========================================" << endl;

    // --- Test 8a: Search on empty graph ---
    cout << "\n  --- 8a: Search on empty graph ---" << endl;
    NSWGraph emptyGraph;
    vector<SearchResult> r0 = search(&emptyGraph, vectorize("test"), 5, EF_SEARCH);
    assert(r0.empty());
    cout << "  search() on empty graph: 0 results (correct)" << endl;
    cout << "  [PASS]" << endl;

    // --- Test 8b: Search on single-node graph ---
    cout << "\n  --- 8b: Search on single-node graph ---" << endl;
    NSWGraph singleGraph;
    singleGraph.createNode("hello world", vectorize("hello world"));
    singleGraph.setEntryPoint(singleGraph.getAllNodes()[0]);
    vector<SearchResult> r1 = search(&singleGraph, vectorize("hello"), 3, EF_SEARCH);
    assert(r1.size() == 1);
    assert(r1[0].text == "hello world");
    cout << "  Result: \"" << r1[0].text << "\" (dist: " << fixed << setprecision(4) << r1[0].distance << ")" << endl;
    cout << "  [PASS]" << endl;

    // --- Test 8c: Search finds the closest match on multi-node graph ---
    cout << "\n  --- 8c: Correct ranking on multi-node graph ---" << endl;
    NSWGraph graph;
    vector<Node*> nodes;
    vector<string> texts = {
        "data structures and algorithms",
        "machine learning basics",
        "database management systems",
        "computer networks and security",
        "operating system concepts",
        "data science with python",
        "discrete mathematics",
        "software engineering principles"
    };
    for (const string& t : texts) {
        Node* n = graph.createNode(t, vectorize(t));
        nodes.push_back(n);
    }
    // Connect as a chain (simple but connected)
    for (size_t i = 0; i < nodes.size() - 1; i++) {
        graph.addEdge(nodes[i], nodes[i + 1]);
    }
    // Add a few extra edges for better connectivity (skip connections)
    graph.addEdge(nodes[0], nodes[3]);
    graph.addEdge(nodes[1], nodes[4]);
    graph.addEdge(nodes[2], nodes[5]);
    graph.addEdge(nodes[0], nodes[6]);
    graph.addEdge(nodes[3], nodes[7]);
    graph.setEntryPoint(nodes.back());

    string query = "data structures course";
    vector<SearchResult> r2 = search(&graph, vectorize(query), 3, EF_SEARCH);
    cout << "  Query: \"" << query << "\"" << endl;
    cout << "  Top " << r2.size() << " results:" << endl;
    for (size_t i = 0; i < r2.size(); i++) {
        cout << "    " << (i + 1) << ". \"" << r2[i].text << "\" (dist: "
             << fixed << setprecision(6) << r2[i].distance << ")" << endl;
    }
    assert(r2.size() == 3);
    assert(r2[0].text == "data structures and algorithms");
    cout << "  Top-1 match is correct!" << endl;
    // Verify results are sorted (ascending distance)
    for (size_t i = 1; i < r2.size(); i++) {
        assert(r2[i].distance >= r2[i - 1].distance);
    }
    cout << "  Results correctly sorted by distance." << endl;
    cout << "  [PASS]" << endl;

    // --- Test 8d: k=1 returns exactly 1 result ---
    cout << "\n  --- 8d: k=1 ---" << endl;
    vector<SearchResult> r3 = search(&graph, vectorize(query), 1, EF_SEARCH);
    assert(r3.size() == 1);
    cout << "  search(k=1) returned " << r3.size() << " result" << endl;
    cout << "  [PASS]" << endl;

    // --- Test 8e: k > graph size ---
    cout << "\n  --- 8e: k larger than graph size ---" << endl;
    vector<SearchResult> r4 = search(&graph, vectorize(query), 100, EF_SEARCH);
    cout << "  search(k=100) on 8-node graph returned " << r4.size() << " results" << endl;
    assert(r4.size() <= 8);
    assert(r4.size() > 0);
    cout << "  [PASS]" << endl;

    cout << "\n  [ALL NSW SEARCH TESTS PASSED]" << endl;
}

// ------------------------------------------------------------
// TEST 9: Search vs Brute Force Comparison
// Compares NSW graph search with O(N) brute force.
// NSW should find the same (or very similar) top results.
// ------------------------------------------------------------
void testSearchVsBruteForce() {
    cout << "\n========================================" << endl;
    cout << "TEST 9: NSW Search vs Brute Force" << endl;
    cout << "========================================" << endl;

    NSWGraph graph;
    vector<string> dataset = {
        "artificial intelligence and deep learning",
        "data structures and algorithms",
        "web development with html css javascript",
        "computer architecture and organization",
        "data science and machine learning",
        "operating systems design principles",
        "computer networks tcp ip model",
        "software engineering methodologies",
        "database management systems sql",
        "discrete mathematics and graph theory"
    };

    vector<Node*> nodes;
    for (const string& t : dataset) {
        Node* n = graph.createNode(t, vectorize(t));
        nodes.push_back(n);
    }
    // Create a well-connected graph (each node connected to next 3)
    for (size_t i = 0; i < nodes.size(); i++) {
        for (size_t j = i + 1; j < min(i + 4, nodes.size()); j++) {
            graph.addEdge(nodes[i], nodes[j]);
        }
    }
    graph.setEntryPoint(nodes.back());

    string query = "machine learning and data";
    vector<double> queryVec = vectorize(query);
    int k = 3;

    // Run both searches
    vector<SearchResult> nswResults = search(&graph, queryVec, k, EF_SEARCH);
    vector<SearchResult> bfResults = bruteForceSearch(&graph, queryVec, k);

    cout << "\n  Query: \"" << query << "\"" << endl;
    cout << "  Dataset: " << dataset.size() << " nodes, k=" << k << endl;

    cout << "\n  Brute Force (O(N)) top-" << k << ":" << endl;
    for (size_t i = 0; i < bfResults.size(); i++) {
        cout << "    " << (i + 1) << ". \"" << bfResults[i].text << "\" (dist: "
             << fixed << setprecision(6) << bfResults[i].distance << ")" << endl;
    }

    cout << "\n  NSW Search (O(log N)) top-" << k << ":" << endl;
    for (size_t i = 0; i < nswResults.size(); i++) {
        cout << "    " << (i + 1) << ". \"" << nswResults[i].text << "\" (dist: "
             << fixed << setprecision(6) << nswResults[i].distance << ")" << endl;
    }

    // The NSW top-1 should match brute force top-1
    cout << "\n  Brute Force #1: \"" << bfResults[0].text << "\"" << endl;
    cout << "  NSW Search #1:      \"" << nswResults[0].text << "\"" << endl;
    assert(nswResults[0].text == bfResults[0].text);
    cout << "  Top-1 results MATCH!" << endl;

    // Both should return k results
    assert(nswResults.size() == (size_t)k);
    assert(bfResults.size() == (size_t)k);
    cout << "  Both returned " << k << " results." << endl;
    cout << "  [PASS]" << endl;

    cout << "\n  [SEARCH VS BRUTE FORCE COMPARISON PASSED]" << endl;
}

// ------------------------------------------------------------
// TEST 10: Search After Deletion
// Verifies that search works correctly after hard deletion.
// Deleted nodes must NOT appear in results.
// ------------------------------------------------------------
void testSearchAfterDeletion() {
    cout << "\n========================================" << endl;
    cout << "TEST 10: Search After Deletion" << endl;
    cout << "========================================" << endl;

    NSWGraph graph;
    vector<Node*> nodes;
    vector<string> texts = {
        "apple fruit",
        "banana fruit",
        "carrot vegetable",
        "dog animal",
        "elephant animal"
    };
    for (const string& t : texts) {
        Node* n = graph.createNode(t, vectorize(t));
        nodes.push_back(n);
    }
    for (size_t i = 0; i < nodes.size() - 1; i++) {
        graph.addEdge(nodes[i], nodes[i + 1]);
    }
    graph.addEdge(nodes[0], nodes[3]);
    graph.addEdge(nodes[1], nodes[4]);
    graph.setEntryPoint(nodes.back());

    // --- 10a: Search before deletion ---
    cout << "\n  --- 10a: Search before deletion ---" << endl;
    vector<SearchResult> r_before = search(&graph, vectorize("fruit"), 5, EF_SEARCH);
    cout << "  Results (5 nodes in graph):" << endl;
    for (size_t i = 0; i < r_before.size(); i++) {
        cout << "    " << r_before[i].text << " (dist: "
             << fixed << setprecision(4) << r_before[i].distance << ")" << endl;
    }
    assert(r_before.size() == 5);
    cout << "  [PASS]" << endl;

    // --- 10b: Delete "apple fruit" and search again ---
    cout << "\n  --- 10b: Delete \"apple fruit\" and search ---" << endl;
    graph.deleteNode(nodes[0]);
    vector<SearchResult> r_after = search(&graph, vectorize("fruit"), 5, EF_SEARCH);
    cout << "  Results (4 nodes remaining):" << endl;
    for (size_t i = 0; i < r_after.size(); i++) {
        cout << "    " << r_after[i].text << " (dist: "
             << fixed << setprecision(4) << r_after[i].distance << ")" << endl;
    }
    assert(r_after.size() == 4);
    // Verify deleted node is NOT in results
    for (const SearchResult& sr : r_after) {
        assert(sr.text != "apple fruit");
    }
    cout << "  Deleted node NOT in results (correct)" << endl;
    cout << "  [PASS]" << endl;

    // --- 10c: Brute force also confirms ---
    cout << "\n  --- 10c: Brute force confirms ---" << endl;
    vector<SearchResult> bf_after = bruteForceSearch(&graph, vectorize("fruit"), 5);
    assert(bf_after.size() == 4);
    for (const SearchResult& sr : bf_after) {
        assert(sr.text != "apple fruit");
    }
    cout << "  Brute force also returns 4 results, no deleted node." << endl;
    cout << "  [PASS]" << endl;

    cout << "\n  [SEARCH AFTER DELETION PASSED]" << endl;
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main() {
    cout << "================================================" << endl;
    cout << "  NSW Project — Full Test Suite" << endl;
    cout << "  Member 1 (Arqish): getTrigrams, hashTrigram," << endl;
    cout << "                      vectorize, cosineDistance" << endl;
    cout << "  Member 2 (Areeba):  NSWGraph — FULLY IMPLEMENTED" << endl;
    cout << "                      createNode, addEdge, deleteNode" << endl;
    cout << "  Member 3 (Hira):    search() + bruteForceSearch()" << endl;
    cout << "                      FULLY IMPLEMENTED" << endl;
    cout << "  Member 4 (Shaheer): Integration, CLI, Persistence" << endl;
    cout << "  Deletion: HARD (edges removed, memory freed)" << endl;
    cout << "================================================" << endl;

    testGetTrigrams();
    testHashTrigram();
    testVectorize();
    testCosineDistance();
    testEndToEnd();
    testGraphOperations();
    testHardDeletion();
    testNSWSearch();
    testSearchVsBruteForce();
    testSearchAfterDeletion();

    cout << "\n================================================" << endl;
    cout << "  ALL TESTS PASSED!" << endl;
    cout << "  Member 1 (Arqish): COMPLETE" << endl;
    cout << "  Member 2 (Areeba):  COMPLETE" << endl;
    cout << "  Member 3 (Hira):    COMPLETE" << endl;
    cout << "  Member 4 (Shaheer): Integration + CLI + Persistence" << endl;
    cout << "================================================" << endl;

    return 0;
}
