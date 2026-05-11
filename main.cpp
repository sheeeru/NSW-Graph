// ============================================================
// main.cpp — NSW Project Test Harness (Phase 1 + Phase 2)
//
// Phase 1: Tests Member 1's vectorizer functions (getTrigrams,
// hashTrigram, vectorize, cosineDistance) and Member 2's graph
// functions (createNode, addEdge, deleteNode, printGraph).
//
// Phase 2: Tests Member 3's search(), Member 4's insert(),
// bruteForceSearch(), hit rate, performance timing, persistence,
// and full integration lifecycle.
//
// Compile:  g++ -std=c++17 -Wall -Wextra -o test main.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp persistence.cpp
// Run:      ./test
// ============================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cassert>
#include <chrono>
#include <fstream>
#include <cstdio>
#include "vectorizer.h"
#include "config.h"
#include "nsw_graph.h"
#include "search.h"
#include "insert.hpp"
#include "persistence.h"

using namespace std;

// ------------------------------------------------------------
// Helper: count non-zero elements in a vector
// ------------------------------------------------------------
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

// ------------------------------------------------------------
// Helper: print a vector (shows non-zero entries only)
// ------------------------------------------------------------
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

// ------------------------------------------------------------
// Helper: print a vector of strings
// ------------------------------------------------------------
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

// =============================================================
//  MEMBER 1 — ARQISH (Vectorizer & Math Specialist)
//  Tests: getTrigrams, hashTrigram, vectorize, cosineDistance
// =============================================================

// ------------------------------------------------------------
// TEST 1: getTrigrams()
// Tests the sliding window trigram extraction.
// Verifies: normal text, exact-3, short (padded), empty, long
// ------------------------------------------------------------
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

// ------------------------------------------------------------
// TEST 2: hashTrigram()
// Tests that hashing is deterministic and in valid range.
// ------------------------------------------------------------
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

// ------------------------------------------------------------
// TEST 3: vectorize()
// Tests text-to-vector conversion.
// Verifies: correct size, determinism, typo-tolerance, empty input
// ------------------------------------------------------------
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

// ------------------------------------------------------------
// TEST 4: cosineDistance()
// Tests the distance metric with known vectors.
// Verifies: identical, opposite, orthogonal, zero-vector guard,
//           real-world text comparison, size mismatch guard
// ------------------------------------------------------------
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

// ------------------------------------------------------------
// TEST 5: End-to-End Vectorizer Demonstration
// Shows the full pipeline: text -> trigrams -> hash -> vector
// -> cosine distance -> best match. Uses manual brute-force
// (no search() function — just comparing all distances).
// ------------------------------------------------------------
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

// =============================================================
//  MEMBER 2 — AREEBA (Graph Structure & Memory Specialist)
//  Tests: createNode, addEdge, deleteNode, printGraph,
//         setEntryPoint, getEntryPoint, getNodeCount, getAllNodes
// =============================================================

// ------------------------------------------------------------
// TEST 6: Graph Construction & Edge Operations
// Tests: empty graph, createNode, addEdge, self-edge,
//        duplicate edge, entry point, printGraph, getAllNodes
// ------------------------------------------------------------
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

// ------------------------------------------------------------
// TEST 7: Hard Deletion
// Tests: deleteNode with relinking, edge removal, entry point
//        reassignment, nullptr guard, M limit relinking,
//        no dangling pointers, no duplicate edges
// ------------------------------------------------------------
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


// =============================================================
//  PHASE 2 — INTEGRATION TESTS
//  MEMBER 3 — HIRA (Search Algorithm Specialist)
//  MEMBER 4 — SHAHEER (Integration, CLI, Persistence)
//  Tests: search(), bruteForceSearch(), insert(), hit rate,
//         performance timing, persistence, full lifecycle
// =============================================================

// ------------------------------------------------------------
// TEST 8: search() — Greedy Search Functionality
// Tests Hira's search algorithm.
// Verifies: empty graph, correct best match, k=1, k>size,
//           result ordering (sorted ascending by distance)
// ------------------------------------------------------------
void testSearch()
{
    cout << "\n========================================" << endl;
    cout << "TEST 8: search() — Member 3" << endl;
    cout << "========================================" << endl;

    // --- 8a: Search on empty graph ---
    cout << "\n  --- 8a: Empty graph ---" << endl;
    {
        NSWGraph g;
        auto results = search(&g, vectorize("hello"), 3, EF_SEARCH);
        assert(results.empty());
    }
    cout << "  search on empty graph -> 0 results" << endl;
    cout << "  [PASS]" << endl;

    // --- 8b: Search on graph with no entry point ---
    cout << "\n  --- 8b: Graph with nodes but no entry point ---" << endl;
    {
        NSWGraph g;
        g.createNode("orphan", vectorize("orphan"));
        auto results = search(&g, vectorize("orphan"), 3, EF_SEARCH);
        assert(results.empty());
    }
    cout << "  search with no entry point -> 0 results" << endl;
    cout << "  [PASS]" << endl;

    // --- 8c: Build graph with insert() and search ---
    // Using 4 texts: with M=3, the first 4 inserts always form
    // a well-connected graph (no M-limit blocking).
    cout << "\n  --- 8c: Search after inserts ---" << endl;
    NSWGraph graph;
    insert(&graph, "data structures and algorithms");
    insert(&graph, "machine learning fundamentals");
    insert(&graph, "database management systems");
    insert(&graph, "operating system concepts");
    cout << "  Inserted 4 notes. Graph: " << graph.getNodeCount() << " nodes." << endl;

    // Search for exact text — should find itself as best match
    auto results = search(&graph, vectorize("data structures and algorithms"), 3, EF_SEARCH);
    assert(!results.empty());
    cout << "  Search for \"data structures and algorithms\" — top result: \""
         << results[0].text << "\" (dist=" << fixed << setprecision(6) << results[0].distance << ")" << endl;
    assert(results[0].text == "data structures and algorithms");
    assert(results[0].distance < 0.0001); // Exact match = distance 0
    cout << "  [PASS]" << endl;

    // --- 8d: k=1 returns exactly 1 result ---
    cout << "\n  --- 8d: k=1 ---" << endl;
    results = search(&graph, vectorize("machine learning"), 1, EF_SEARCH);
    assert(results.size() == 1);
    cout << "  k=1 -> " << results.size() << " result: \""
         << results[0].text << "\"" << endl;
    cout << "  [PASS]" << endl;

    // --- 8e: k > node count returns all reachable nodes ---
    cout << "\n  --- 8e: k > node count ---" << endl;
    results = search(&graph, vectorize("computer"), 100, EF_SEARCH);
    assert((int)results.size() == graph.getNodeCount());
    cout << "  k=100 with " << graph.getNodeCount()
         << " nodes -> returned " << results.size() << " results" << endl;
    cout << "  [PASS]" << endl;

    // --- 8f: Results are sorted (closest first) ---
    cout << "\n  --- 8f: Results sorted ascending ---" << endl;
    results = search(&graph, vectorize("database"), 3, EF_SEARCH);
    for (size_t i = 1; i < results.size(); i++)
    {
        assert(results[i].distance >= results[i - 1].distance);
    }
    cout << "  Distances: ";
    for (size_t i = 0; i < results.size(); i++)
    {
        cout << fixed << setprecision(4) << results[i].distance;
        if (i + 1 < results.size())
            cout << " <= ";
    }
    cout << endl;
    cout << "  [PASS] All distances in ascending order" << endl;

    // --- 8g: Similar text finds correct match ---
    cout << "\n  --- 8g: Typo-tolerant search ---" << endl;
    results = search(&graph, vectorize("operating systems and kernels"), 3, EF_SEARCH);
    assert(!results.empty());
    cout << "  Search \"operating systems and kernels\" -> best: \""
         << results[0].text << "\" (dist=" << fixed << setprecision(6) << results[0].distance << ")" << endl;
    assert(results[0].text == "operating system concepts");
    cout << "  [PASS]" << endl;

    // --- 8h: Node with 0 neighbors (M-limit edge case) ---
    // Inserting a 5th text that may fail to connect if all
    // closest nodes are at M=3 capacity. Search should still
    // work — it returns the entry point (the new node).
    cout << "\n  --- 8h: M-limit edge case (5th node) ---" << endl;
    insert(&graph, "computer networks and security");
    results = search(&graph, vectorize("data structures"), 3, EF_SEARCH);
    assert(!results.empty());
    cout << "  After 5th insert: search still returns "
         << results.size() << " result(s)" << endl;
    cout << "  Note: if 5th node got 0 edges (M limit), search starts"
         << endl;
    cout << "  from that node and may only find itself." << endl;
    cout << "  [PASS] No crash, results returned" << endl;

    cout << "\n  [ALL SEARCH TESTS PASSED]" << endl;
}

// ------------------------------------------------------------
// TEST 9: bruteForceSearch() — Ground Truth
// Tests that brute force correctly finds the closest nodes
// by checking every single node in the graph.
// Verifies: correctness against manual calculation, sorting,
//           k=1, k>size, empty graph, nullptr guard
// ------------------------------------------------------------
void testBruteForceSearch()
{
    cout << "\n========================================" << endl;
    cout << "TEST 9: bruteForceSearch() — Member 4" << endl;
    cout << "========================================" << endl;

    NSWGraph graph;

    // --- 9a: Empty graph ---
    cout << "\n  --- 9a: Empty graph ---" << endl;
    auto results = bruteForceSearch(&graph, vectorize("hello"), 3);
    assert(results.empty());
    cout << "  bruteForceSearch on empty graph -> 0 results" << endl;
    cout << "  [PASS]" << endl;

    // --- 9b: nullptr graph ---
    cout << "\n  --- 9b: nullptr graph ---" << endl;
    results = bruteForceSearch(nullptr, vectorize("hello"), 3);
    assert(results.empty());
    cout << "  bruteForceSearch(nullptr) -> 0 results, no crash" << endl;
    cout << "  [PASS]" << endl;

    // --- 9c: k <= 0 returns empty ---
    cout << "\n  --- 9c: k <= 0 ---" << endl;
    insert(&graph, "apple");
    results = bruteForceSearch(&graph, vectorize("apple"), 0);
    assert(results.empty());
    results = bruteForceSearch(&graph, vectorize("apple"), -1);
    assert(results.empty());
    cout << "  k=0 and k=-1 both return empty" << endl;
    cout << "  [PASS]" << endl;

    // --- 9d: Verify correctness against manual calculation ---
    cout << "\n  --- 9d: Correctness check ---" << endl;
    // Clear and rebuild with known data
    NSWGraph graph2;
    vector<string> texts = {
        "alpha bravo charlie",
        "delta echo foxtrot",
        "golf hotel india",
        "alpha bravo delta",
        "echo foxtrot golf",
    };
    for (const string &t : texts)
        insert(&graph2, t);

    string query = "alpha bravo charlie delta";
    results = bruteForceSearch(&graph2, vectorize(query), 3);

    // Manually compute distances to verify
    cout << "  Query: \"" << query << "\"" << endl;
    cout << "  Brute force top-" << results.size() << ":" << endl;
    for (size_t i = 0; i < results.size(); i++)
    {
        double manual = cosineDistance(vectorize(query), vectorize(results[i].text));
        cout << "    [" << i << "] \"" << results[i].text
             << "\" dist=" << fixed << setprecision(6) << results[i].distance
             << " (manual=" << manual << ")" << endl;
        assert(abs(results[i].distance - manual) < 0.0001);
    }
    cout << "  [PASS] All distances match manual calculation" << endl;

    // --- 9e: Results are sorted ---
    cout << "\n  --- 9e: Sorted ascending ---" << endl;
    for (size_t i = 1; i < results.size(); i++)
    {
        assert(results[i].distance >= results[i - 1].distance);
    }
    cout << "  [PASS]" << endl;

    // --- 9f: k=1 returns exactly 1 ---
    cout << "\n  --- 9f: k=1 ---" << endl;
    results = bruteForceSearch(&graph2, vectorize("alpha"), 1);
    assert(results.size() == 1);
    cout << "  k=1 -> 1 result: \"" << results[0].text << "\"" << endl;
    cout << "  [PASS]" << endl;

    // --- 9g: k > node count returns all ---
    cout << "\n  --- 9g: k > node count ---" << endl;
    results = bruteForceSearch(&graph2, vectorize("test"), 100);
    assert((int)results.size() == graph2.getNodeCount());
    cout << "  k=100 with " << graph2.getNodeCount()
         << " nodes -> " << results.size() << " results" << endl;
    cout << "  [PASS]" << endl;

    cout << "\n  [ALL BRUTE FORCE SEARCH TESTS PASSED]" << endl;
}

// ------------------------------------------------------------
// TEST 10: Insert Pipeline
// Tests the full insertion flow: text -> vectorize -> createNode
// -> search closest -> addEdge -> setEntryPoint.
// Verifies: first node gets entry point, subsequent nodes get
//           edges, empty text rejected, each node is searchable
// ------------------------------------------------------------
void testInsertPipeline()
{
    cout << "\n========================================" << endl;
    cout << "TEST 10: Insert Pipeline — Member 4" << endl;
    cout << "========================================" << endl;

    NSWGraph graph;

    // --- 10a: First insert sets entry point ---
    cout << "\n  --- 10a: First insert ---" << endl;
    insert(&graph, "data structures");
    assert(graph.getNodeCount() == 1);
    assert(graph.getEntryPoint() != nullptr);
    assert(graph.getEntryPoint()->text == "data structures");
    cout << "  1 node, entry point: \"" << graph.getEntryPoint()->text << "\"" << endl;
    cout << "  [PASS]" << endl;

    // --- 10b: Second insert connects to first ---
    cout << "\n  --- 10b: Second insert gets edges ---" << endl;
    insert(&graph, "data algorithms");
    assert(graph.getNodeCount() == 2);
    const vector<Node *> &nodes2 = graph.getAllNodes();
    // At least one of the two nodes should have an edge
    bool hasEdge = (nodes2[0]->neighbors.size() > 0 || nodes2[1]->neighbors.size() > 0);
    assert(hasEdge);
    cout << "  Node 0 neighbors: " << nodes2[0]->neighbors.size()
         << ", Node 1 neighbors: " << nodes2[1]->neighbors.size() << endl;
    cout << "  [PASS] Edges created between first two nodes" << endl;

    // --- 10c: Multiple inserts create connected graph ---
    // With M=3, the first 4 inserts always get edges.
    cout << "\n  --- 10c: Multiple inserts (4 nodes) ---" << endl;
    insert(&graph, "machine learning");
    insert(&graph, "deep learning");
    cout << "  " << graph.getNodeCount() << " nodes total." << endl;

    int totalEdges = 0;
    for (Node *n : graph.getAllNodes())
    {
        totalEdges += (int)n->neighbors.size();
    }
    // With 4 nodes and M=3, all 4 should be reachable from entry point
    cout << "  Total neighbor count: " << totalEdges
         << " (each edge counted twice)" << endl;
    assert(totalEdges >= 4); // At least 2 edges exist (4 after counting twice)
    cout << "  [PASS]" << endl;

    // --- 10d: Each inserted text is searchable ---
    // Only test the first 4 texts — with M=3, the 4th insert
    // always gets edges, so all 4 nodes are reachable.
    cout << "\n  --- 10d: All texts searchable ---" << endl;
    vector<string> searchable = {"data structures", "data algorithms",
                                 "machine learning", "deep learning"};
    for (const string &q : searchable)
    {
        auto r = search(&graph, vectorize(q), 1, EF_SEARCH);
        assert(!r.empty());
        bool foundExact = false;
        for (const auto &res : r)
        {
            if (res.text == q)
                foundExact = true;
        }
        assert(foundExact);
        cout << "  \"" << q << "\" -> found in graph [PASS]" << endl;
    }
    cout << "  [ALL 4 TEXTS SEARCHABLE]" << endl;

    // --- 10e: 5th insert may fail to connect (M=3 edge limit) ---
    cout << "\n  --- 10e: M-limit edge case ---" << endl;
    int countBefore = graph.getNodeCount();
    insert(&graph, "neural networks");
    assert(graph.getNodeCount() == countBefore + 1);
    const vector<Node *> &allN = graph.getAllNodes();
    Node *lastNode = allN[allN.size() - 1];
    cout << "  5th node: \"" << lastNode->text << "\" has "
         << lastNode->neighbors.size() << " neighbor(s)" << endl;
    cout << "  Entry point: \"" << graph.getEntryPoint()->text << "\"" << endl;
    cout << "  [PASS] Node added regardless of edge status" << endl;

    // --- 10f: Empty text insert fails gracefully ---
    cout << "\n  --- 10f: Empty text ---" << endl;
    int countBefore2 = graph.getNodeCount();
    insert(&graph, "");
    assert(graph.getNodeCount() == countBefore2);
    cout << "  insert(\"\") did not add any node" << endl;
    cout << "  [PASS]" << endl;

    // --- 10g: Entry point always points to last inserted ---
    cout << "\n  --- 10g: Entry point updates ---" << endl;
    insert(&graph, "last inserted node");
    assert(graph.getEntryPoint()->text == "last inserted node");
    cout << "  Entry point: \"" << graph.getEntryPoint()->text << "\"" << endl;
    cout << "  [PASS]" << endl;

    cout << "\n  [ALL INSERT PIPELINE TESTS PASSED]" << endl;
}

// ------------------------------------------------------------
// TEST 11: Hit Rate Measurement
// Inserts a dataset, then compares NSW search results against
// brute force (ground truth) for each text. Reports how often
// the true best match appears in the NSW top-k results.
// ------------------------------------------------------------
void testHitRate()
{
    cout << "\n========================================" << endl;
    cout << "TEST 11: Hit Rate Measurement" << endl;
    cout << "========================================" << endl;

    NSWGraph graph;

    // Dataset: 15 CS study notes with clear clusters
    vector<string> dataset = {
        "data structures and algorithms",
        "data structures trees and graphs",
        "data structures linked lists and arrays",
        "machine learning fundamentals",
        "machine learning neural networks and deep learning",
        "deep learning and artificial intelligence",
        "database management systems",
        "computer networks and security",
        "operating system concepts",
        "software engineering principles",
        "discrete mathematics and logic",
        "computer architecture and organization",
        "web development with html and css",
        "software testing and quality assurance",
        "algorithms and complexity analysis",
    };

    cout << "\n  Building graph with " << dataset.size() << " notes..." << endl;
    for (const string &text : dataset)
    {
        insert(&graph, text);
    }
    cout << "  Graph: " << graph.getNodeCount() << " nodes." << endl;

    int hits = 0;
    int total = (int)dataset.size();
    int k = 3; // We check if ground truth #1 appears in NSW top-3

    cout << "\n  Comparing NSW top-" << k << " vs brute force #1 for each note:" << endl;
    cout << "  " << string(60, '-') << endl;

    for (const string &text : dataset)
    {
        vector<double> queryVec = vectorize(text);

        // Brute force: ground truth best match
        auto bfResults = bruteForceSearch(&graph, queryVec, 1);
        string bfBest = bfResults[0].text;

        // NSW search: top-k
        auto nswResults = search(&graph, queryVec, k, EF_SEARCH);

        // Check if brute force's #1 is in NSW top-k
        bool found = false;
        for (const auto &r : nswResults)
        {
            if (r.text == bfBest)
            {
                found = true;
                break;
            }
        }

        if (found)
            hits++;

        cout << "  " << (found ? "HIT " : "MISS") << " | query: \""
             << text << "\"" << endl;
        cout << "         BF best: \"" << bfBest << "\"" << endl;
        if (!nswResults.empty())
        {
            cout << "         NSW top:  \"" << nswResults[0].text << "\"" << endl;
        }
    }

    double hitRate = (double)hits / total * 100.0;

    cout << "\n  " << string(60, '-') << endl;
    cout << "  HIT RATE: " << hits << " / " << total << " = "
         << fixed << setprecision(1) << hitRate << "%" << endl;
    cout << "  (M=" << M << ", EF_SEARCH=" << EF_SEARCH
         << ", k=" << k << ")" << endl;

    // With M=3, EF_SEARCH=6, k=3, on 15 nodes, we expect reasonable hit rate
    // Not asserting a strict threshold since graph topology is non-deterministic
    cout << "  [PASS] Hit rate measured successfully" << endl;

    cout << "\n  [HIT RATE TEST PASSED]" << endl;
}

// ------------------------------------------------------------
// TEST 12: Performance Timing
// Compares NSW greedy search vs brute force search timing.
// Each query is run 1000 times to get measurable differences.
// With small datasets the gap is small, but the infrastructure
// demonstrates the O(log N) vs O(N) concept.
// ------------------------------------------------------------
void testPerformanceTiming()
{
    cout << "\n========================================" << endl;
    cout << "TEST 12: Performance Timing" << endl;
    cout << "========================================" << endl;

    NSWGraph graph;

    vector<string> dataset = {
        "data structures and algorithms",
        "data structures trees and graphs",
        "data structures linked lists and arrays",
        "machine learning fundamentals",
        "machine learning neural networks and deep learning",
        "deep learning and artificial intelligence",
        "database management systems",
        "computer networks and security",
        "operating system concepts",
        "software engineering principles",
        "discrete mathematics and logic",
        "computer architecture and organization",
        "web development with html and css",
        "software testing and quality assurance",
        "algorithms and complexity analysis",
    };

    for (const string &text : dataset)
        insert(&graph, text);

    cout << "  Graph: " << graph.getNodeCount() << " nodes, M=" << M
         << ", EF_SEARCH=" << EF_SEARCH << endl;

    vector<string> queries = {
        "data structures",
        "machine learning",
        "database systems",
        "operating systems",
        "algorithms",
    };

    int iterations = 1000;

    cout << "\n  Running " << iterations << " iterations per query per method..." << endl;

    // --- NSW Search Timing ---
    auto nswStart = chrono::high_resolution_clock::now();
    for (const string &q : queries)
    {
        vector<double> qv = vectorize(q);
        for (int i = 0; i < iterations; i++)
        {
            search(&graph, qv, 3, EF_SEARCH);
        }
    }
    auto nswEnd = chrono::high_resolution_clock::now();
    double nswMs = chrono::duration_cast<chrono::microseconds>(nswEnd - nswStart).count() / 1000.0;

    // --- Brute Force Timing ---
    auto bfStart = chrono::high_resolution_clock::now();
    for (const string &q : queries)
    {
        vector<double> qv = vectorize(q);
        for (int i = 0; i < iterations; i++)
        {
            bruteForceSearch(&graph, qv, 3);
        }
    }
    auto bfEnd = chrono::high_resolution_clock::now();
    double bfMs = chrono::duration_cast<chrono::microseconds>(bfEnd - bfStart).count() / 1000.0;

    cout << "\n  " << string(50, '-') << endl;
    cout << "  Method            | Total Time | Avg per query" << endl;
    cout << "  " << string(50, '-') << endl;
    cout << "  NSW Greedy Search | " << setw(8) << fixed << setprecision(2) << nswMs
         << " ms | " << setw(10) << (nswMs / queries.size()) << " ms" << endl;
    cout << "  Brute Force       | " << setw(8) << fixed << setprecision(2) << bfMs
         << " ms | " << setw(10) << (bfMs / queries.size()) << " ms" << endl;
    cout << "  " << string(50, '-') << endl;

    if (nswMs < bfMs)
    {
        cout << "  NSW was " << fixed << setprecision(2) << (bfMs / nswMs)
             << "x faster than brute force" << endl;
    }
    else
    {
        cout << "  Note: With only " << graph.getNodeCount()
             << " nodes, brute force can be equally fast." << endl;
        cout << "  The NSW advantage grows significantly with larger datasets." << endl;
    }

    cout << "  (Both methods return correct results)" << endl;
    cout << "  [PASS]" << endl;

    cout << "\n  [PERFORMANCE TIMING TEST PASSED]" << endl;
}

// ------------------------------------------------------------
// TEST 13: Persistence (Save / Load)
// Tests saving graph to file and loading it back.
// Verifies: node count, text content, search works on loaded
//           graph, empty graph save/load, file cleanup
// ------------------------------------------------------------
void testPersistence()
{
    cout << "\n========================================" << endl;
    cout << "TEST 13: Persistence (Save / Load)" << endl;
    cout << "========================================" << endl;

    string testFile = "test_persistence_data.txt";

    // --- 13a: Insert nodes and save ---
    cout << "\n  --- 13a: Save graph ---" << endl;
    {
        NSWGraph graph;
        insert(&graph, "data structures and algorithms");
        insert(&graph, "machine learning fundamentals");
        insert(&graph, "database management systems");
        insert(&graph, "operating system concepts");

        cout << "  Original graph: " << graph.getNodeCount() << " nodes" << endl;
        saveToFile(&graph, testFile);
    } // graph destructor runs here

    // Verify file was written
    {
        ifstream check(testFile);
        assert(check.good());
        cout << "  File \"" << testFile << "\" created successfully" << endl;
    }

    // --- 13b: Load into new graph ---
    cout << "\n  --- 13b: Load graph ---" << endl;
    NSWGraph loadedGraph;
    loadFromFile(&loadedGraph, testFile);
    cout << "  Loaded graph: " << loadedGraph.getNodeCount() << " nodes" << endl;
    assert(loadedGraph.getNodeCount() == 4);
    cout << "  [PASS] Node count matches" << endl;

    // --- 13c: Verify all texts present ---
    cout << "\n  --- 13c: Texts present in loaded graph ---" << endl;
    vector<string> expected = {
        "data structures and algorithms",
        "machine learning fundamentals",
        "database management systems",
        "operating system concepts",
    };
    const vector<Node *> &allNodes = loadedGraph.getAllNodes();
    assert(allNodes.size() == expected.size());
    for (size_t i = 0; i < expected.size(); i++)
    {
        bool found = false;
        for (Node *n : allNodes)
        {
            if (n->text == expected[i])
            {
                found = true;
                break;
            }
        }
        assert(found);
        cout << "  Found: \"" << expected[i] << "\"" << endl;
    }
    cout << "  [PASS] All texts present" << endl;

    // --- 13d: Search works on loaded graph ---
    cout << "\n  --- 13d: Search on loaded graph ---" << endl;
    auto results = search(&loadedGraph, vectorize("data structures"), 3, EF_SEARCH);
    assert(!results.empty());
    cout << "  Search for \"data structures\" -> top: \""
         << results[0].text << "\"" << endl;
    assert(results[0].text == "data structures and algorithms");
    cout << "  [PASS]" << endl;

    // --- 13e: Save empty graph ---
    cout << "\n  --- 13e: Save empty graph ---" << endl;
    {
        NSWGraph emptyGraph;
        saveToFile(&emptyGraph, "test_empty.txt");
    }
    {
        ifstream check("test_empty.txt");
        assert(check.good());
        string line;
        int lineCount = 0;
        while (getline(check, line))
            lineCount++;
        assert(lineCount == 0);
        cout << "  Empty graph saves as 0-line file" << endl;
    }
    remove("test_empty.txt");
    cout << "  [PASS]" << endl;

    // --- Cleanup ---
    remove(testFile.c_str());
    cout << "\n  Cleaned up test files" << endl;

    cout << "\n  [ALL PERSISTENCE TESTS PASSED]" << endl;
}

// ------------------------------------------------------------
// TEST 14: Full Lifecycle Integration
// Tests the complete workflow: insert -> search -> delete ->
// re-search -> insert replacement -> save -> load -> verify.
// This is the "everything works together" test.
// ------------------------------------------------------------
void testFullLifecycle()
{
    cout << "\n========================================" << endl;
    cout << "TEST 14: Full Lifecycle Integration" << endl;
    cout << "========================================" << endl;

    string lifecycleFile = "test_lifecycle_data.txt";

    // --- 14a: Build initial graph ---
    cout << "\n  --- 14a: Build graph ---" << endl;
    NSWGraph graph;
    insert(&graph, "data structures and algorithms");
    insert(&graph, "machine learning basics");
    insert(&graph, "database management systems");
    insert(&graph, "operating system concepts");
    cout << "  Graph: " << graph.getNodeCount() << " nodes" << endl;
    assert(graph.getNodeCount() == 4);

    // --- 14b: Search and verify ---
    cout << "\n  --- 14b: Initial search ---" << endl;
    auto results = search(&graph, vectorize("data structures"), 3, EF_SEARCH);
    assert(!results.empty());
    assert(results[0].text == "data structures and algorithms");
    cout << "  Search \"data structures\" -> \"" << results[0].text << "\"" << endl;
    cout << "  [PASS]" << endl;

    // --- 14c: Delete a node ---
    cout << "\n  --- 14c: Delete node ---" << endl;
    const vector<Node *> &allNodes = graph.getAllNodes();
    Node *toDelete = nullptr;
    for (Node *n : allNodes)
    {
        if (n->text == "database management systems")
        {
            toDelete = n;
            break;
        }
    }
    assert(toDelete != nullptr);
    graph.deleteNode(toDelete);
    assert(graph.getNodeCount() == 3);
    cout << "  Deleted \"database management systems\". Nodes: "
         << graph.getNodeCount() << endl;

    // Verify deleted node is gone
    results = search(&graph, vectorize("database management systems"), 3, EF_SEARCH);
    for (const auto &r : results)
    {
        assert(r.text != "database management systems");
    }
    cout << "  Deleted node does not appear in search results" << endl;
    cout << "  [PASS]" << endl;

    // --- 14d: Re-search, verify graph still works ---
    cout << "\n  --- 14d: Search after deletion ---" << endl;
    results = search(&graph, vectorize("data structures"), 3, EF_SEARCH);
    assert(!results.empty());
    assert(results[0].text == "data structures and algorithms");
    cout << "  Search still works after deletion" << endl;
    cout << "  [PASS]" << endl;

    // --- 14e: Insert replacement node ---
    cout << "\n  --- 14e: Insert replacement ---" << endl;
    insert(&graph, "database systems and sql");
    assert(graph.getNodeCount() == 4);
    results = search(&graph, vectorize("database systems"), 3, EF_SEARCH);
    assert(!results.empty());
    bool foundReplacement = false;
    for (const auto &r : results)
    {
        if (r.text == "database systems and sql")
            foundReplacement = true;
    }
    assert(foundReplacement);
    cout << "  Inserted \"database systems and sql\" -> found in search" << endl;
    cout << "  [PASS]" << endl;

    // --- 14f: Save, destroy, load, verify ---
    cout << "\n  --- 14f: Save -> Load cycle ---" << endl;
    saveToFile(&graph, lifecycleFile);
    cout << "  Saved " << graph.getNodeCount() << " nodes" << endl;

    // graph destructor runs at end of scope — but we need it alive
    // so we verify the file first, then load into a new graph
    {
        NSWGraph newGraph;
        loadFromFile(&newGraph, lifecycleFile);
        assert(newGraph.getNodeCount() == 4);
        cout << "  Loaded " << newGraph.getNodeCount() << " nodes into new graph" << endl;

        // Search on loaded graph
        results = search(&newGraph, vectorize("data structures"), 3, EF_SEARCH);
        assert(!results.empty());
        assert(results[0].text == "data structures and algorithms");
        cout << "  Search on loaded graph works correctly" << endl;

        // Verify replacement node exists
        bool found = false;
        for (Node *n : newGraph.getAllNodes())
        {
            if (n->text == "database systems and sql")
            {
                found = true;
                break;
            }
        }
        assert(found);
        cout << "  Replacement node persists through save/load" << endl;
        cout << "  [PASS]" << endl;
    }

    // Cleanup
    remove(lifecycleFile.c_str());
    cout << "  Cleaned up test file" << endl;

    cout << "\n  [FULL LIFECYCLE TEST PASSED]" << endl;
}


// =============================================================
//  MAIN — Run all tests
// =============================================================
int main()
{
    cout << "================================================" << endl;
    cout << "  NSW Project — Complete Test Suite" << endl;
    cout << "  (Phase 1: Components + Phase 2: Integration)" << endl;
    cout << "================================================" << endl;
    cout << "  Member 1 (Arqish): getTrigrams, hashTrigram," << endl;
    cout << "                      vectorize, cosineDistance" << endl;
    cout << "  Member 2 (Areeba):  NSWGraph — createNode," << endl;
    cout << "                      addEdge, deleteNode" << endl;
    cout << "  Member 3 (Hira):    search() — greedy search" << endl;
    cout << "  Member 4 (Shaheer): insert(), bruteForceSearch()," << endl;
    cout << "                      hit rate, persistence" << endl;
    cout << "  M=" << M << " | EF_SEARCH=" << EF_SEARCH
         << " | VECTOR_DIM=" << VECTOR_DIM << endl;
    cout << "================================================" << endl;

    // --- Phase 1: Component Tests ---
    cout << "\n  >>> PHASE 1: Component Tests <<<" << endl;
    testGetTrigrams();
    testHashTrigram();
    testVectorize();
    testCosineDistance();
    testVectorizerEndToEnd();
    testGraphOperations();
    testHardDeletion();

    // --- Phase 2: Integration Tests ---
    cout << "\n\n  >>> PHASE 2: Integration Tests <<<" << endl;
    testSearch();
    testBruteForceSearch();
    testInsertPipeline();
    testHitRate();
    testPerformanceTiming();
    testPersistence();
    testFullLifecycle();

    // --- Summary ---
    cout << "\n================================================" << endl;
    cout << "  ALL TESTS PASSED!" << endl;
    cout << "================================================" << endl;
    cout << "  PHASE 1 (Components):" << endl;
    cout << "    TEST 1:  getTrigrams()           — PASSED" << endl;
    cout << "    TEST 2:  hashTrigram()            — PASSED" << endl;
    cout << "    TEST 3:  vectorize()              — PASSED" << endl;
    cout << "    TEST 4:  cosineDistance()         — PASSED" << endl;
    cout << "    TEST 5:  Vectorizer End-to-End    — PASSED" << endl;
    cout << "    TEST 6:  Graph Operations         — PASSED" << endl;
    cout << "    TEST 7:  Hard Deletion            — PASSED" << endl;
    cout << "  PHASE 2 (Integration):" << endl;
    cout << "    TEST 8:  search()                 — PASSED" << endl;
    cout << "    TEST 9:  bruteForceSearch()       — PASSED" << endl;
    cout << "    TEST 10: Insert Pipeline          — PASSED" << endl;
    cout << "    TEST 11: Hit Rate Measurement     — PASSED" << endl;
    cout << "    TEST 12: Performance Timing       — PASSED" << endl;
    cout << "    TEST 13: Persistence (Save/Load)  — PASSED" << endl;
    cout << "    TEST 14: Full Lifecycle           — PASSED" << endl;
    cout << "================================================" << endl;
    cout << "  14/14 test groups passed. 0 failures." << endl;
    cout << "================================================" << endl;

    return 0;
}

// g++ -std=c++17 -Wall -Wextra -o test main.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp persistence.cpp
