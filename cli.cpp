// cli.cpp — Interactive NSW Graph CLI
// Compile: g++ -std=c++17 -o cli cli.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp persistence.cpp
// Run:     ./cli

#include <iostream>
#include <string>
#include "nsw_graph.h"
#include "vectorizer.h"
#include "search.h"
#include "insert.hpp"
#include "persistence.h"
using namespace std;

void printHelp() {
    cout << "\nCommands:" << endl;
    cout << "  add <text>      — insert a new phrase into the graph" << endl;
    cout << "  search <text>   — find top 3 similar phrases" << endl;
    cout << "  delete <text>   — delete a phrase from the graph" << endl;
    cout << "  show            — print the full graph state" << endl;
    cout << "  count           — print total node count" << endl;
    cout << "  help            — show this menu" << endl;
    cout << "  save <file>     — save all nodes to a text file" << endl;
    cout << "  load <file>     — load nodes from a text file" << endl;
    cout << "  exit            — quit the program" << endl;
    cout << endl;
}

int main() {

    NSWGraph graph;

    cout << "================================================" << endl;
    cout << "  NSW Graph — Private Local Text Search" << endl;
    cout << "  Graph starts empty. Type 'help' for commands." << endl;
    cout << "================================================" << endl;

    printHelp();

    string line;

    while (true) {

        cout << "> ";
        getline(cin, line);

        // skip empty input
        if (line.empty()) {
            continue;
        }

        // parse command and argument
        string command = "";
        string argument = "";

        int spacePos = line.find(' ');
        if (spacePos == string::npos) {
            command = line;
        } else {
            command = line.substr(0, spacePos);
            argument = line.substr(spacePos + 1);
        }

        // ADD
        if (command == "add") {
            if (argument.empty()) {
                cout << "Usage: add <text>" << endl;
                continue;
            }
            insert(&graph, argument);
            cout << "Added. Total nodes: " << graph.getNodeCount() << endl;
        }

        // SEARCH
        else if (command == "search") {
            if (argument.empty()) {
                cout << "Usage: search <text>" << endl;
                continue;
            }
            if (graph.getNodeCount() == 0) {
                cout << "Graph is empty. Add some phrases first." << endl;
                continue;
            }

            vector<SearchResult> results = search(
                &graph,
                vectorize(argument),
                3,
                EF_SEARCH
            );

            if (results.empty()) {
                cout << "No results found." << endl;
            } else {
                cout << "\nTop results for \"" << argument << "\":" << endl;
                for (int i = 0; i < results.size(); i++) {
                    cout << "  [" << i+1 << "] \""
                         << results[i].text
                         << "\"  dist=" << results[i].distance << endl;
                }
                cout << endl;
            }
        }

        // DELETE
        else if (command == "delete") {
            if (argument.empty()) {
                cout << "Usage: delete <text>" << endl;
                continue;
            }
            if (graph.getNodeCount() == 0) {
                cout << "Graph is empty." << endl;
                continue;
            }

            // find the node by text
            const vector<Node*>& allNodes = graph.getAllNodes();
            Node* target = nullptr;
            for (int i = 0; i < allNodes.size(); i++) {
                if (allNodes[i]->text == argument) {
                    target = allNodes[i];
                    break;
                }
            }

            if (target == nullptr) {
                cout << "\"" << argument << "\" not found in graph." << endl;
            } else {
                graph.deleteNode(target);
                cout << "Deleted \"" << argument << "\". Total nodes: "
                     << graph.getNodeCount() << endl;
            }
        }

        // SHOW
        else if (command == "show") {
            graph.printGraph();
        }
        //COUNT
        else if (command == "count") {
            cout << "Total nodes: " << graph.getNodeCount() << endl;
        }

        // SAVE
        else if (command == "save") {
            if (argument.empty()) {
                cout << "Usage: save <filename>" << endl;
                continue;
            }
            saveToFile(&graph, argument);
        }

        // LOAD
        else if (command == "load") {
            if (argument.empty()) {
                cout << "Usage: load <filename>" << endl;
                continue;
            }
            loadFromFile(&graph, argument);
        }

        // HELP
        else if (command == "help") {
            printHelp();
        }

        //EXIT
        else if (command == "exit") {
            cout << "Exiting. Destructor will free all nodes." << endl;
            break;
        }

        //UNKNOWN
        else {
            cout << "Unknown command: \"" << command << "\". Type 'help'." << endl;
        }
    }

    return 0;
}
