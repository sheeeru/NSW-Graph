#ifndef INSERT_H
#define INSERT_H

#include "nsw_graph.h"
#include "search.h"
#include "vectorizer.h"

//adds a new text phrase into the NSW graph
//handles the full pipeline: text → vector → node → edges → entry point
void insert(NSWGraph* graph, const string& text);

#endif // INSERT_H