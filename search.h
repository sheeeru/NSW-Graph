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

#endif // SEARCH_H
