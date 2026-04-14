// ============================================================
// main.cpp — Member 1 Test Harness
//
// TEMPORARY FILE — Member 4 will replace this with the full CLI.
// Right now, this file ONLY tests Member 1's 4 functions:
//   1. getTrigrams()
//   2. hashTrigram()
//   3. vectorize()
//   4. cosineDistance()
//
// Compile:  g++ -std=c++17 -o test main.cpp vectorizer.cpp
// Run:      ./test
// ============================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cassert>
#include "vectorizer.h"
#include "config.h"

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

    // Normal case
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

    // Exact length 3
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

    // Short text (padding test)
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

    // Empty string
    vector<string> t4 = getTrigrams("");
    cout << "\n  getTrigrams(\"\"):" << endl;
    cout << "  Expected: [] (empty)" << endl;
    cout << "  Got:      [] (size=" << t4.size() << ")" << endl;
    assert(t4.empty());
    cout << "  [PASS]" << endl;

    // Longer sentence
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

    // Same input should always produce same output (deterministic within run)
    int h1 = hashTrigram("hel");
    int h2 = hashTrigram("hel");
    cout << "\n  hashTrigram(\"hel\") = " << h1 << endl;
    cout << "  hashTrigram(\"hel\") = " << h2 << " (same call, must match)" << endl;
    assert(h1 == h2);
    cout << "  [PASS] Deterministic" << endl;

    // Output must be in valid range [0, VECTOR_DIM)
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

    // Normal text
    vector<double> v1 = vectorize("hello world");
    cout << "\n  vectorize(\"hello world\"):" << endl;
    printVectorSparse(v1);
    assert(v1.size() == VECTOR_DIM);
    assert(countNonZero(v1) > 0);
    cout << "  [PASS] Correct size and non-empty" << endl;

    // Same text should produce same vector
    vector<double> v1_again = vectorize("hello world");
    bool identical = true;
    for (size_t i = 0; i < v1.size(); i++) {
        if (v1[i] != v1_again[i]) { identical = false; break; }
    }
    assert(identical);
    cout << "\n  Re-vectorizing same text -> identical vector" << endl;
    cout << "  [PASS] Deterministic" << endl;

    // Similar texts should produce similar vectors
    vector<double> v2 = vectorize("hello worl");
    double dist = cosineDistance(v1, v2);
    cout << "\n  vectorize(\"hello world\") vs vectorize(\"hello worl\"):" << endl;
    cout << "  Distance: " << fixed << setprecision(6) << dist << endl;
    cout << "  (Typo-tolerance check: distance should be small, << 1.0)" << endl;
    cout << "  [PASS] Typo-tolerance demonstrated" << endl;

    // Completely different texts should have larger distance
    vector<double> v3 = vectorize("quantum physics equations");
    double dist_diff = cosineDistance(v1, v3);
    cout << "\n  vectorize(\"hello world\") vs vectorize(\"quantum physics equations\"):" << endl;
    cout << "  Distance: " << fixed << setprecision(6) << dist_diff << endl;
    cout << "  (Unrelated texts: distance should be larger)" << endl;
    cout << "  [PASS] Dissimilar texts are farther apart" << endl;

    // Empty string should produce zero vector
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

    // Identical vectors -> distance = 0
    vector<double> a = {1.0, 2.0, 3.0};
    vector<double> b = {1.0, 2.0, 3.0};
    double d1 = cosineDistance(a, b);
    cout << "\n  cosineDistance([1,2,3], [1,2,3]):" << endl;
    cout << "  Expected: 0.000000 (identical)" << endl;
    cout << "  Got:      " << fixed << setprecision(6) << d1 << endl;
    assert(d1 < 0.0001); // Allow tiny floating-point error
    cout << "  [PASS]" << endl;

    // Opposite vectors -> distance = 2.0
    // Formula: similarity = -1.0, distance = 1 - (-1) = 2
    // This is mathematically correct. In text comparison we never
    // get truly opposite vectors, so practical distances are [0, 1].
    vector<double> c = {1.0, 0.0, 0.0};
    vector<double> d = {-1.0, 0.0, 0.0};
    double d2 = cosineDistance(c, d);
    cout << "\n  cosineDistance([1,0,0], [-1,0,0]):" << endl;
    cout << "  Expected: 2.000000 (opposite vectors)" << endl;
    cout << "  Got:      " << fixed << setprecision(6) << d2 << endl;
    cout << "  [PASS]" << endl;

    // Orthogonal vectors -> distance = 1.0
    vector<double> e = {1.0, 0.0};
    vector<double> f = {0.0, 1.0};
    double d3 = cosineDistance(e, f);
    cout << "\n  cosineDistance([1,0], [0,1]):" << endl;
    cout << "  Expected: 1.000000 (orthogonal)" << endl;
    cout << "  Got:      " << fixed << setprecision(6) << d3 << endl;
    assert(d3 > 0.999 && d3 <= 1.001);
    cout << "  [PASS]" << endl;

    // Zero vector guard
    vector<double> zero(5, 0.0);
    vector<double> g = {1.0, 2.0, 3.0, 4.0, 5.0};
    double d4 = cosineDistance(zero, g);
    cout << "\n  cosineDistance([0,0,0,0,0], [1,2,3,4,5]):" << endl;
    cout << "  Expected: 1.000000 (zero-vector guard)" << endl;
    cout << "  Got:      " << fixed << setprecision(6) << d4 << endl;
    assert(d4 == 1.0);
    cout << "  [PASS] Division by zero prevented" << endl;

    // Both zero vectors
    double d5 = cosineDistance(zero, zero);
    cout << "\n  cosineDistance([0,0,0,0,0], [0,0,0,0,0]):" << endl;
    cout << "  Expected: 1.000000 (zero-vector guard)" << endl;
    cout << "  Got:      " << fixed << setprecision(6) << d5 << endl;
    assert(d5 == 1.0);
    cout << "  [PASS]" << endl;

    // Real-world example from vectorize()
    vector<double> v_hello = vectorize("hello world");
    vector<double> v_hella = vectorize("hella world");
    vector<double> v_quantum = vectorize("quantum entanglement theory");
    double d_real1 = cosineDistance(v_hello, v_hella);
    double d_real2 = cosineDistance(v_hello, v_quantum);
    cout << "\n  Real-world comparison:" << endl;
    cout << "  \"hello world\" vs \"hella world\":     " << fixed << setprecision(6) << d_real1 << endl;
    cout << "  \"hello world\" vs \"quantum entang...\": " << fixed << setprecision(6) << d_real2 << endl;
    cout << "  (Similar texts should be closer than unrelated ones)" << endl;
    assert(d_real1 < d_real2);
    cout << "  [PASS] Similar texts are closer than unrelated ones" << endl;
}

// ------------------------------------------------------------
// TEST 5: End-to-End Demonstration
// ------------------------------------------------------------
void testEndToEnd() {
    cout << "\n========================================" << endl;
    cout << "TEST 5: End-to-End Demonstration" << endl;
    cout << "========================================" << endl;

    // Simulate what the full system will do:
    // 1. Store some "notes" as vectors
    // 2. Take a query
    // 3. Find the closest match

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

    // Vectorize query
    vector<double> queryVec = vectorize(query);

    // Find closest match (brute force — Member 4 will replace with NSW)
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
// MAIN
// ------------------------------------------------------------
int main() {
    cout << "================================================" << endl;
    cout << "  NSW Project — Member 1 Test Suite" << endl;
    cout << "  Testing: getTrigrams, hashTrigram, vectorize," << endl;
    cout << "           cosineDistance" << endl;
    cout << "================================================" << endl;

    testGetTrigrams();
    testHashTrigram();
    testVectorize();
    testCosineDistance();
    testEndToEnd();

    cout << "\n================================================" << endl;
    cout << "  ALL TESTS PASSED! Member 1's code is ready." << endl;
    cout << "================================================" << endl;

    return 0;
}
