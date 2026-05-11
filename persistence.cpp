#include "persistence.h"
#include "insert.hpp"
#include <fstream>
#include <iostream>
using namespace std;

void saveToFile(const NSWGraph* graph, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "[persistence] error: cannot open \"" << filename << "\" for writing." << endl;
        return;
    }
    const vector<Node*>& nodes = graph->getAllNodes();
    for (size_t i = 0; i < nodes.size(); i++) {
        file << nodes[i]->text << "\n";
    }
    file.close();
    cout << "[persistence] saved " << nodes.size() << " nodes to \"" << filename << "\"" << endl;
}

void loadFromFile(NSWGraph* graph, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "[persistence] error: cannot open \"" << filename << "\" for reading." << endl;
        return;
    }
    string line;
    int count = 0;
    while (getline(file, line)) {
        if (!line.empty()) {
            insert(graph, line);
            count++;
        }
    }
    file.close();
    cout << "[persistence] loaded " << count << " nodes from \"" << filename << "\"" << endl;
}
