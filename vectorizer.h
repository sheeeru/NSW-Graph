#ifndef VECTORIZER_H
#define VECTORIZER_H

// ============================================================
// vectorizer.h — Input Processing Layer
// Owner: Arqish (Vectorization & Math Specialist)
//
// Responsibility: Convert raw text into numerical vectors
// and compute distance between vectors.
//
// Cross-dependencies:
//   - Areeba stores the output of vectorize() inside Node.numericalVector
//   - Hira calls cosineSimilarity() repeatedly inside the search loop
// ============================================================

#include <string>
#include <vector>

using namespace std;

// ------------------------------------------------------------
// getTrigrams(text)
// Extracts all overlapping 3-character sequences from text.
//
// Uses a sliding window of size 3.
//
// Examples:
//   "hello"     -> ["hel", "ell", "llo"]
//   "cat"       -> ["cat"]
//   "hi"        -> ["hi "]        (padded with spaces if < 3 chars)
//   "a"         -> ["a  "]        (padded with spaces if < 3 chars)
//   ""          -> []             (empty string, no trigrams)
//
// Viva Defense:
//   "We use a sliding window of 3 characters. It is typo-tolerant
//    because 'apple' and 'aple' still share the trigram 'ple',
//    allowing the system to recognize them as similar."
// ------------------------------------------------------------
vector<string> getTrigrams(const string& text);

// ------------------------------------------------------------
// hashTrigram(trigram)
// Maps a 3-character string to an index in [0, VECTOR_DIM).
//
// Uses std::hash<string> to get a size_t, then applies modulo.
//
// Viva Defense:
//   "Instead of storing a massive dictionary of words that grows
//    infinitely, we use hash % 300. This guarantees FIXED memory
//    usage regardless of how many unique words the user enters.
//    This is called the Hashing Trick."
// ------------------------------------------------------------
int hashTrigram(const string& trigram);

// ------------------------------------------------------------
// vectorize(text)
// Converts a text string into a fixed-size numerical vector.
//
// Process:
//   1. Extract trigrams via getTrigrams()
//   2. Hash each trigram to get an index
//   3. Increment the value at that index
//   4. Return the "fingerprint" vector
//
// Result: A vector<double> of size VECTOR_DIM.
// Each index represents a trigram "bucket".
// Higher values = that trigram appeared more often.
//
// Viva Defense:
//   "Dimension 300 is a Goldilocks number. Large enough to
//    minimize hash collisions (keeping data distinct) but small
//    enough to fit in memory and process quickly."
// ------------------------------------------------------------
vector<double> vectorize(const string& text);

// ------------------------------------------------------------
// cosineDistance(A, B)
// Computes the cosine DISTANCE between two vectors.
//
// Formula:
//   similarity = (A . B) / (||A|| * ||B||)
//   distance   = 1 - similarity
//
// Return value:
//   0.0 = identical vectors (same direction)
//   1.0 = opposite vectors
//   ~0.5 = somewhat different
//
// Edge case: If either vector has zero magnitude (e.g., empty text),
// returns 1.0 (maximum distance) to prevent division by zero.
//
// Viva Defense:
//   "We measure the angle between vectors. It is better than
//    Euclidean distance for text because it ignores the length
//    of the sentence (magnitude) and focuses purely on the
//    content pattern (direction)."
// ------------------------------------------------------------
double cosineDistance(const vector<double>& A, const vector<double>& B);

#endif // VECTORIZER_H
