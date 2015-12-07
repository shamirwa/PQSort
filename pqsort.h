#ifndef PQSORT_H
#define PQSORT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stddef.h>
#include "header.h"

struct threadParam
{
    int threadID;
    int *inputArray;
    int inputArraySize;
    int threadNumber;
};

struct partitionData
{
    int** partitionBeginPtr;
    int* numOfPartitionElement;
    char alignFactor[56];
};

struct barrier_node *barrierAttr;
struct partitionData* partionDataInfo;
struct barrier_node *barrierAttrMain;
int searchPivotIndex(int *inputData, int lowValue, int highValue, int keyValue);

#endif
