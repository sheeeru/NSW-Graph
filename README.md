# IsoCortex: Private Local Text Matching using Single-Layer NSW Graphs

> **CS201: Data Structures II | Spring 2026**
> **Instructor:** Faisal Alvi

## 📖 Abstract

This project builds a simple, efficient graph locally in C++ to quickly find similar text phrases. Each sentence is converted into a number-based vector using **Character Trigram Hashing**. These vectors become nodes in the graph, and a **Greedy Search** algorithm is used to navigate the structure and find the closest match. 

By operating entirely locally, this system provides a complete, privacy-focused solution for data retrieval without uploading sensitive information to the cloud.

---

## ⚠️ The Problem

Users often need to quickly find similar text phrases in datasets like:
* Personal notes
* Confidential legal documents
* Organized code snippets

**Challenges with existing methods:**
* **Standard Data Structures:** Arrays or linked lists require scanning every entry, which is too slow for large datasets ($O(N)$). Binary Search Trees (BSTs) and Hash Tables only work for exact matches (e.g., they cannot match "hello worl" to "hello world").
* **Cloud Solutions:** Cloud-based AI and search solutions are fast but send data online, compromising privacy (e.g., exposing confidential legal documents).

## 💡 Our Solution & Objectives

* **Build a Dynamic NSW Graph:** Create a probabilistic graph structure that allows adding new text entries without rebuilding the whole index.
* **Achieve Fast Search:** Use a Greedy Search algorithm to find the top-K most similar phrases significantly faster than a linear scan.
* **Ensure Privacy:** Keep all data processing local to the user’s machine. No network calls are made.
* **Convert Text to Vectors:** Turn sentences into numerical vectors using Trigram Hashing, making the system highly tolerant to typos.
* **Compare Performance:** Quantitatively measure speed and accuracy against a simple linear search to validate the graph's efficiency.

---

## 🛠️ Data Structures & Algorithms

This project heavily leverages advanced data structures to achieve logarithmic search times. 

### Core Data Structure
The core structure is the **Single-Layer Navigable Small World (NSW) Graph**.
* **Representation:** Adjacency List implemented using C++ Pointers (`std::vector<Node*>`).
* **Node Object:** A struct storing the text, its numerical vector, and a list of neighbor pointers.
* **Edges:** Undirected edges implemented as bidirectional pointer references. The graph enforces a maximum degree limit $M$ (tuned between 8 and 16).
* **Entry Point:** A designated starting node pointer used to begin every search operation.

### Algorithms

#### 1. Text Vectorization (Trigram Hashing)
To mathematically compare text, sentences are converted into fixed-size vectors (dimension 300).
* **Trigram Extraction:** Split text into overlapping 3-character sequences (e.g., "hello" → "hel", "ell", "llo").
* **Feature Hashing:** Each trigram maps to an index using modulo arithmetic: `index = hash(trigram) % 300`
* **Vector Construction:** Increment the value at the computed index to build a numerical "fingerprint" of the text, allowing for efficient large-vocabulary handling with a fixed memory footprint.

#### 2. Distance Metric
We measure the distance between two vectors $A$ and $B$ using **Cosine Similarity**:
`Similarity = (A · B) / (||A|| ||B||)`
The distance is calculated as `1 - Similarity`. This metric is robust for text as it focuses on the direction of the vectors rather than their magnitude.

#### 3. Insertion Algorithm
1. Convert the new text to a vector.
2. Search the existing graph to find the $M$ closest neighbors.
3. Create bidirectional edges between the new node and these $M$ neighbors.
4. Update the `entry_point` to the newly inserted node to ensure recent additions are easily reachable.

#### 4. Greedy Search Algorithm
1. Start at the `entry_point`.
2. Calculate the distance from the query vector to the current node.
3. Examine all neighbors of the current node and move to the one closest to the query.
4. Repeat until a local minimum is reached.
5. **Exploration Factor (`efSearch`):** To prevent getting trapped in poor local matches, a Min-Heap (priority queue) maintains the best nodes found during the search, exploring their neighbors to find better overall matches.

---

## 💻 Tech Stack
* **Language:** C++ 17 (Standard Library only)
* **Containers:** `std::vector`, `std::priority_queue`, `std::unordered_set`
* **Memory Management:** Dynamic node allocation with a master list of pointers for memory safety and cleanup.
* **Storage:** Standard file I/O for local data persistence.

---

## 📈 Expected Outcomes & Challenges

**Deliverables:**
1. A fully functional CLI tool for adding and searching private text notes.
2. An implementation of the NSW Graph demonstrating $O(\log N)$ search behavior as the dataset grows, compared to $O(N)$ brute-force.
3. A robust understanding of Proximity Graphs, ready to be explained with hand-drawn diagrams of the "greedy walk" during the final Viva.

**Challenges to Mitigate:**
* **Local Minima:** Adjusting the `efSearch` parameter to keep a larger candidate set and avoid getting stuck in poor matches.
* **Parameter Tuning:** Finding the optimal maximum number of neighbors ($M$) to balance graph connectivity and search speed.
* **Accuracy Measurement:** Tracking the *Hit Rate* to verify how many of the true top matches the graph successfully finds compared to a linear scan.

---

## 👥 Team Members
* **Areeba Izhar** (`ai09625`)
* **Arqish Zaria** (`az09714`)
* **Noor E Hira** (`nh10085`)
* **Shaheer Qureshi** (`sq09647`)

---

## 📚 References
1. Malkov, Y. A., & Yashunin, D. A. (2018). *Efficient and robust approximate nearest neighbor search using Hierarchical Navigable Small World graphs*. IEEE Transactions on Pattern Analysis and Machine Intelligence.
2. Manning, C. D., Raghavan, P., & Schütze, H. (2008). *Introduction to Information Retrieval*. Cambridge University Press.
