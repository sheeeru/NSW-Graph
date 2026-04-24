#include "search.h"

#include <queue>
#include <unordered_set>
#include <algorithm>
#include "vectorizer.h"
using namespace std;


// general priority queue syntax from the library priority_queue<Type,Container,Comparator>

//         MAIN SEARCH FUNCTION         //
vector<SearchResult> search(NSWGraph* graph,const vector<double>& queryVector,int k,int efSearch){
    vector<SearchResult> final_results; //where we will keep our answers

    //first we shall check for an important  condition which is to check if graph empty
    if (graph==nullptr){ return {};}

    Node* entry_node= graph->getEntryPoint(); //we will need this to begin our search

    if (entry_node == nullptr) { return {}; } 

    //now we create a priority queue for min heap
    priority_queue < pair<double,Node*>,vector<pair<double, Node*>>,greater<pair<double, Node*>>> candidates_min;
    priority_queue<pair<double, Node*>> results; //max heap for results to have an early stop
    unordered_set<Node*> visited_nodes; //just use a USET for visited nodes
    double distance= cosineDistance(queryVector,entry_node->numericalVector); // we will use distance to implement our greedy search

    //now we will push our first entry to min and max heap

    candidates_min.push({distance,entry_node}); 
    results.push({distance,entry_node});
    visited_nodes.insert(entry_node); //it is now visited too

    while (!candidates_min.empty()){
        pair<double,Node*> current = candidates_min.top(); 
        candidates_min.pop(); //get the smallest distance node of this node we have popped

        Node* current_node = current.second;
        double current_dist = current.first;

        // results is a MAX-heap so results.top() is always the
        // WORST (largest distance) result we have found so far.
        //
        // If the current candidate is already worse than our worst
        // result AND we already have K results, every remaining
        // candidate in the min-heap will also be worse (heap property).
        // No point continuing — we cannot improve our top-K anymore.
        // --------------------------------------------------------
        if ((int)results.size() >= k && current_dist > results.top().first){
            break;
        }

        for (int i = 0; i < (int)current_node->neighbors.size(); i++){ //loop through to find the neighbours of a node
            if (visited_nodes.count(current_node->neighbors[i]) == 0){ // not visited before (count tells existence in unordered set, 1 if exists 0 otherwise)
                    
                    visited_nodes.insert(current_node->neighbors[i]); //mark it visited since its visited now

                    double distance_2 = cosineDistance(queryVector, current_node->neighbors[i]->numericalVector); //calc distance between query vector and current neighbour
                    
                    // --------------------------------------------------------
                    // efSearch controls the beam width of the search.
                    // We only add a neighbor to candidates if we have not yet
                    // reached efSearch candidates, OR if this neighbor is
                    // closer than our current worst result.
                    // This prevents the candidate heap from growing too large
                    // and keeps the search focused on promising nodes.
                    // --------------------------------------------------------
                    if ((int)candidates_min.size() < efSearch || distance_2 < results.top().first){
                        candidates_min.push({distance_2, current_node->neighbors[i]});
                    }

                    if ((int)results.size() < k){
                        results.push({distance_2, current_node->neighbors[i]});
                    }
                    else if (distance_2 < results.top().first){
                        results.pop();
                        results.push({distance_2, current_node->neighbors[i]});
                    }
            }
        }
    }

    while (!results.empty()){
        final_results.push_back({ //use max heap to get final results
            
            results.top().second->text,
            results.top().first
        });
        results.pop();
    }

    reverse(final_results.begin(), final_results.end()); //because max heap gives worst to best, we reverse to get best to worst
    return final_results;
}


// ============================================================
// bruteForceSearch()
// Scans every node in the graph linearly — O(N).
// Used ONLY for benchmarking: run this alongside search() on
// datasets of increasing size to prove NSW is faster.
// Also useful as a correctness check — results should roughly
// match search() output.
// ============================================================
vector<SearchResult> bruteForceSearch(
    NSWGraph* graph,
    const vector<double>& queryVector,
    int k
) {
    vector<SearchResult> results;

    if (graph == nullptr || k <= 0) return results;

    const vector<Node*>& nodes = graph->getAllNodes();

    for (Node* node : nodes) {

        double dist = cosineDistance(queryVector, node->numericalVector);

        results.push_back({node->text, dist});
    }

    sort(results.begin(), results.end(),
         [](const SearchResult& a, const SearchResult& b) {
             return a.distance < b.distance;
         });

    if (results.size() > (size_t)k) {
        results.resize(k);
    }

    return results;
}
