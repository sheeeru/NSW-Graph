#ifndef CONFIG_H
#define CONFIG_H

//Vector dimension for Trigram Hashing.
//300 is a balance
const int VECTOR_DIM = 300;

//Max number of neighbors per node (graph degree limit).
//Range: 8 (sparse, fast) to 16 (dense, accurate).
//const int M = 16;
const int M = 3;

//Exploration factor for Greedy Search.
//Controls how many candidate paths the search keeps open.
const int EF_SEARCH = 6;

#endif // CONFIG_H
