#ifndef CONFIG_H
#define CONFIG_H

// Vector dimension for Trigram Hashing.
// 300 is a balance: large enough to minimize hash collisions,
// small enough to fit in memory and compute quickly.
const int VECTOR_DIM = 300;

// Maximum number of neighbors per node (graph degree limit).
// Controls graph density. Range: 8 (sparse, fast) to 16 (dense, accurate).
//une during performance testing.
// const int M = 16;
const int M = 3;

//Exploration factor for Greedy Search.
//Controls how many candidate paths the search keeps open.
//Higher = more accurate but slower. Lower = faster but might miss results.
const int EF_SEARCH = 32;

#endif // CONFIG_H
