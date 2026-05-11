#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "nsw_graph.h"
#include <string>

// Save all node texts to a file (one text per line).
void saveToFile(const NSWGraph* graph, const std::string& filename);

// Load texts from file and insert each into the graph.
void loadFromFile(NSWGraph* graph, const std::string& filename);

#endif // PERSISTENCE_H
