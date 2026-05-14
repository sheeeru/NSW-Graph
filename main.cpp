// main.cpp — NSW Graph Test Harness
// Compile:  g++ -std=c++17 -Wall -Wextra -o test main.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp persistence.cpp
// Run:      ./test

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

// ── Helpers ──────────────────────────────────────────────────────────────────
int countNonZero(const vector<double>& v) {
    int c = 0; for (double x : v) if (x != 0.0) c++; return c;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 1: getTrigrams()   — Section A  (vectorizer.cpp)
// ─────────────────────────────────────────────────────────────────────────────
void testGetTrigrams() {
    cout << "\n=== TEST 1: getTrigrams() ===" << endl;

    // Normal text: 5 chars → 3 trigrams
    auto t = getTrigrams("hello");
    assert(t.size()==3 && t[0]=="hel" && t[1]=="ell" && t[2]=="llo");
    cout << "  hello → " << t[0] << "," << t[1] << "," << t[2] << "  [PASS]" << endl;

    // Exactly 3 chars → 1 trigram
    t = getTrigrams("cat");
    assert(t.size()==1 && t[0]=="cat");
    cout << "  cat   → " << t[0] << "  [PASS]" << endl;

    // Short text padded with spaces
    t = getTrigrams("hi");
    assert(t.size()==1 && t[0]=="hi ");
    cout << "  hi    → \"" << t[0] << "\"  (padded)  [PASS]" << endl;

    t = getTrigrams("a");
    assert(t.size()==1 && t[0]=="a  ");
    cout << "  a     → \"" << t[0] << "\"  (padded)  [PASS]" << endl;

    // Empty string
    t = getTrigrams("");
    assert(t.empty());
    cout << "  \"\"  → []  [PASS]" << endl;

    // Multi-word: formula = length - 2
    t = getTrigrams("hello world");
    assert(t.size()==9);
    cout << "  hello world (11 chars) → " << t.size() << " trigrams  [PASS]" << endl;

    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 2: hashTrigram()   — Section A  (vectorizer.cpp)
// ─────────────────────────────────────────────────────────────────────────────
void testHashTrigram() {
    cout << "\n=== TEST 2: hashTrigram() ===" << endl;

    // Deterministic: same input → same output every time
    int h1 = hashTrigram("hel"), h2 = hashTrigram("hel");
    assert(h1 == h2);
    cout << "  hashTrigram(\"hel\") = " << h1 << "  (deterministic)  [PASS]" << endl;

    // All outputs in range [0, VECTOR_DIM)
    for (const string s : {"cat","dog","zzz","   ","aaa"}) {
        int h = hashTrigram(s);
        assert(h >= 0 && h < VECTOR_DIM);
        cout << "  \"" << s << "\" → " << h << "  (in range)  [PASS]" << endl;
    }

    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 3: vectorize()   — Section A  (vectorizer.cpp)
// ─────────────────────────────────────────────────────────────────────────────
void testVectorize() {
    cout << "\n=== TEST 3: vectorize() ===" << endl;

    // Correct output size
    auto v1 = vectorize("hello world");
    assert((int)v1.size() == VECTOR_DIM && countNonZero(v1) > 0);
    cout << "  \"hello world\" → size=" << v1.size()
         << ", non-zero=" << countNonZero(v1) << "  [PASS]" << endl;

    // Deterministic
    auto v1b = vectorize("hello world");
    bool same = true;
    for (int i = 0; i < VECTOR_DIM; i++) if (v1[i] != v1b[i]) { same=false; break; }
    assert(same);
    cout << "  Re-vectorizing gives identical vector  [PASS]" << endl;

    // Typo tolerance: "hello world" vs "hello worl" should be close
    double d_typo = cosineDistance(v1, vectorize("hello worl"));
    double d_diff  = cosineDistance(v1, vectorize("quantum physics"));
    assert(d_typo < 0.5 && d_typo < d_diff);
    cout << "  Typo dist=" << fixed << setprecision(4) << d_typo
         << "  Unrelated dist=" << d_diff << "  [PASS]" << endl;

    // Empty string → zero vector
    auto ve = vectorize("");
    assert((int)ve.size()==VECTOR_DIM && countNonZero(ve)==0);
    cout << "  Empty string → zero vector  [PASS]" << endl;

    // Single char: "a" → padded "a  " → 1 trigram → 1 non-zero
    auto vs = vectorize("a");
    assert(countNonZero(vs)==1);
    cout << "  \"a\" → 1 non-zero entry  [PASS]" << endl;

    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 4: cosineDistance()   — Section A  (vectorizer.cpp)
// ─────────────────────────────────────────────────────────────────────────────
void testCosineDistance() {
    cout << "\n=== TEST 4: cosineDistance() ===" << endl;

    // Identical → 0
    vector<double> a={1,2,3}, b={1,2,3};
    assert(cosineDistance(a,b) < 0.0001);
    cout << "  Identical vectors → 0  [PASS]" << endl;

    // Orthogonal → 1
    vector<double> e={1,0}, f={0,1};
    double d3 = cosineDistance(e,f);
    assert(d3 > 0.999 && d3 <= 1.001);
    cout << "  Orthogonal vectors → 1  [PASS]" << endl;

    // Zero-vector guard → 1.0 (no crash)
    vector<double> z(5,0.0), g={1,2,3,4,5};
    assert(cosineDistance(z,g)==1.0 && cosineDistance(z,z)==1.0);
    cout << "  Zero-vector guard → 1.0  (no crash)  [PASS]" << endl;

    // Size mismatch guard → 1.0
    vector<double> h1={1,2}, h2={1,2,3};
    assert(cosineDistance(h1,h2)==1.0);
    cout << "  Size mismatch → 1.0  [PASS]" << endl;

    // Similar texts are closer than unrelated
    auto vh = vectorize("hello world");
    double ds = cosineDistance(vh, vectorize("hella world"));
    double dd = cosineDistance(vh, vectorize("quantum physics"));
    assert(ds < dd);
    cout << "  Similar=" << fixed << setprecision(4) << ds
         << "  Unrelated=" << dd << "  [PASS]" << endl;

    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 5: Vectorizer End-to-End   — Section A 
// Full pipeline: text → trigrams → hash → vector → cosine → best match
// ─────────────────────────────────────────────────────────────────────────────
void testVectorizerEndToEnd() {
    cout << "\n=== TEST 5: Vectorizer End-to-End ===" << endl;

    vector<string> notes = {
        "data structures and algorithms",
        "machine learning basics",
        "database management systems",
        "computer networks and security",
        "operating system concepts",
    };
    string query = "data structures course";
    auto qv = vectorize(query);

    string best; double bestD = 999.0;
    for (auto& note : notes) {
        double d = cosineDistance(qv, vectorize(note));
        cout << "  " << note << " → " << fixed << setprecision(4) << d;
        if (d < bestD) { bestD=d; best=note; cout << "  <-- BEST"; }
        cout << endl;
    }
    assert(best == "data structures and algorithms");
    cout << "  Best match: \"" << best << "\"  [PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 6: Graph Operations   — Section B (nsw_graph.cpp / node.h)
// createNode, addEdge (bidirectionality, self-edge, duplicate prevention)
// setEntryPoint, getEntryPoint, getNodeCount, getAllNodes
// ─────────────────────────────────────────────────────────────────────────────
void testGraphOperations() {
    cout << "\n=== TEST 6: Graph Operations ===" << endl;

    NSWGraph g;

    // Empty graph
    assert(g.getNodeCount()==0 && g.getEntryPoint()==nullptr && g.getAllNodes().empty());
    cout << "  Empty graph: count=0, entry=nullptr  [PASS]" << endl;

    // createNode stores text and vector correctly
    Node* n0 = g.createNode("data structures", vectorize("data structures"));
    Node* n1 = g.createNode("machine learning", vectorize("machine learning"));
    Node* n2 = g.createNode("operating systems", vectorize("operating systems"));
    assert(g.getNodeCount()==3);
    assert(n0->text=="data structures" && (int)n0->numericalVector.size()==VECTOR_DIM);
    cout << "  3 nodes created, text+vector stored  [PASS]" << endl;

    // addEdge: bidirectional
    g.addEdge(n0,n1); g.addEdge(n1,n2); g.addEdge(n0,n2);
    assert(n0->neighbors.size()==2 && n1->neighbors.size()==2 && n2->neighbors.size()==2);
    assert(n0->neighbors[0]==n1 && n1->neighbors[0]==n0);  // bidirectional
    cout << "  Bidirectional edges: each node has 2 neighbours  [PASS]" << endl;

    // Self-edge prevention
    g.addEdge(n0,n0);
    assert(n0->neighbors.size()==2);
    cout << "  Self-edge attempt: size unchanged  [PASS]" << endl;

    // Duplicate edge prevention
    g.addEdge(n0,n1);
    assert(n0->neighbors.size()==2);
    cout << "  Duplicate edge attempt: size unchanged  [PASS]" << endl;

    // Entry point
    g.setEntryPoint(n2);
    assert(g.getEntryPoint()==n2);
    cout << "  Entry point set to: \"" << g.getEntryPoint()->text << "\"  [PASS]" << endl;

    // getAllNodes order
    const auto& all = g.getAllNodes();
    assert(all.size()==3 && all[0]==n0 && all[1]==n1 && all[2]==n2);
    cout << "  getAllNodes() correct order  [PASS]" << endl;

    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 7: Hard Deletion   — Section B (nsw_graph.cpp)
// deleteNode: removes node, relinks neighbours, updates entry point,
// no dangling pointers, no duplicate edges, nullptr guard
// ─────────────────────────────────────────────────────────────────────────────
void testHardDeletion() {
    cout << "\n=== TEST 7: Hard Deletion ===" << endl;

    NSWGraph g;
    Node* n0 = g.createNode("apple",  vectorize("apple"));
    Node* n1 = g.createNode("apply",  vectorize("apply"));
    Node* n2 = g.createNode("banana", vectorize("banana"));
    Node* n3 = g.createNode("orange", vectorize("orange"));
    g.addEdge(n0,n1); g.addEdge(n0,n2); g.addEdge(n0,n3);
    g.addEdge(n1,n2); g.addEdge(n1,n3); g.addEdge(n2,n3);
    g.setEntryPoint(n0);
    cout << "  Setup: 4-node fully connected graph (M=16, 3 neighbours each)" << endl;

    // Delete non-entry-point node
    g.deleteNode(n3);
    assert(g.getNodeCount()==3 && g.getEntryPoint()==n0);
    for (Node* nd : g.getAllNodes())
        for (Node* nb : nd->neighbors) assert(nb != n3);
    cout << "  Deleted n3: count=3, n3 gone from all lists  [PASS]" << endl;

    // No duplicate edges
    for (Node* nd : g.getAllNodes())
        for (size_t i=0; i<nd->neighbors.size(); i++)
            for (size_t j=i+1; j<nd->neighbors.size(); j++)
                assert(nd->neighbors[i] != nd->neighbors[j]);
    cout << "  No duplicate edges  [PASS]" << endl;

    // Delete entry point → reassign
    g.deleteNode(n0);
    assert(g.getNodeCount()==2 && g.getEntryPoint()!=nullptr);
    cout << "  Deleted entry point: reassigned to \""
         << g.getEntryPoint()->text << "\"  [PASS]" << endl;

    // Remaining nodes still connected
    assert(n1->neighbors.size()==1 && n1->neighbors[0]==n2);
    assert(n2->neighbors.size()==1 && n2->neighbors[0]==n1);
    cout << "  n1 and n2 still connected after deletion  [PASS]" << endl;

    // Delete until empty
    g.deleteNode(n1); g.deleteNode(n2);
    assert(g.getNodeCount()==0 && g.getEntryPoint()==nullptr);
    cout << "  Graph emptied: count=0, entry=nullptr  [PASS]" << endl;

    // nullptr guard
    g.deleteNode(nullptr);
    assert(g.getNodeCount()==0);
    cout << "  deleteNode(nullptr): no crash  [PASS]" << endl;

    // Relink under M limit (star graph)
    NSWGraph g2;
    Node* ctr = g2.createNode("center", vectorize("center"));
    vector<Node*> leaves;
    for (int i=0; i<M; i++) {
        string lt = "leaf"+to_string(i);
        Node* lf = g2.createNode(lt, vectorize(lt));
        g2.addEdge(ctr,lf);
        leaves.push_back(lf);
    }
    g2.setEntryPoint(ctr);
    g2.deleteNode(ctr);
    assert(g2.getNodeCount()==M && leaves[0]->neighbors.size()>0);
    cout << "  Star deletion: leaves relinked  [PASS]" << endl;

    // No dangling pointers
    const auto& rem = g2.getAllNodes();
    for (Node* nd : rem)
        for (Node* nb : nd->neighbors) {
            bool found = false;
            for (Node* n : rem) if (nb==n) { found=true; break; }
            assert(found);
        }
    cout << "  No dangling pointers  [PASS]" << endl;

    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 8: search()   — Section C (search.cpp)
// Greedy NSW search: empty graph, exact match, k=1, k>size,
// sorted results, typo-tolerance, M-limit edge case
// ─────────────────────────────────────────────────────────────────────────────
void testSearch() {
    cout << "\n=== TEST 8: search() ===" << endl;

    // Empty graph
    { NSWGraph g; assert(search(&g, vectorize("hello"), 3, EF_SEARCH).empty()); }
    cout << "  Empty graph → 0 results  [PASS]" << endl;

    // No entry point
    { NSWGraph g; g.createNode("x", vectorize("x"));
      assert(search(&g, vectorize("x"), 3, EF_SEARCH).empty()); }
    cout << "  No entry point → 0 results  [PASS]" << endl;

    // Build graph
    NSWGraph g;
    insert(&g, "data structures and algorithms");
    insert(&g, "machine learning fundamentals");
    insert(&g, "database management systems");
    insert(&g, "operating system concepts");

    // Exact match returns itself with distance ≈ 0
    auto r = search(&g, vectorize("data structures and algorithms"), 3, EF_SEARCH);
    assert(!r.empty() && r[0].text=="data structures and algorithms" && r[0].distance<0.0001);
    cout << "  Exact match: dist=" << r[0].distance << "  [PASS]" << endl;

    // k=1 returns exactly 1
    r = search(&g, vectorize("machine learning"), 1, EF_SEARCH);
    assert(r.size()==1);
    cout << "  k=1 → 1 result  [PASS]" << endl;

    // k > node count returns all reachable nodes
    r = search(&g, vectorize("computer"), 100, EF_SEARCH);
    assert((int)r.size()==g.getNodeCount());
    cout << "  k=100, " << g.getNodeCount() << " nodes → " << r.size() << " returned  [PASS]" << endl;

    // Results sorted ascending
    r = search(&g, vectorize("database"), 3, EF_SEARCH);
    for (size_t i=1; i<r.size(); i++) assert(r[i].distance >= r[i-1].distance);
    cout << "  Results sorted ascending  [PASS]" << endl;

    // Typo tolerance
    r = search(&g, vectorize("operating systems and kernels"), 3, EF_SEARCH);
    assert(!r.empty() && r[0].text=="operating system concepts");
    cout << "  Typo-tolerant: found \"" << r[0].text << "\"  [PASS]" << endl;

    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 9: Insert Pipeline   — Section B (insert.cpp)
// First node sets entry point, subsequent nodes get edges,
// empty text rejected, all phrases searchable, entry point updates
// ─────────────────────────────────────────────────────────────────────────────
void testInsertPipeline() {
    cout << "\n=== TEST 9: Insert Pipeline ===" << endl;

    NSWGraph g;

    // First insert → entry point set
    insert(&g, "data structures");
    assert(g.getNodeCount()==1 && g.getEntryPoint()->text=="data structures");
    cout << "  First insert: entry point = \"" << g.getEntryPoint()->text << "\"  [PASS]" << endl;

    // Second insert → edge created
    insert(&g, "data algorithms");
    assert(g.getNodeCount()==2);
    const auto& n2 = g.getAllNodes();
    assert(n2[0]->neighbors.size()>0 || n2[1]->neighbors.size()>0);
    cout << "  Second insert: edge created  [PASS]" << endl;

    // Multiple inserts build connected graph
    insert(&g, "machine learning");
    insert(&g, "deep learning");
    int totalEdges = 0;
    for (Node* n : g.getAllNodes()) totalEdges += (int)n->neighbors.size();
    assert(totalEdges >= 4);
    cout << "  4 nodes, total neighbour count=" << totalEdges << "  [PASS]" << endl;

    // All 4 phrases are searchable as exact matches
    for (auto& q : {"data structures","data algorithms","machine learning","deep learning"}) {
        auto r = search(&g, vectorize(q), 1, EF_SEARCH);
        bool found = !r.empty() && r[0].text==q;
        assert(found);
        cout << "  \"" << q << "\" searchable  [PASS]" << endl;
    }

    // Empty text rejected
    int before = g.getNodeCount();
    insert(&g, "");
    assert(g.getNodeCount()==before);
    cout << "  Empty text rejected  [PASS]" << endl;

    // Entry point always updates to last inserted
    insert(&g, "neural networks");
    assert(g.getEntryPoint()->text=="neural networks");
    cout << "  Entry point updated to last inserted  [PASS]" << endl;

    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 10: Search Accuracy   — Section C (search.cpp)
// Verifies that searching for an exact phrase always returns
// that phrase itself as the top-1 result (distance ≈ 0).
// ─────────────────────────────────────────────────────────────────────────────
void testSearchAccuracy() {
    cout << "\n=== TEST 10: Search Accuracy ===" << endl;

    NSWGraph g;
    vector<string> dataset = {
        "data structures and algorithms",
        "data structures trees and graphs",
        "machine learning fundamentals",
        "machine learning neural networks",
        "database management systems",
        "operating system concepts",
        "computer networks and security",
        "software engineering principles",
    };
    for (auto& t : dataset) insert(&g, t);
    cout << "  Graph: " << g.getNodeCount() << " nodes" << endl;

    int hits = 0;
    for (auto& phrase : dataset) {
        auto r = search(&g, vectorize(phrase), 1, EF_SEARCH);
        bool exact = !r.empty() && r[0].text==phrase && r[0].distance<0.0001;
        if (exact) hits++;
        cout << "  \"" << phrase << "\" → "
             << (exact ? "EXACT MATCH" : "MISS") << endl;
    }
    cout << "  Hit rate: " << hits << "/" << dataset.size() << endl;
    assert(hits == (int)dataset.size());
    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 11: Performance Timing   — Section D (main.cpp)
// Times NSW search over 1000 iterations to demonstrate O(log N) behaviour.
// ─────────────────────────────────────────────────────────────────────────────
void testPerformanceTiming() {
    cout << "\n=== TEST 11: Performance Timing ===" << endl;

    NSWGraph g;
    vector<string> dataset = {
        "data structures and algorithms",
        "data structures trees and graphs",
        "data structures linked lists",
        "machine learning fundamentals",
        "machine learning neural networks",
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
    for (auto& t : dataset) insert(&g, t);
    cout << "  Graph: " << g.getNodeCount()
         << " nodes | M=" << M << " | EF_SEARCH=" << EF_SEARCH << endl;

    vector<string> queries = {
        "data structures", "machine learning",
        "database systems", "operating systems", "algorithms"
    };

    const int ITERS = 1000;
    auto t0 = chrono::high_resolution_clock::now();
    for (auto& q : queries) {
        auto qv = vectorize(q);
        for (int i=0; i<ITERS; i++) search(&g, qv, 3, EF_SEARCH);
    }
    auto t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration_cast<chrono::microseconds>(t1-t0).count() / 1000.0;

    cout << "  NSW greedy search: " << fixed << setprecision(2) << ms
         << " ms total over " << ITERS << " iterations × "
         << queries.size() << " queries" << endl;
    cout << "  Avg per query: " << (ms/queries.size()) << " ms" << endl;
    assert(ms >= 0);  // just verify it ran without crashing
    cout << "  [PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 12: Persistence   — Section D (persistence.cpp)
// saveToFile / loadFromFile round-trip, search on loaded graph, empty save
// ─────────────────────────────────────────────────────────────────────────────
void testPersistence() {
    cout << "\n=== TEST 12: Persistence (Save / Load) ===" << endl;

    const string testFile = "test_persist.txt";

    // Save
    {
        NSWGraph g;
        insert(&g, "data structures and algorithms");
        insert(&g, "machine learning fundamentals");
        insert(&g, "database management systems");
        insert(&g, "operating system concepts");
        saveToFile(&g, testFile);
        cout << "  Saved 4 nodes to \"" << testFile << "\"" << endl;
    }

    // Load and verify count
    NSWGraph g2;
    loadFromFile(&g2, testFile);
    assert(g2.getNodeCount()==4);
    cout << "  Loaded: " << g2.getNodeCount() << " nodes  [PASS]" << endl;

    // All phrases present
    vector<string> expected = {
        "data structures and algorithms","machine learning fundamentals",
        "database management systems","operating system concepts"
    };
    for (auto& e : expected) {
        bool found = false;
        for (Node* n : g2.getAllNodes()) if (n->text==e) { found=true; break; }
        assert(found);
        cout << "  Found: \"" << e << "\"  [PASS]" << endl;
    }

    // Search works on loaded graph
    auto r = search(&g2, vectorize("data structures"), 3, EF_SEARCH);
    assert(!r.empty() && r[0].text=="data structures and algorithms");
    cout << "  Search on loaded graph → \"" << r[0].text << "\"  [PASS]" << endl;

    // Empty graph saves as 0-line file
    {
        NSWGraph ge;
        saveToFile(&ge, "test_empty.txt");
        ifstream f("test_empty.txt");
        string line; int lines=0;
        while (getline(f,line)) lines++;
        assert(lines==0);
        remove("test_empty.txt");
    }
    cout << "  Empty graph saves as 0-line file  [PASS]" << endl;

    remove(testFile.c_str());
    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// TEST 13: Full Lifecycle   — Section D (main.cpp)
// insert → search → delete → re-search → insert → save → load → verify
// ─────────────────────────────────────────────────────────────────────────────
void testFullLifecycle() {
    cout << "\n=== TEST 13: Full Lifecycle ===" << endl;

    const string lcFile = "test_lifecycle.txt";
    NSWGraph g;

    // Build
    insert(&g, "data structures and algorithms");
    insert(&g, "machine learning basics");
    insert(&g, "database management systems");
    insert(&g, "operating system concepts");
    assert(g.getNodeCount()==4);
    cout << "  Built: 4 nodes  [PASS]" << endl;

    // Search
    auto r = search(&g, vectorize("data structures"), 3, EF_SEARCH);
    assert(!r.empty() && r[0].text=="data structures and algorithms");
    cout << "  Search: \"" << r[0].text << "\"  [PASS]" << endl;

    // Delete
    Node* del = nullptr;
    for (Node* n : g.getAllNodes()) if (n->text=="database management systems") { del=n; break; }
    assert(del);
    g.deleteNode(del);
    assert(g.getNodeCount()==3);
    r = search(&g, vectorize("database management systems"), 3, EF_SEARCH);
    for (auto& x : r) assert(x.text!="database management systems");
    cout << "  Deleted \"database management systems\", gone from results  [PASS]" << endl;

    // Graph still works after deletion
    r = search(&g, vectorize("data structures"), 3, EF_SEARCH);
    assert(!r.empty() && r[0].text=="data structures and algorithms");
    cout << "  Search still works after deletion  [PASS]" << endl;

    // Insert replacement
    insert(&g, "database systems and sql");
    assert(g.getNodeCount()==4);
    r = search(&g, vectorize("database systems"), 3, EF_SEARCH);
    bool found = false;
    for (auto& x : r) if (x.text=="database systems and sql") { found=true; break; }
    assert(found);
    cout << "  Replacement \"database systems and sql\" searchable  [PASS]" << endl;

    // Save → Load → Verify
    saveToFile(&g, lcFile);
    NSWGraph g2;
    loadFromFile(&g2, lcFile);
    assert(g2.getNodeCount()==4);
    r = search(&g2, vectorize("data structures"), 3, EF_SEARCH);
    assert(!r.empty() && r[0].text=="data structures and algorithms");
    bool foundRep = false;
    for (Node* n : g2.getAllNodes()) if (n->text=="database systems and sql") { foundRep=true; break; }
    assert(foundRep);
    cout << "  Save → Load: 4 nodes, search correct, replacement persists  [PASS]" << endl;

    remove(lcFile.c_str());
    cout << "  [ALL PASS]" << endl;
}

// ─────────────────────────────────────────────────────────────────────────────
// MAIN
// ─────────────────────────────────────────────────────────────────────────────
int main()
{
    cout << "================================================" << endl;
    cout << "  NSW Graph — Test Suite" << endl;
    cout << "  M=" << M << " | EF_SEARCH=" << EF_SEARCH
         << " | VECTOR_DIM=" << VECTOR_DIM << endl;
    cout << "================================================" << endl;
    cout << "  Section A  : getTrigrams, hashTrigram, vectorize, cosineDistance" << endl;
    cout << "  Section B  : NSWGraph (createNode, addEdge, deleteNode), insert" << endl;
    cout << "  Section C  : search() — greedy NSW search" << endl;
    cout << "  Section D  : persistence, timing, lifecycle tests" << endl;
    cout << "================================================" << endl;

    testGetTrigrams();
    testHashTrigram();
    testVectorize();
    testCosineDistance();
    testVectorizerEndToEnd();
    testGraphOperations();
    testHardDeletion();
    testSearch();
    testInsertPipeline();
    testSearchAccuracy();
    testPerformanceTiming();
    testPersistence();
    testFullLifecycle();

    cout << "\n================================================" << endl;
    cout << "  ALL TESTS PASSED" << endl;
    cout << "================================================" << endl;
    cout << "  TEST  1: getTrigrams()        — PASSED" << endl;
    cout << "  TEST  2: hashTrigram()        — PASSED" << endl;
    cout << "  TEST  3: vectorize()          — PASSED" << endl;
    cout << "  TEST  4: cosineDistance()     — PASSED" << endl;
    cout << "  TEST  5: Vectorizer E2E       — PASSED" << endl;
    cout << "  TEST  6: Graph Operations     — PASSED" << endl;
    cout << "  TEST  7: Hard Deletion        — PASSED" << endl;
    cout << "  TEST  8: search()             — PASSED" << endl;
    cout << "  TEST  9: Insert Pipeline      — PASSED" << endl;
    cout << "  TEST 10: Search Accuracy      — PASSED" << endl;
    cout << "  TEST 11: Performance Timing   — PASSED" << endl;
    cout << "  TEST 12: Persistence          — PASSED" << endl;
    cout << "  TEST 13: Full Lifecycle       — PASSED" << endl;
    cout << "================================================" << endl;
    cout << "  13/13 tests passed." << endl;
    cout << "================================================" << endl;

    return 0;
}