#ifndef _HEADER_H_
#define _HEADER_H_
#include <pthread.h>

#define CACHE_LINE_SIZE 64

// Struct Barrier Node
struct barrier_node {
    pthread_mutex_t count_lock;
    pthread_cond_t ok_to_proceed;
    int count;
};

// used to store the nByPSquare elements of each thread and also 
// used to store the partition for each thread (for e.g. 0th partition of all threads for thread 0
struct miscInfoArray
{
    int *beginPtr;
    int elementCount;
    int currPosition;
    char alignFactor[52];
};

// Message for all to all broadcast algorithm to compute the prefix sum
struct prefixSumPacketInfo
{
    int prefixSum;
    int prefixSumPacket;
    char alignFactor[56];
};
extern int* pqsort(int* input, int num_elements, int num_threads);
int compare(const void * a, const void * b);
// Global Variables
int *pivotsArray; // Array containing the p-1 pivots
int pivotsCount;  // Total Number of unique Pivots
struct prefixSumPacketInfo *prefixSumPacketInfoList; 
int logThreadNum;
struct miscInfoArray *customArray;
struct barrier_node *barrierAttrSecond;

int compare_int_keys(register void* key1, register void* key2);
#endif
