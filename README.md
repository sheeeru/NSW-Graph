# Data Structures II
**CS201** **Spring 2026** **March 28, 2026**

## Project Proposal
### Private Local Text Matching using Single-Layer Navigable Small World (NSW) Graphs

**Team Members:**
* Areeba Izhar `ai09625`
* Arqish Zaria `az09714`
* Noor E Hira `nh10085`
* Shaheer Qureshi `sq09647`

**Instructor:** Faisal Alvi

---

## 1 Abstract
This project builds a simple graph locally in C++ to quickly find similar text phrases by converting each sentence is turned into a number-based vector using Character Trigram Hashing. These vectors become nodes in the graph and Greedy Search is used to find the closest match.

## 2 Introduction

### 2.1 Problem Statement
Users often need to quickly find text phrases that are similar, for example:
* Searching personal notes
* Looking through confidential legal documents
* Organizing code snippets

**Challenges with existing methods:**
* Arrays or linked lists requires scanning every entry, slow for large datasets (O(N))
* Binary Search Trees or Hash Tables works only for exact matches. For example, a BST or hash table storing "hello world" cannot find "hello worl".
* Cloud-based solutions are fast but send data online, which can compromise privacy. For instance, uploading legal documents to an online service could expose the data.

**Need:**
* A data structure that can search for similar text efficiently
* Works offline, without sending data to the cloud

### 2.2 Relevance to Data Structures II
This project is relevant to data structures because it demonstrates how the choice of structure affects efficiency and performance:

* **Graphs:** We use an adjacency list to represent the NSW graph. This goes beyond basic graph traversal to implement "navigability" and "small-world" properties.
* **Greedy Algorithms:** The search algorithm is a greedy best-first search, a fundamental algorithmic paradigm.
* **Priority Queues:** We utilize a Min-Heap to manage candidate nodes during the search process.
* **Hashing:** We use the "Hashing Trick" to convert variable-length text into fixed-size numerical vectors efficiently.

The Navigable Small World (NSW) graph is a probabilistic data structure that builds a network where every node is connected to its nearest neighbors, allowing queries to "hop" through the graph to find matches in logarithmic time.

## 3 Objectives
The main goals of this project are:
* **Build a Dynamic NSW Graph:** Create a graph that lets us add new text entries without rebuilding the whole structure.
* **Fast Search:** Use a Greedy Search algorithm to quickly find the top-K most similar phrases, much faster than checking every entry one by one.
* **Privacy-Friendly:** Keep all data on the user's computer so nothing is sent to the internet.
* **Convert Text to Vectors:** Turn sentences into numerical vectors using Trigram Hashing, making the system tolerant to typos and small changes.
* **Compare Performance:** Check how fast and accurate our method is compared to a simple linear search, to see if the graph approach is worth it.

## 4 Methodology

### 4.1 The Data Structures
The core structure is the Single-Layer Navigable Small World (NSW) Graph.
* **Representation:** Adjacency List implemented using C++ Pointers (`std::vector<Node*>`).
* **Node Object:** A struct storing the text, its numerical vector (array), and a list of neighbor pointers.
* **Edges:** Undirected edges implemented as bidirectional pointer references between Node objects. The graph has a maximum degree limit M, which will be tuned between 8 and 16.
* **Entry Point:** A pointer to a designated starting node used to begin every search operation.

### 4.2 Algorithms

#### 4.2.1 Text Vectorization (Trigram Hashing)
To compare text mathematically, each sentence is converted into a fixed-size vector.
1. **Trigram Extraction:** Split the text into overlapping 3-character sequences (e.g., "hello" → "hel", "ell", "llo").
2. **Feature Hashing:** Each trigram is mapped to an index in a fixed-size vector (dimension 300) using: `index = hash(trigram) % 300`
3. **Vector Construction:** Increment the value at the computed index to build a numerical "fingerprint" of the text.

This approach handles large vocabularies efficiently while using fixed memory.

#### 4.2.2 Distance Metric (Cosine Similarity)
We measure the distance between two vectors A and B using Cosine Similarity.
`similarity = (A · B) / (||A|| ||B||)`
The distance is `1 − similarity`. This metric is robust for text data as it focuses on the angle (direction) rather than the magnitude of the vectors.

#### 4.2.3 Insertion Algorithm
When a new sentence is added:
1. Convert text to a vector.
2. Perform a search on the existing graph to find the M closest neighbors.
3. Create bidirectional edges between the new node and these M neighbors.
4. Update the `entry_point` to the newly inserted node to ensure recent additions are easily reachable for future searches.

This ensures the graph remains "navigable" as new nodes are added into the existing network immediately.

#### 4.2.4 Greedy Search Algorithm
This is the core logic for finding matches.
1. Start at the `entry_point`.
2. Calculate the distance from the query vector to the current node.
3. Examine all neighbors of the current node.
4. **Greedy Step:** Move to the neighbor that is closest to the query.
5. Repeat until no neighbor is closer than the current node (local minimum reached).
6. **Exploration Factor (efSearch):** To make sure the search doesn't get stuck in a poor match, we keep a priority queue (min-heap) of the best nodes found instead of only following the closest neighbor to find better matches.

*(Note: Algorithm 2: SEARCH-LAYER. Sourced from Malkov & Yashunin (2018) is referenced here).*

### 4.3 Tools and Technologies
* **Language:** C++ 17 (Standard Library only).
* **Data Structures:** `std::vector` (for adjacency lists), `std::priority_queue` (for heaps), `std::unordered_set` (for visited tracking).
* **Memory Management:** Nodes are dynamically allocated. A master list of Node pointers is maintained to ensure proper cleanup and memory safety.
* **Privacy:** All processing is local (no network calls). Data persistence handled via standard file I/O.
* **Testing:** Custom test cases comparing Brute-Force vs. NSW runtimes.

### 4.4 Expected Challenges
* **Getting Stuck in Poor Matches:** Sometimes the search might stop at a node that looks good locally, but isn't the best overall. We plan to fix this by adjusting efSearch, which keeps a larger set of candidate nodes during the search.
* **Choosing Graph Parameters:** Setting the right maximum number of neighbors (M) is important. Too few neighbors can make the graph disconnected, while too many can slow down the search. We will test values between 8 and 16.
* **Measuring Accuracy:** To make sure the search works well, we will check our Hit Rate, which tells us how many of the true top matches the algorithm actually finds.

## 5 Expected Outcomes
By the end of this project, we will deliver:
1. A fully functional CLI tool for adding and searching private text notes.
2. A working implementation of the NSW Graph that demonstrates faster search times than brute-force as the dataset grows (demonstrating O(log N) behavior vs O(N)).
3. A robust understanding of Proximity Graphs, able to be explained during the Viva using hand-drawn diagrams of the "greedy walk."

## 6 Project Structure

```
cs201_nsw_project/
├── config.h           # Global constants (M=3, EF_SEARCH=6, VECTOR_DIM=300)
├── node.h             # Node struct definition
├── vectorizer.h       # Vectorizer function declarations
├── vectorizer.cpp     # Trigram hashing & cosine distance implementation
├── nsw_graph.h        # NSWGraph class declaration
├── nsw_graph.cpp      # Graph operations (createNode, addEdge, deleteNode, etc.)
├── search.h           # Search function declarations
├── search.cpp         # Greedy search & brute-force search implementation
├── insert.hpp         # Insert pipeline declaration
├── insert.cpp         # Full insertion flow implementation
├── persistence.h      # Save/load function declarations
├── persistence.cpp    # File I/O for graph persistence
├── cli.cpp            # Interactive CLI (manual testing)
├── main.cpp           # Automated test harness (all tests)
└── README.md          # This file
```

## 7 Build & Test

### 7.1 Prerequisites
* A C++17-compatible compiler (g++ or clang++)
* Terminal / command line access
* No external libraries required (standard library only)

### 7.2 Compilation

Open a terminal and navigate to the project directory:

```bash
cd cs201_nsw_project/
```

**For automated tests** (compiles `main.cpp`):

```bash
g++ -std=c++17 -Wall -Wextra -o test main.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp persistence.cpp
```

**For interactive CLI** (compiles `cli.cpp`):

```bash
g++ -std=c++17 -Wall -Wextra -o cli cli.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp persistence.cpp
```

### 7.3 Automated Testing

Run the test harness to execute all tests at once:

```bash
./test
```

This runs **12 tests** covering every component:

| Test | Component | Member | What it verifies |
|------|-----------|--------|------------------|
| 1 | `getTrigrams()` | Arqish | Sliding window trigram extraction (normal, short, empty, multi-word) |
| 2 | `hashTrigram()` | Arqish | Deterministic hashing, valid range `[0, 300)` |
| 3 | `vectorize()` | Arqish | Text-to-vector conversion, determinism, typo-tolerance, empty input |
| 4 | `cosineDistance()` | Arqish | Identical/opposite/orthogonal vectors, zero-vector guard, size mismatch |
| 5 | End-to-End Vectorizer | Arqish | Full pipeline: text → trigrams → vector → cosine distance → best match |
| 6 | Graph Operations | Areeba | createNode, addEdge, bidirectionality, self-edge/duplicate prevention, entry point |
| 7 | Hard Deletion | Areeba | deleteNode with relinking, edge removal, M-limit relinking, dangling pointer check |
| 8 | `search()` | Hira | Empty graph, correct best match, k=1, k>size, sorted results, typo-tolerance |
| 9 | `bruteForceSearch()` | Shaheer | Correctness against manual calculation, sorting, edge cases (nullptr, k<=0) |
| 10 | Insert Pipeline | Shaheer | First-node entry point, edge creation, empty text rejection, each node is searchable |
| 11 | Hit Rate & Performance | Shaheer | NSW vs brute-force accuracy, timing comparison on 50-node dataset |
| 12 | Persistence (Save/Load) | Shaheer | saveToFile, loadFromFile round-trip, empty graph, file overwrite |

All tests use `assert()` — if any test fails, the program will crash with an error message. A clean run ending with no crash means **all 12 tests passed**.

### 7.4 Manual Testing (Interactive CLI)

Compile and launch the interactive CLI:

```bash
g++ -std=c++17 -Wall -Wextra -o cli cli.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp persistence.cpp
./cli
```

You will see the startup banner and command menu. The graph starts **empty** — you build it by adding phrases.

#### Available Commands

| Command | Description |
|---------|-------------|
| `add <text>` | Insert a new phrase into the graph as a node |
| `search <text>` | Find the top 3 most similar phrases using NSW greedy search |
| `delete <text>` | Remove a phrase from the graph (hard deletion with neighbor relinking) |
| `show` | Print the entire graph state (all nodes and their neighbor connections) |
| `count` | Display the total number of nodes currently in the graph |
| `save <file>` | Save all nodes to a text file for later use |
| `load <file>` | Load nodes from a previously saved text file |
| `help` | Display the command menu |
| `exit` | Quit the program |

#### Example Session

Below is a walkthrough showing how to add phrases, search for matches, inspect the graph, and use save/load:

```
================================================
  NSW Graph — Private Local Text Search
  Graph starts empty. Type 'help' for commands.
================================================

> add data structures and algorithms
Added. Total nodes: 1

> add machine learning fundamentals
Added. Total nodes: 2

> add database management systems
Added. Total nodes: 3

> add operating system concepts
Added. Total nodes: 4

> search data structures

Top results for "data structures":
  [1] "data structures and algorithms"  dist=0.291082
  [2] "database management systems"     dist=0.88906
  [3] "machine learning fundamentals"   dist=0.896995

> search operating systems

Top results for "operating systems":
  [1] "operating system concepts"       dist=0.123456
  [2] "data structures and algorithms"  dist=0.889012
  [3] "database management systems"     dist=1.012345

> count
Total nodes: 4

> show
  [Node 0] "data structures and algorithms"  Neighbors: 3
    -> "machine learning fundamentals"
    -> "database management systems"
    -> "operating system concepts"
  [Node 1] "machine learning fundamentals"  Neighbors: 3
    -> "data structures and algorithms"
    -> "database management systems"
    -> "operating system concepts"
  ...

> save mydata.txt
Saved 4 nodes to mydata.txt

> load mydata.txt
Loaded 4 nodes from mydata.txt

> exit
Exiting. Destructor will free all nodes.
```

#### Understanding Search Results

Each search returns up to **3 results** sorted by **cosine distance** (ascending):
* **Distance 0.000000** = exact match (identical text)
* **Distance < 0.5** = very similar (e.g., a typo or minor word difference)
* **Distance > 0.8** = mostly unrelated text
* **Distance 1.0** = completely orthogonal (no shared trigrams)

The lower the distance, the better the match.

### 7.5 Quick Reference

```bash
# Full cycle: compile, test, and run CLI
cd cs201_nsw_project/

# Run all automated tests
g++ -std=c++17 -Wall -Wextra -o test main.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp persistence.cpp
./test

# Run interactive CLI
g++ -std=c++17 -Wall -Wextra -o cli cli.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp persistence.cpp
./cli
```

## 8 References
1. Malkov, Y. A., & Yashunin, D. A. (2018). Efficient and robust approximate nearest neighbor search using Hierarchical Navigable Small World graphs. IEEE Transactions on Pattern Analysis and Machine Intelligence.
2. Manning, C. D., Raghavan, P., & Schütze, H. (2008). Introduction to Information Retrieval. Cambridge University Press. (For Vector Space Models and Cosine Similarity).
