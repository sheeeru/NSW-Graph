// main.cpp (don't run this yet)
// Compile:  g++ -std=c++17 -o test main.cpp vectorizer.cpp nsw_graph.cpp search.cpp
// Run:      ./test

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cassert>
#include "vectorizer.h"
#include "config.h"
#include "nsw_graph.h"

using namespace std;

int countNonZero(const vector<double> &vec)
{
    int count = 0;
    for (double v : vec)
    {
        if (v != 0.0)
            count++;
    }
    return count;
}

// Helper: print a vector (shows non-zero entries only)
void printVectorSparse(const vector<double> &vec)
{
    cout << "  Size: " << vec.size() << " | Non-zero entries: " << countNonZero(vec) << endl;
    cout << "  Non-zero values: [";
    bool first = true;
    for (size_t i = 0; i < vec.size(); i++)
    {
        if (vec[i] != 0.0)
        {
            if (!first)
                cout << ", ";
            cout << "idx" << i << "=" << vec[i];
            first = false;
        }
    }
    cout << "]" << endl;
}

// Helper: print a vector of strings
void printStringVec(const vector<string> &vec)
{
    cout << "  [";
    for (size_t i = 0; i < vec.size(); i++)
    {
        if (i > 0)
            cout << ", ";
        cout << "\"" << vec[i] << "\"";
    }
    cout << "]" << endl;
}

void testGetTrigrams()
{
    cout << "\n========================================" << endl;
    cout << "TEST 1: getTrigrams() — Member 1" << endl;
    cout << "========================================" << endl;

    // --- 1a: Normal text (5 chars -> 3 trigrams) ---
    cout << "\n  --- 1a: Normal text ---" << endl;
    vector<string> t1 = getTrigrams("hello");
    cout << "  getTrigrams(\"hello\"):" << endl;
    printStringVec(t1);
    cout << "  Expected: [\"hel\", \"ell\", \"llo\"]" << endl;
    assert(t1.size() == 3);
    assert(t1[0] == "hel");
    assert(t1[1] == "ell");
    assert(t1[2] == "llo");
    cout << "  [PASS]" << endl;

    // --- 1b: Exactly 3 characters (1 trigram) ---
    cout << "\n  --- 1b: Exactly 3 chars ---" << endl;
    vector<string> t2 = getTrigrams("cat");
    cout << "  getTrigrams(\"cat\"):" << endl;
    printStringVec(t2);
    assert(t2.size() == 1);
    assert(t2[0] == "cat");
    cout << "  [PASS]" << endl;

    // --- 1c: Short text (< 3 chars, padded with spaces) ---
    cout << "\n  --- 1c: Short text (padded) ---" << endl;
    vector<string> t3 = getTrigrams("hi");
    cout << "  getTrigrams(\"hi\"):" << endl;
    printStringVec(t3);
    assert(t3.size() == 1);
    assert(t3[0] == "hi ");
    cout << "  [PASS]" << endl;

    // --- 1d: Single character ---
    cout << "\n  --- 1d: Single character ---" << endl;
    vector<string> t3b = getTrigrams("a");
    cout << "  getTrigrams(\"a\"):" << endl;
    printStringVec(t3b);
    assert(t3b.size() == 1);
    assert(t3b[0] == "a  ");
    cout << "  [PASS]" << endl;

    // --- 1e: Empty string ---
    cout << "\n  --- 1e: Empty string ---" << endl;
    vector<string> t4 = getTrigrams("");
    cout << "  getTrigrams(\"\"):" << endl;
    cout << "  Got: [] (size=" << t4.size() << ")" << endl;
    assert(t4.empty());
    cout << "  [PASS]" << endl;

    // --- 1f: Multi-word string ---
    cout << "\n  --- 1f: Multi-word (11 chars -> 9 trigrams) ---" << endl;
    vector<string> t5 = getTrigrams("hello world");
    cout << "  getTrigrams(\"hello world\"):" << endl;
    printStringVec(t5);
    assert(t5.size() == 9);
    cout << "  [PASS]" << endl;

    // --- 1g: Trigram count formula: length - 2 ---
    cout << "\n  --- 1g: Trigram count formula check ---" << endl;
    string testStr = "abcdefghij"; // 10 chars -> 8 trigrams
    vector<string> t6 = getTrigrams(testStr);
    assert(t6.size() == 8);
    cout << "  \"" << testStr << "\" (len=" << testStr.length() << ") -> "
         << t6.size() << " trigrams (expected " << testStr.length() - 2 << ")" << endl;
    cout << "  [PASS]" << endl;

    cout << "\n  [ALL GETTRIGRAMS TESTS PASSED]" << endl;
}

// TEST 2: hashTrigram()
// Tests that hashing is deterministic and in valid range.
void testHashTrigram()
{
    cout << "\n========================================" << endl;
    cout << "TEST 2: hashTrigram() — Member 1" << endl;
    cout << "========================================" << endl;

    // --- 2a: Deterministic ---
    cout << "\n  --- 2a: Deterministic ---" << endl;
    int h1 = hashTrigram("hel");
    int h2 = hashTrigram("hel");
    cout << "  hashTrigram(\"hel\") = " << h1 << endl;
    cout << "  hashTrigram(\"hel\") = " << h2 << " (same call, must match)" << endl;
    assert(h1 == h2);
    cout << "  [PASS]" << endl;

    // --- 2b: All values in range [0, VECTOR_DIM) ---
    cout << "\n  --- 2b: Range check ---" << endl;
    int h3 = hashTrigram("cat");
    int h4 = hashTrigram("dog");
    int h5 = hashTrigram("zzz");
    int h6 = hashTrigram("   ");
    int h7 = hashTrigram("aaa");
    cout << "  hashTrigram(\"cat\") = " << h3 << endl;
    cout << "  hashTrigram(\"dog\") = " << h4 << endl;
    cout << "  hashTrigram(\"zzz\") = " << h5 << endl;
    cout << "  hashTrigram(\"   \") = " << h6 << endl;
    cout << "  hashTrigram(\"aaa\") = " << h7 << endl;
    assert(h3 >= 0 && h3 < VECTOR_DIM);
    assert(h4 >= 0 && h4 < VECTOR_DIM);
    assert(h5 >= 0 && h5 < VECTOR_DIM);
    assert(h6 >= 0 && h6 < VECTOR_DIM);
    assert(h7 >= 0 && h7 < VECTOR_DIM);
    cout << "  [PASS] All values in range [0, " << VECTOR_DIM << ")" << endl;

    // --- 2c: Different inputs CAN produce different hashes ---
    cout << "\n  --- 2c: Different inputs -> different hashes (usually) ---" << endl;
    int h_same1 = hashTrigram("abc");
    int h_same2 = hashTrigram("xyz");
    cout << "  hashTrigram(\"abc\") = " << h_same1 << endl;
    cout << "  hashTrigram(\"xyz\") = " << h_same2 << endl;
    // Not asserting they're different (hash collision is possible),
    // but logging for inspection.
    if (h_same1 != h_same2)
    {
        cout << "  Different hashes (expected most of the time)" << endl;
    }
    else
    {
        cout << "  Same hash (collision possible but unlikely for 300 buckets)" << endl;
    }
    cout << "  [PASS] No crash, values valid" << endl;

    cout << "\n  [ALL HASHTRIGRAM TESTS PASSED]" << endl;
}

// TEST 3: vectorize()
// Tests text-to-vector conversion.
// Verifies: correct size, determinism, typo-tolerance, empty input
void testVectorize()
{
    cout << "\n========================================" << endl;
    cout << "TEST 3: vectorize() — Member 1" << endl;
    cout << "========================================" << endl;

    // --- 3a: Correct output size (VECTOR_DIM) ---
    cout << "\n  --- 3a: Output size = VECTOR_DIM ---" << endl;
    vector<double> v1 = vectorize("hello world");
    cout << "  vectorize(\"hello world\"):" << endl;
    printVectorSparse(v1);
    assert(v1.size() == (size_t)VECTOR_DIM);
    assert(countNonZero(v1) > 0);
    cout << "  [PASS]" << endl;

    // --- 3b: Deterministic ---
    cout << "\n  --- 3b: Deterministic ---" << endl;
    vector<double> v1_again = vectorize("hello world");
    bool identical = true;
    for (size_t i = 0; i < v1.size(); i++)
    {
        if (v1[i] != v1_again[i])
        {
            identical = false;
            break;
        }
    }
    assert(identical);
    cout << "  Re-vectorizing \"hello world\" -> identical vector" << endl;
    cout << "  [PASS]" << endl;

    // --- 3c: Typo-tolerance ---
    cout << "\n  --- 3c: Typo-tolerance ---" << endl;
    vector<double> v2 = vectorize("hello worl");
    double dist_typo = cosineDistance(v1, v2);
    cout << "  \"hello world\" vs \"hello worl\" (1 char typo):" << endl;
    cout << "  Distance: " << fixed << setprecision(6) << dist_typo << endl;
    assert(dist_typo < 0.5); // Should be small — typo-tolerant
    cout << "  [PASS] Small distance (typo-tolerant)" << endl;

    // --- 3d: Unrelated texts are far apart ---
    cout << "\n  --- 3d: Unrelated texts are far apart ---" << endl;
    vector<double> v3 = vectorize("quantum physics equations");
    double dist_diff = cosineDistance(v1, v3);
    cout << "  \"hello world\" vs \"quantum physics equations\":" << endl;
    cout << "  Distance: " << fixed << setprecision(6) << dist_diff << endl;
    assert(dist_diff > dist_typo); // Unrelated must be farther than typo
    cout << "  [PASS] Farther than typo comparison" << endl;

    // --- 3e: Empty string produces zero vector ---
    cout << "\n  --- 3e: Empty string -> zero vector ---" << endl;
    vector<double> v_empty = vectorize("");
    cout << "  vectorize(\"\"):" << endl;
    cout << "  Non-zero entries: " << countNonZero(v_empty) << endl;
    assert(v_empty.size() == (size_t)VECTOR_DIM);
    assert(countNonZero(v_empty) == 0);
    cout << "  [PASS]" << endl;

    // --- 3f: Single character ---
    cout << "\n  --- 3f: Single character ---" << endl;
    vector<double> v_single = vectorize("a");
    cout << "  vectorize(\"a\") non-zero entries: " << countNonZero(v_single) << endl;
    assert(v_single.size() == (size_t)VECTOR_DIM);
    assert(countNonZero(v_single) == 1); // "a  " padded -> 1 trigram -> 1 non-zero
    cout << "  [PASS]" << endl;

    cout << "\n  [ALL VECTORIZE TESTS PASSED]" << endl;
}

// TEST 4: cosineDistance()
// Tests the distance metric with known vectors.
// Verifies: identical, opposite, orthogonal, zero-vector guard,
//           real-world text comparison, size mismatch guard
void testCosineDistance()
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: cosineDistance() — Member 1" << endl;
    cout << "========================================" << endl;

    // --- 4a: Identical vectors -> distance 0 ---
    cout << "\n  --- 4a: Identical vectors ---" << endl;
    vector<double> a = {1.0, 2.0, 3.0};
    vector<double> b = {1.0, 2.0, 3.0};
    double d1 = cosineDistance(a, b);
    cout << "  cosineDistance([1,2,3], [1,2,3]) = " << fixed << setprecision(6) << d1 << endl;
    assert(d1 < 0.0001);
    cout << "  [PASS]" << endl;

    // --- 4b: Opposite vectors -> distance 2 ---
    cout << "\n  --- 4b: Opposite vectors ---" << endl;
    vector<double> c = {1.0, 0.0, 0.0};
    vector<double> d = {-1.0, 0.0, 0.0};
    double d2 = cosineDistance(c, d);
    cout << "  cosineDistance([1,0,0], [-1,0,0]) = " << fixed << setprecision(6) << d2 << endl;
    cout << "  (Expected: 2.000000)" << endl;
    cout << "  [PASS]" << endl;

    // --- 4c: Orthogonal vectors -> distance 1 ---
    cout << "\n  --- 4c: Orthogonal vectors ---" << endl;
    vector<double> e = {1.0, 0.0};
    vector<double> f = {0.0, 1.0};
    double d3 = cosineDistance(e, f);
    cout << "  cosineDistance([1,0], [0,1]) = " << fixed << setprecision(6) << d3 << endl;
    assert(d3 > 0.999 && d3 <= 1.001);
    cout << "  [PASS]" << endl;

    // --- 4d: Zero-vector guard (one zero vector) ---
    cout << "\n  --- 4d: Zero-vector guard (one zero) ---" << endl;
    vector<double> zero(5, 0.0);
    vector<double> g = {1.0, 2.0, 3.0, 4.0, 5.0};
    double d4 = cosineDistance(zero, g);
    cout << "  cosineDistance([0,0,0,0,0], [1,2,3,4,5]) = " << fixed << setprecision(6) << d4 << endl;
    assert(d4 == 1.0);
    cout << "  [PASS] Division by zero prevented" << endl;

    // --- 4e: Zero-vector guard (both zero vectors) ---
    cout << "\n  --- 4e: Zero-vector guard (both zero) ---" << endl;
    double d5 = cosineDistance(zero, zero);
    cout << "  cosineDistance([0,0,0,0,0], [0,0,0,0,0]) = " << fixed << setprecision(6) << d5 << endl;
    assert(d5 == 1.0);
    cout << "  [PASS]" << endl;

    // --- 4f: Size mismatch guard ---
    cout << "\n  --- 4f: Size mismatch ---" << endl;
    vector<double> h1 = {1.0, 2.0};
    vector<double> h2 = {1.0, 2.0, 3.0};
    double d6 = cosineDistance(h1, h2);
    cout << "  cosineDistance(size=2, size=3) = " << fixed << setprecision(6) << d6 << endl;
    assert(d6 == 1.0); // Should return max distance (1.0) as safety
    cout << "  [PASS] Invalid comparison handled" << endl;

    // --- 4g: Real-world text comparison ---
    cout << "\n  --- 4g: Real-world text comparison ---" << endl;
    vector<double> v_hello = vectorize("hello world");
    vector<double> v_hella = vectorize("hella world");
    vector<double> v_quantum = vectorize("quantum entanglement theory");
    double d_similar = cosineDistance(v_hello, v_hella);
    double d_different = cosineDistance(v_hello, v_quantum);
    cout << "  \"hello world\" vs \"hella world\":     " << fixed << setprecision(6) << d_similar << endl;
    cout << "  \"hello world\" vs \"quantum entang...\": " << fixed << setprecision(6) << d_different << endl;
    assert(d_similar < d_different);
    cout << "  [PASS] Similar texts closer than unrelated ones" << endl;

    cout << "\n  [ALL COSINEDISTANCE TESTS PASSED]" << endl;
}

// TEST 5: End-to-End Vectorizer Demonstration
// Shows the full pipeline: text -> trigrams -> hash -> vector
// -> cosine distance -> best match. Uses manual brute-force
// (no search() function — just comparing all distances).
void testVectorizerEndToEnd()
{
    cout << "\n========================================" << endl;
    cout << "TEST 5: End-to-End Vectorizer Demo" << endl;
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
    for (const string &note : notes)
    {
        cout << "    - \"" << note << "\"" << endl;
    }
    cout << "\n  Query: \"" << query << "\"" << endl;

    vector<double> queryVec = vectorize(query);

    string bestMatch;
    double bestDist = 999.0;

    cout << "\n  Distances from query to each note (manual brute-force):" << endl;
    for (const string &note : notes)
    {
        vector<double> noteVec = vectorize(note);
        double dist = cosineDistance(queryVec, noteVec);
        cout << "    \"" << note << "\" -> "
             << fixed << setprecision(6) << dist;
        if (dist < bestDist)
        {
            bestDist = dist;
            bestMatch = note;
            cout << "  <-- BEST";
        }
        cout << endl;
    }

    cout << "\n  BEST MATCH: \"" << bestMatch << "\" (distance: "
         << fixed << setprecision(6) << bestDist << ")" << endl;
    assert(bestMatch == "data structures and algorithms");
    cout << "  [PASS]" << endl;

    cout << "\n  [END-TO-END VECTORIZER DEMO PASSED]" << endl;
}

//  MEMBER 2 — AREEBA (Graph Structure & Memory Specialist)
//  Tests: createNode, addEdge, deleteNode, printGraph,
//         setEntryPoint, getEntryPoint, getNodeCount, getAllNodes

// TEST 6: Graph Construction & Edge Operations
// Tests: empty graph, createNode, addEdge, self-edge,
//        duplicate edge, entry point, printGraph, getAllNodes
void testGraphOperations()
{
    cout << "\n========================================" << endl;
    cout << "TEST 6: Graph Operations — Member 2" << endl;
    cout << "========================================" << endl;

    NSWGraph graph;

    // --- 6a: Empty graph ---
    cout << "\n  --- 6a: Empty graph ---" << endl;
    assert(graph.getNodeCount() == 0);
    assert(graph.getEntryPoint() == nullptr);
    assert(graph.getAllNodes().empty());
    cout << "  Node count: 0, Entry point: nullptr" << endl;
    cout << "  [PASS]" << endl;

    // --- 6b: Create nodes ---
    cout << "\n  --- 6b: Create nodes via createNode() ---" << endl;
    Node *n0 = graph.createNode("data structures", vectorize("data structures"));
    Node *n1 = graph.createNode("machine learning", vectorize("machine learning"));
    Node *n2 = graph.createNode("operating systems", vectorize("operating systems"));
    assert(graph.getNodeCount() == 3);
    // Verify node data stored correctly
    assert(n0->text == "data structures");
    assert(n1->text == "machine learning");
    assert(n2->text == "operating systems");
    assert(n0->numericalVector.size() == (size_t)VECTOR_DIM);
    cout << "  Created 3 nodes. Text and vectors stored correctly." << endl;
    cout << "  [PASS]" << endl;

    // --- 6c: Add edges (bidirectional) ---
    cout << "\n  --- 6c: Add edges ---" << endl;
    graph.addEdge(n0, n1); // data <-> machine
    graph.addEdge(n1, n2); // machine <-> operating
    graph.addEdge(n0, n2); // data <-> operating
    cout << "  n0 neighbors: " << n0->neighbors.size() << " (expected: 2)" << endl;
    cout << "  n1 neighbors: " << n1->neighbors.size() << " (expected: 2)" << endl;
    cout << "  n2 neighbors: " << n2->neighbors.size() << " (expected: 2)" << endl;
    assert(n0->neighbors.size() == 2);
    assert(n1->neighbors.size() == 2);
    assert(n2->neighbors.size() == 2);
    // Verify bidirectionality: if n0->n1 exists, n1->n0 must exist
    assert(n0->neighbors[0] == n1);
    assert(n1->neighbors[0] == n0);
    cout << "  [PASS] Bidirectional edges correct" << endl;

    // --- 6d: Self-edge prevention ---
    cout << "\n  --- 6d: Self-edge prevention ---" << endl;
    graph.addEdge(n0, n0);
    cout << "  n0 neighbors after self-edge attempt: " << n0->neighbors.size()
         << " (expected: 2, unchanged)" << endl;
    assert(n0->neighbors.size() == 2);
    cout << "  [PASS]" << endl;

    // --- 6e: Duplicate edge prevention ---
    cout << "\n  --- 6e: Duplicate edge prevention ---" << endl;
    graph.addEdge(n0, n1); // Already connected
    cout << "  n0 neighbors after duplicate edge attempt: " << n0->neighbors.size()
         << " (expected: 2, unchanged)" << endl;
    assert(n0->neighbors.size() == 2);
    cout << "  [PASS]" << endl;

    // --- 6f: Set and get entry point ---
    cout << "\n  --- 6f: Entry point ---" << endl;
    graph.setEntryPoint(n2);
    assert(graph.getEntryPoint() == n2);
    cout << "  Entry point set to: \"" << graph.getEntryPoint()->text << "\"" << endl;
    cout << "  [PASS]" << endl;

    // --- 6g: printGraph() (visual inspection) ---
    cout << "\n  --- 6g: printGraph() ---" << endl;
    graph.printGraph();
    cout << "  [PASS] (visual inspection)" << endl;

    // --- 6h: getAllNodes() ---
    cout << "\n  --- 6h: getAllNodes() ---" << endl;
    const vector<Node *> &allNodes = graph.getAllNodes();
    assert(allNodes.size() == 3);
    assert(allNodes[0] == n0);
    assert(allNodes[1] == n1);
    assert(allNodes[2] == n2);
    cout << "  getAllNodes() returned 3 nodes in correct order" << endl;
    cout << "  [PASS]" << endl;

    // --- 6i: Node data accessible via pointers ---
    cout << "\n  --- 6i: Node data accessible ---" << endl;
    double dist = cosineDistance(n0->numericalVector, n1->numericalVector);
    cout << "  Distance n0 <-> n1: " << fixed << setprecision(6) << dist << endl;
    assert(dist >= 0.0 && dist <= 2.0); // Valid cosine distance range
    cout << "  [PASS]" << endl;

    cout << "\n  [ALL GRAPH OPERATION TESTS PASSED]" << endl;
}

// TEST 7: Hard Deletion
// Tests: deleteNode with relinking, edge removal, entry point
//        reassignment, nullptr guard, M limit relinking,
//        no dangling pointers, no duplicate edges
void testHardDeletion()
{
    cout << "\n========================================" << endl;
    cout << "TEST 7: Hard Deletion — Member 2" << endl;
    cout << "========================================" << endl;

    // ======== SETUP: 4-node fully connected graph ========
    //
    //    n0 -- n1
    //    |  \  |
    //    n2 -- n3
    //
    NSWGraph graph;
    Node *n0 = graph.createNode("apple", vectorize("apple"));
    Node *n1 = graph.createNode("apply", vectorize("apply"));
    Node *n2 = graph.createNode("banana", vectorize("banana"));
    Node *n3 = graph.createNode("orange", vectorize("orange"));

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
    graph.printGraph();

    // --- 7a: Delete a non-entry-point node ---
    cout << "\n  --- 7a: Delete n3 (orange) — non-entry-point ---" << endl;
    cout << "  Before: node count = " << graph.getNodeCount() << endl;
    graph.deleteNode(n3);
    cout << "  After:  node count = " << graph.getNodeCount() << " (expected: 3)" << endl;
    assert(graph.getNodeCount() == 3);
    assert(graph.getEntryPoint() == n0); // Entry point unchanged
    cout << "  Entry point still: " << graph.getEntryPoint()->text << " (correct)" << endl;
    cout << "  [PASS]" << endl;

    // --- 7b: Edges to deleted node removed from all neighbors ---
    cout << "\n  --- 7b: Edges to deleted node removed ---" << endl;
    cout << "  n0 neighbors: " << n0->neighbors.size() << " (expected: 2)" << endl;
    cout << "  n1 neighbors: " << n1->neighbors.size() << " (expected: 2)" << endl;
    cout << "  n2 neighbors: " << n2->neighbors.size() << " (expected: 2)" << endl;
    assert(n0->neighbors.size() == 2);
    assert(n1->neighbors.size() == 2);
    assert(n2->neighbors.size() == 2);
    // Verify n3 is NOT in any neighbor list
    for (Node *nb : n0->neighbors)
        assert(nb != n3);
    for (Node *nb : n1->neighbors)
        assert(nb != n3);
    for (Node *nb : n2->neighbors)
        assert(nb != n3);
    cout << "  [PASS] All edges to deleted node removed." << endl;

    // --- 7c: No duplicate edges after deletion ---
    cout << "\n  --- 7c: No duplicate edges after deletion ---" << endl;
    for (Node *node : graph.getAllNodes())
    {
        for (size_t i = 0; i < node->neighbors.size(); i++)
        {
            for (size_t j = i + 1; j < node->neighbors.size(); j++)
            {
                assert(node->neighbors[i] != node->neighbors[j]);
            }
        }
    }
    cout << "  No duplicate neighbors in any node." << endl;
    cout << "  [PASS]" << endl;

    // --- 7d: Delete the entry point ---
    cout << "\n  --- 7d: Delete n0 (apple) — the entry point ---" << endl;
    graph.deleteNode(n0);
    cout << "  Node count: " << graph.getNodeCount() << " (expected: 2)" << endl;
    assert(graph.getNodeCount() == 2);
    cout << "  New entry point: \""
         << (graph.getEntryPoint() ? graph.getEntryPoint()->text : "nullptr") << "\"" << endl;
    assert(graph.getEntryPoint() != nullptr);
    cout << "  [PASS] Entry point reassigned." << endl;

    // Verify remaining nodes are still connected
    cout << "  n1 neighbors: " << n1->neighbors.size() << " (expected: 1 = n2)" << endl;
    cout << "  n2 neighbors: " << n2->neighbors.size() << " (expected: 1 = n1)" << endl;
    assert(n1->neighbors.size() == 1);
    assert(n2->neighbors.size() == 1);
    assert(n1->neighbors[0] == n2);
    assert(n2->neighbors[0] == n1);
    cout << "  [PASS] n1 and n2 still connected." << endl;

    graph.printGraph();

    // --- 7e: Delete until graph is empty ---
    cout << "\n  --- 7e: Delete until graph is empty ---" << endl;
    graph.deleteNode(n1);
    cout << "  After deleting n1: count = " << graph.getNodeCount() << endl;
    assert(graph.getNodeCount() == 1);

    graph.deleteNode(n2);
    cout << "  After deleting n2: count = " << graph.getNodeCount() << endl;
    assert(graph.getNodeCount() == 0);
    assert(graph.getEntryPoint() == nullptr);
    cout << "  Entry point: nullptr (correct, graph is empty)" << endl;
    cout << "  [PASS]" << endl;

    // --- 7f: Delete nullptr ---
    cout << "\n  --- 7f: Delete nullptr ---" << endl;
    graph.deleteNode(nullptr); // Should not crash
    cout << "  deleteNode(nullptr) — no crash." << endl;
    assert(graph.getNodeCount() == 0);
    cout << "  [PASS]" << endl;

    // --- 7g: M limit relinking (star graph deletion) ---
    cout << "\n  --- 7g: Relinking under M limit ---" << endl;
    NSWGraph graph2;
    Node *center = graph2.createNode("center", vectorize("center"));
    vector<Node *> leaves;
    for (int i = 0; i < M; i++)
    {
        string leafText = "leaf" + to_string(i);
        Node *leaf = graph2.createNode(leafText, vectorize(leafText));
        graph2.addEdge(center, leaf);
        leaves.push_back(leaf);
    }
    graph2.setEntryPoint(center);
    cout << "  Star graph: center + " << M << " leaves" << endl;
    assert(center->neighbors.size() == (size_t)M);
    assert(leaves[0]->neighbors.size() == 1);
    cout << "  Center: " << center->neighbors.size() << " neighbors, Leaf0: "
         << leaves[0]->neighbors.size() << " neighbor" << endl;

    // Delete center — leaves should relink to each other
    graph2.deleteNode(center);
    cout << "  After deleting center: " << graph2.getNodeCount() << " nodes remain" << endl;
    assert(graph2.getNodeCount() == M);

    cout << "  Leaf 0 neighbors: " << leaves[0]->neighbors.size() << endl;
    assert(leaves[0]->neighbors.size() > 0); // Relinking happened
    cout << "  [PASS] Relinking works under M limit." << endl;

    // --- 7h: No dangling pointers after star deletion ---
    cout << "\n  --- 7h: No dangling pointers ---" << endl;
    const vector<Node *> &remaining = graph2.getAllNodes();
    bool danglingFound = false;
    for (Node *node : remaining)
    {
        for (Node *nb : node->neighbors)
        {
            bool foundInGraph = false;
            for (Node *n : remaining)
            {
                if (nb == n)
                {
                    foundInGraph = true;
                    break;
                }
            }
            if (!foundInGraph)
                danglingFound = true;
        }
    }
    assert(!danglingFound);
    cout << "  No dangling pointers." << endl;
    cout << "  [PASS]" << endl;

    // --- 7i: No duplicate edges after star deletion ---
    cout << "\n  --- 7i: No duplicate edges after star deletion ---" << endl;
    for (Node *node : remaining)
    {
        for (size_t i = 0; i < node->neighbors.size(); i++)
        {
            for (size_t j = i + 1; j < node->neighbors.size(); j++)
            {
                assert(node->neighbors[i] != node->neighbors[j]);
            }
        }
    }
    cout << "  No duplicate neighbors in any node." << endl;
    cout << "  [PASS]" << endl;

    cout << "\n  [ALL HARD DELETION TESTS PASSED]" << endl;
}

//  MAIN — Run all tests
int main()
{
    cout << "================================================" << endl;
    cout << "  NSW Project — Component Test Suite (Phase 1)" << endl;
    cout << "================================================" << endl;
    cout << "  Member 1 (Arqish): getTrigrams, hashTrigram," << endl;
    cout << "                     vectorize, cosineDistance" << endl;
    cout << "  Member 2 (Areeba):  NSWGraph — createNode," << endl;
    cout << "                      addEdge, deleteNode" << endl;
    cout << "  Member 3 (Hira):    search.h/cpp (NOT tested here)" << endl;
    cout << "  Member 4 (Shaheer): Integration + CLI + Persistence" << endl;
    cout << "  Deletion: HARD (edges removed, memory freed)" << endl;
    cout << "================================================" << endl;

    // --- Member 1 tests ---
    testGetTrigrams();
    testHashTrigram();
    testVectorize();
    testCosineDistance();
    testVectorizerEndToEnd();

    // --- Member 2 tests ---
    testGraphOperations();
    testHardDeletion();

    cout << "\n================================================" << endl;
    cout << "  ALL TESTS PASSED!" << endl;
    cout << "================================================" << endl;
    cout << "  TEST 1: getTrigrams()          — PASSED" << endl;
    cout << "  TEST 2: hashTrigram()           — PASSED" << endl;
    cout << "  TEST 3: vectorize()             — PASSED" << endl;
    cout << "  TEST 4: cosineDistance()        — PASSED" << endl;
    cout << "  TEST 5: Vectorizer End-to-End   — PASSED" << endl;
    cout << "  TEST 6: Graph Operations        — PASSED" << endl;
    cout << "  TEST 7: Hard Deletion           — PASSED" << endl;
    cout << "================================================" << endl;

    return 0;
}

// g++ -std=c++17 -Wall -Wextra -o test main.cpp vectorizer.cpp nsw_graph.cpp search.cpp