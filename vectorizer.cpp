// ============================================================
// vectorizer.cpp — Input Processing Layer (Implementation)
// Owner: Member 1 (Vectorization & Math Specialist)
//
// This file implements all text-to-vector conversion functions
// and the cosine distance metric used for comparing vectors.
// ============================================================

#include "vectorizer.h"
#include "config.h"
#include <cmath>    // for sqrt()
#include <string>

// ------------------------------------------------------------
// getTrigrams(text)
// ------------------------------------------------------------
// Sliding window of size 3 over the input text.
// If text is shorter than 3 characters, it is padded with
// spaces so that we still produce a valid trigram.
// An empty string returns an empty vector.
// ------------------------------------------------------------
vector<string> getTrigrams(const string& text) {

    // Edge case: completely empty string
    if (text.empty()) {
        return {};
    }

    // If text is shorter than 3 characters, pad with spaces.
    // Example: "hi" -> "hi " -> trigram: "hi "
    string padded = text;
    while (padded.length() < 3) {
        padded += ' ';
    }

    vector<string> trigrams;

    // Sliding window: extract substrings of length 3
    // Loop runs from index 0 to (length - 3) inclusive.
    // Example: "hello" (length 5) -> i = 0,1,2 -> 3 trigrams
    for (size_t i = 0; i <= padded.length() - 3; i++) {
        trigrams.push_back(padded.substr(i, 3));
    }

    return trigrams;
}

// ------------------------------------------------------------
// hashTrigram(trigram)
// ------------------------------------------------------------
// Uses C++ standard library hash function for strings.
// The hash value is taken modulo VECTOR_DIM to produce an
// index in the valid range [0, VECTOR_DIM).
//
// Note: std::hash is NOT cryptographic. Different runs may
// produce different values (seed randomization). This is fine
// for our use case because we always hash and search within
// the same program execution.
// ------------------------------------------------------------
int hashTrigram(const string& trigram) {
    hash<string> hasher;
    size_t hashValue = hasher(trigram);
    return static_cast<int>(hashValue % VECTOR_DIM);
}

// ------------------------------------------------------------
// vectorize(text)
// ------------------------------------------------------------
// Builds a numerical "fingerprint" of the text.
//
// Process:
//   1. Break text into trigrams (3-char sequences)
//   2. Hash each trigram to get a bucket index
//   3. Increment the count at that bucket
//
// The result is a VECTOR_DIM-sized vector where each position
// represents how many times a particular trigram "bucket"
// was hit. Texts with similar trigrams will have similar vectors.
// ------------------------------------------------------------
vector<double> vectorize(const string& text) {

    // Initialize a vector of VECTOR_DIM zeros
    vector<double> vec(VECTOR_DIM, 0.0);

    // Get all trigrams from the text
    vector<string> trigrams = getTrigrams(text);

    // For each trigram, hash it and increment the count
    for (const string& trigram : trigrams) {
        int index = hashTrigram(trigram);
        vec[index] += 1.0;
    }

    return vec;
}

// ------------------------------------------------------------
// cosineDistance(A, B)
// ------------------------------------------------------------
// Computes cosine DISTANCE = 1 - cosine_similarity.
//
// Why distance and not similarity?
//   In our search algorithm, lower = better (closer match).
//   Distance of 0 = identical, 1 = maximally different.
//
// Zero-vector guard:
//   If a text is empty or produces a zero vector, we cannot
//   compute cosine similarity (division by zero). We return
//   1.0 (maximum distance) to indicate these vectors cannot
//   be meaningfully compared.
// ------------------------------------------------------------
double cosineDistance(const vector<double>& A, const vector<double>& B) {

    // Safety check: vectors must be the same size
    if (A.size() != B.size()) {
        return 1.0; // Invalid comparison
    }

    double dotProduct = 0.0;
    double magnitudeA = 0.0;
    double magnitudeB = 0.0;

    // Single pass: compute dot product and both magnitudes
    for (size_t i = 0; i < A.size(); i++) {
        dotProduct  += A[i] * B[i];
        magnitudeA  += A[i] * A[i];
        magnitudeB  += B[i] * B[i];
    }

    // Convert squared magnitudes to actual magnitudes
    magnitudeA = sqrt(magnitudeA);
    magnitudeB = sqrt(magnitudeB);

    // ZERO-VECTOR GUARD:
    // If either vector has zero magnitude (e.g., empty text ""),
    // cosine similarity is undefined. We return 1.0 (max distance)
    // to safely handle this without crashing.
    if (magnitudeA == 0.0 || magnitudeB == 0.0) {
        return 1.0;
    }

    // Cosine similarity: dot product / (magnitude_A * magnitude_B)
    double similarity = dotProduct / (magnitudeA * magnitudeB);

    // Clamp to [-1, 1] to handle floating-point edge cases
    // (e.g., if rounding errors push similarity slightly above 1.0)
    if (similarity > 1.0) similarity = 1.0;
    if (similarity < -1.0) similarity = -1.0;

    // Convert similarity to distance (0 = identical, 1 = opposite)
    return 1.0 - similarity;
}
