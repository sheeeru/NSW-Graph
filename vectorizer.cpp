#include "vectorizer.h"
#include "config.h"
#include <cmath>    // for sqrt()
#include <string>

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
    for (size_t i = 0; i <= padded.length() - 3; i++) {
        trigrams.push_back(padded.substr(i, 3));
    }

    return trigrams;
}

int hashTrigram(const string& trigram) {
    hash<string> hasher;
    size_t hashValue = hasher(trigram);
    return static_cast<int>(hashValue % VECTOR_DIM);
}

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

//   Distance of 0 = identical, 1 = maximally different.
//   similarity = (A . B) / (||A|| * ||B||)
//   distance   = 1 - similarity
// Return value:
//   0.0 = identical vectors (same direction)
//   1.0 = opposite vectors
//   ~0.5 = somewhat different
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
    if (magnitudeA == 0.0 || magnitudeB == 0.0) { //zero magnitude
        return 1.0;
    }
    double similarity = dotProduct / (magnitudeA * magnitudeB);
    if (similarity > 1.0) similarity = 1.0;
    if (similarity < -1.0) similarity = -1.0;
    return 1.0 - similarity; //convert similarity to diff 
}
