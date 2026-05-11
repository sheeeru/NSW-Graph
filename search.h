#ifndef SEARCH_H
#define SEARCH_H

#include "node.h"
#include "nsw_graph.h"
#include <vector>
#include <string>

struct SearchResult {
    std::string text;
    double distance;
};

// search()
std::vector<SearchResult> search(
    NSWGraph* graph,
    const std::vector<double>& queryVector,
    int k,
    int efSearch
);
// bruteForceSearch()
// Checks EVERY node — no graph traversal.
// Used as ground-truth for hit-rate measurement.
std::vector<SearchResult> bruteForceSearch(
    const NSWGraph* graph,
    const std::vector<double>& queryVector,
    int k
);
#endif // SEARCH_H
