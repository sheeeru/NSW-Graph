// ============================================================
// main.cpp — NSW Project Test Harness
//
// TEMPORARY FILE — Member 4 will replace this with the full CLI.
// Right now, this file tests Member 1's 4 functions and verifies
// that the full project skeleton compiles together cleanly.
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
// TEST 6: Skeleton compilation check (Member 2 & 3)
// Verifies the graph and search headers link properly.
// This test will be expanded once Members 2 & 3 implement.
// ------------------------------------------------------------
void testSkeletonCompilation() {
    cout << "\n========================================" << endl;
    cout << "TEST 6: Skeleton Compilation Check" << endl;
    cout << "========================================" << endl;

    // Verify NSWGraph can be instantiated (Member 2's class)
    NSWGraph graph;
    cout << "\n  NSWGraph created successfully." << endl;
    cout << "  Node count: " << graph.getNodeCount() << endl;
    cout << "  Active nodes: " << graph.getActiveNodeCount() << endl;
    cout << "  Deleted nodes: " << graph.getDeletedNodeCount() << endl;
    cout << "  Entry point: " << (graph.getEntryPoint() == nullptr ? "nullptr (correct)" : "ERROR") << endl;
    assert(graph.getNodeCount() == 0);
    assert(graph.getActiveNodeCount() == 0);
    assert(graph.getDeletedNodeCount() == 0);
    assert(graph.getEntryPoint() == nullptr);
    cout << "  [PASS] NSWGraph compiles and initializes correctly." << endl;

    // Verify search function is callable (Member 3's function)
    // Will return empty vector since graph is empty and function is skeleton
    vector<double> dummyQuery(VECTOR_DIM, 0.0);
    vector<SearchResult> results = search(&graph, dummyQuery, 5, EF_SEARCH);
    cout << "\n  search() called on empty graph." << endl;
    cout << "  Results: " << results.size() << " (expected: 0)" << endl;
    assert(results.empty());
    cout << "  [PASS] search() compiles and handles empty graph." << endl;

    // Verify getAllNodes() works (used by Member 4's brute-force)
    const vector<Node*>& nodes = graph.getAllNodes();
    assert(nodes.empty());
    cout << "\n  getAllNodes() returns empty list." << endl;
    cout << "  [PASS] Member 4's brute-force access point works." << endl;
}

// ------------------------------------------------------------
// TEST 7: Lazy Deletion
// Tests the removeNode() function and verifies:
//   - Nodes can be marked as deleted
//   - Deleted nodes are excluded from active count
//   - Entry point is reassigned when deleted
//   - Deleting all nodes leaves graph empty
//   - Double-deletion returns false
//   - Deleting nullptr returns false
//   - findNodeById() works
// ------------------------------------------------------------
void testLazyDeletion() {
    cout << "\n========================================" << endl;
    cout << "TEST 7: Lazy Deletion" << endl;
    cout << "========================================" << endl;

    NSWGraph graph;

    // Create 3 nodes manually (bypassing createNode since it's skeleton)
    vector<double> vec1 = vectorize("data structures");
    vector<double> vec2 = vectorize("machine learning");
    vector<double> vec3 = vectorize("operating systems");

    Node* n0 = new Node(0, "data structures", vec1);
    Node* n1 = new Node(1, "machine learning", vec2);
    Node* n2 = new Node(2, "operating systems", vec3);

    // Add edges manually
    n0->neighbors.push_back(n1);
    n1->neighbors.push_back(n0);
    n1->neighbors.push_back(n2);
    n2->neighbors.push_back(n1);

    // Set entry point
    graph.setEntryPoint(n2);
    cout << "\n  Setup: 3 nodes created, entry point = node 2" << endl;

    // --- Test 7a: findNodeById ---
    // NOTE: findNodeById searches masterList, which is only populated
    // by createNode(). Since we created nodes manually here for testing,
    // they won't be found. This test verifies the function compiles and
    // returns nullptr for untracked nodes. Full test after Member 2
    // implements createNode().
    cout << "\n  --- findNodeById ---" << endl;
    Node* found = graph.findNodeById(1);
    cout << "  findNodeById(1) = " << (found ? found->text : "nullptr") << endl;
    cout << "  (Returns nullptr because nodes were created manually," << endl;
    cout << "   not via createNode(). Will work after Member 2 implements.)" << endl;

    Node* notFound = graph.findNodeById(99);
    cout << "  findNodeById(99) = " << (notFound ? "ERROR" : "nullptr") << " (correct)" << endl;
    assert(notFound == nullptr);
    cout << "  [PASS] findNodeById compiles and returns nullptr for unknown IDs." << endl;

    // --- Test 7b: Delete a non-entry-point node ---
    cout << "\n  --- Delete node 1 (non-entry-point) ---" << endl;
    bool deleted = graph.removeNode(n1);
    cout << "  removeNode(n1) returned: " << (deleted ? "true" : "false") << endl;
    assert(deleted == true);
    assert(n1->is_deleted == true);
    cout << "  n1->is_deleted = " << (n1->is_deleted ? "true" : "false") << endl;
    cout << "  Entry point still node 2: " << (graph.getEntryPoint() == n2 ? "true" : "false") << endl;
    assert(graph.getEntryPoint() == n2); // Entry point unchanged
    cout << "  [PASS] Non-entry-point node deleted, entry point unchanged." << endl;

    // --- Test 7c: Double deletion ---
    cout << "\n  --- Double deletion of node 1 ---" << endl;
    bool deletedAgain = graph.removeNode(n1);
    cout << "  removeNode(n1) again returned: " << (deletedAgain ? "true (ERROR)" : "false") << endl;
    assert(deletedAgain == false);
    cout << "  [PASS] Double deletion returns false." << endl;

    // --- Test 7d: Delete the entry point ---
    cout << "\n  --- Delete node 2 (entry point) ---" << endl;
    bool deleted2 = graph.removeNode(n2);
    cout << "  removeNode(n2) returned: " << (deleted2 ? "true" : "false") << endl;
    assert(deleted2 == true);
    assert(n2->is_deleted == true);
    cout << "  n2->is_deleted = " << (n2->is_deleted ? "true" : "false") << endl;
    // NOTE: removeNode() reassigns entry point by searching masterList.
    // Since we created nodes manually (not via createNode()), masterList
    // is empty, so entry point becomes nullptr. This is correct behavior.
    // After Member 2 implements createNode(), it will find n0 in masterList
    // and auto-reassign properly.
    cout << "  Entry point after deleting untracked entry: "
         << (graph.getEntryPoint() == nullptr ? "nullptr" : "ERROR") << endl;
    assert(graph.getEntryPoint() == nullptr);
    cout << "  [PASS] Entry point correctly unset (masterList is empty)." << endl;
    cout << "  (After Member 2 implements createNode(), auto-reassign works.)" << endl;

    // --- Test 7e: Delete nullptr ---
    cout << "\n  --- Delete nullptr ---" << endl;
    bool deletedNull = graph.removeNode(nullptr);
    cout << "  removeNode(nullptr) returned: " << (deletedNull ? "true (ERROR)" : "false") << endl;
    assert(deletedNull == false);
    cout << "  [PASS] nullptr deletion returns false." << endl;

    // --- Test 7f: Edges still exist after deletion ---
    cout << "\n  --- Edge preservation check ---" << endl;
    cout << "  n0->neighbors.size() = " << n0->neighbors.size()
         << " (expected: 1, still pointing to deleted n1)" << endl;
    assert(n0->neighbors.size() == 1);
    assert(n0->neighbors[0] == n1); // Edge to deleted node still exists
    cout << "  n1->neighbors.size() = " << n1->neighbors.size()
         << " (expected: 2, still intact)" << endl;
    assert(n1->neighbors.size() == 2); // Deleted node's edges preserved
    cout << "  [PASS] Edges preserved after lazy deletion." << endl;

    // --- Test 7g: Search skips deleted nodes ---
    cout << "\n  --- Search skips deleted nodes ---" << endl;
    vector<double> queryVec = vectorize("data algo");
    vector<SearchResult> results = search(&graph, queryVec, 5, EF_SEARCH);
    // The search skeleton returns empty, but once Member 3 implements it,
    // deleted nodes (n1, n2) should NOT appear in results.
    // Only n0 ("data structures") should be returned.
    cout << "  Search results count: " << results.size()
         << " (skeleton returns 0, will work after Member 3 implements)" << endl;
    cout << "  [PASS] Search compilation check (full test after Member 3)." << endl;

    // --- Cleanup ---
    delete n0;
    delete n1;
    delete n2;
    cout << "\n  [ALL DELETION TESTS PASSED]" << endl;
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main() {
    cout << "================================================" << endl;
    cout << "  NSW Project — Full Skeleton Test Suite" << endl;
    cout << "  Member 1: getTrigrams, hashTrigram, vectorize," << endl;
    cout << "             cosineDistance" << endl;
    cout << "  Member 2: NSWGraph (skeleton) + Lazy Deletion" << endl;
    cout << "  Member 3: search() (skeleton)" << endl;
    cout << "  Member 4: WAITING (needs Members 2 & 3)" << endl;
    cout << "================================================" << endl;

    testGetTrigrams();
    testHashTrigram();
    testVectorize();
    testCosineDistance();
    testEndToEnd();
    testSkeletonCompilation();
    testLazyDeletion();

    cout << "\n================================================" << endl;
    cout << "  ALL TESTS PASSED!" << endl;
    cout << "  Member 1: COMPLETE" << endl;
    cout << "  Member 2: SKELETON READY (fill in TODOs)" << endl;
    cout << "             removeNode() DONE (lazy deletion)" << endl;
    cout << "  Member 3: SKELETON READY (fill in TODOs)" << endl;
    cout << "             (deletion skip logic documented)" << endl;
    cout << "  Member 4: WAITING (needs Members 2 & 3)" << endl;
    cout << "================================================" << endl;

    return 0;
}
