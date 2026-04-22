#ifndef SEARCH_H
#define SEARCH_H

// ============================================================
// search.h — Greedy Search Algorithm
// Owner: Hira (Search Algorithm & Complexity Specialist)
// ============================================================

#include "node.h"
#include "nsw_graph.h"
#include <vector>
#include <string>

struct SearchResult {
    std::string text;
    double distance;
};

// ============================================================
// search()
// ============================================================
std::vector<SearchResult> search(
    NSWGraph* graph,
    const std::vector<double>& queryVector,
    int k,
    int efSearch
);

// ============================================================
// bruteForceSearch()
// ============================================================
std::vector<SearchResult> bruteForceSearch(
    NSWGraph* graph,
    const std::vector<double>& queryVector,
    int k
);

#endif // SEARCH_H
