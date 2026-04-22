#ifndef SEARCH_H
#define SEARCH_H

// ============================================================
// search.h — Greedy Search Algorithm
// Owner: Member 3 (Search Algorithm & Complexity Specialist)
//
// Provides the search function that finds the top-K most similar
// nodes to a query vector by traversing the NSW graph.
//
// Uses:
//   - Member 1's cosineDistance() for comparing vectors
//   - Member 2's Node struct and neighbor pointers for navigation
//   - std::priority_queue (min-heap) for candidate management
//   - std::unordered_set for visited node tracking
//
// DELETION:
//   Deleted nodes (node->is_deleted == true) are automatically
//   skipped during search. They may be traversed through
//   (their edges are still used for navigation) but are never
//   included in search results.
// ============================================================

#include "node.h"
#include "nsw_graph.h"
#include <vector>
#include <string>

// A single search result: the matched text and its distance score
struct SearchResult {
    string text;
    double distance;
    int nodeId;  // ID of the matched node (useful for deletion tracking)
};

// ------------------------------------------------------------
// search(graph, queryVector, k, efSearch)
//
// Performs a greedy best-first search on the NSW graph to find
// the top-K nodes most similar to the query vector.
//
// Parameters:
//   graph        - Pointer to the NSWGraph to search in
//   queryVector  - The vectorized query (output of Member 1's vectorize())
//   k            - Number of results to return (e.g., top 5)
//   efSearch     - Exploration factor: how many candidates to keep open
//                  Higher = more accurate but slower. Uses EF_SEARCH from config.
//
// Returns:
//   A vector of SearchResult, sorted by distance (closest first).
//   Only includes ACTIVE (non-deleted) nodes.
//   Returns empty vector if graph is empty or k <= 0.
//
// Algorithm outline:
//   1. Start at graph->getEntryPoint()
//   2. Initialize visited set and candidate priority queue (min-heap)
//   3. While candidates exist:
//      a. Pop closest candidate
//      b. If candidate is deleted, skip it (but still explore neighbors)
//      c. Check all its neighbors
//      d. Add unvisited neighbors to candidates
//      e. Track the K best ACTIVE results found so far
//      f. Early stop: if current candidate is worse than the K-th best, stop
//   4. Return top-K results sorted by distance (excluding deleted nodes)
//
// DELETION HANDLING:
//   - Deleted nodes CAN be traversed through (their edges are valid).
//   - Deleted nodes are NOT added to the results list.
//   - When checking early stopping, only compare against active results.
// ------------------------------------------------------------
vector<SearchResult> search(
    NSWGraph* graph,
    const vector<double>& queryVector,
    int k,
    int efSearch
);

#endif // SEARCH_H
