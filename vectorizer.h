#ifndef VECTORIZER_H
#define VECTORIZER_H
#include <string>
#include <vector>
using namespace std;

vector<string> getTrigrams(const string& text);
int hashTrigram(const string& trigram);
vector<double> vectorize(const string& text);
double cosineDistance(const vector<double>& A, const vector<double>& B);

#endif 
