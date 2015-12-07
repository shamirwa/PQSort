#include "pqsort.h"
#include "linearbarrier.h"
#include "heap.h"
#include <math.h>

// Comparator function passed in qsort function
int compare (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

// Function to calculate the time difference between start and end struct
long timevaldiff(struct timeval *starttime, struct timeval *endtime)
{
    long milliSec;
    milliSec=(endtime->tv_sec-starttime->tv_sec)*1000;
    milliSec+=(endtime->tv_usec-starttime->tv_usec)/1000;
    return milliSec;
}

void partitionMerge(struct miscInfoArray *structArray,int partitionNum,int *offset,int thread_id)
{

    heap h;
    heap_create(&h, partitionNum, NULL);
    int index = 0;
    int numOfElement = 0;
    struct miscInfoArray *myMiscInfoArray;
    int *partitionKey;

    for(;index < partitionNum; index++)
    {
        if(structArray[index].elementCount > 0)
        {
            heap_insert(&h, structArray[index].beginPtr + structArray[index].currPosition, structArray + index);
            numOfElement = numOfElement +  structArray[index].elementCount;
        }
    }
    index = 0;
    int *merged = (int*)malloc(sizeof(int) * numOfElement);
    while(heap_delmin(&h, (void **)&partitionKey, (void **)&myMiscInfoArray))
    {
        merged[index++] = *partitionKey;

        if(++(myMiscInfoArray->currPosition) < myMiscInfoArray->elementCount){
           heap_insert(&h, myMiscInfoArray->beginPtr + myMiscInfoArray->currPosition, myMiscInfoArray);
        }
    }
    mylib_barrier(barrierAttrSecond,partitionNum);
    memcpy(offset, merged, sizeof(int) * numOfElement);

}

// Api to search for the pivot position in the array of each thread.
int searchPivotIndex(int *inputData, int lowValue, int highValue, int keyValue) 
{
    int midIndex, midValue;
    while (lowValue <= highValue) {
        midIndex = lowValue + (highValue - lowValue) / 2;
        midValue = inputData[midIndex];
        if (midValue < keyValue)
            lowValue = midIndex + 1;
        else if (midValue > keyValue)
            highValue = midIndex - 1;
        else
            return midIndex; 
    }
    return -1*(lowValue + 1);
}

// Thread Function which will be executed by all the threads.
void *threadFunction(void*ptr)
{
    struct threadParam *argument = (struct threadParam*) ptr;
    int myThreadID = argument->threadID;
    int startIndex = 0, elementCount = 0, i = 0, index = 0, currentIndex = 0, pivotEndIndex = 0;
    int paramNbyP = argument->inputArraySize / argument->threadNumber;
    int elementLeft = argument->inputArraySize % argument->threadNumber;
    int nByPSquare = argument->inputArraySize/(argument->threadNumber * argument->threadNumber);
    int *threadNByPElements = (int*)malloc(sizeof(int)*argument->threadNumber);
    
    if(myThreadID < (elementLeft))
    {
        startIndex = myThreadID * (paramNbyP + 1);
        elementCount = paramNbyP + 1;
    }
    else
    {
        startIndex = myThreadID * paramNbyP + elementLeft;
        elementCount = paramNbyP;
    }
    qsort(argument->inputArray+startIndex , elementCount,sizeof(int),compare_int_keys);

    for(i=0;i < argument->threadNumber; i++)
    {
        index = startIndex + nByPSquare * i;
        threadNByPElements[i] = argument->inputArray[index]; 
    }
    customArray[myThreadID].beginPtr = threadNByPElements;
    customArray[myThreadID].elementCount = i;
    customArray[myThreadID].currPosition = 0;
    
    mylib_barrier(barrierAttr,argument->threadNumber);

    partionDataInfo[myThreadID].partitionBeginPtr = (int **)malloc(sizeof(int*) * argument->threadNumber);
    partionDataInfo[myThreadID].numOfPartitionElement = (int*)malloc(sizeof(int) * argument->threadNumber);
    memset(partionDataInfo[myThreadID].partitionBeginPtr,0,sizeof(int*) * argument->threadNumber);
    memset(partionDataInfo[myThreadID].numOfPartitionElement,0,sizeof(int) * argument->threadNumber);
    
    currentIndex = startIndex;
    int pivotIndex = 0, actualIndex = 0;
    for(i=0;i<pivotsCount && currentIndex < startIndex + elementCount;i++)
    {
        if(argument->inputArray[currentIndex] > pivotsArray[i]){
            partionDataInfo[myThreadID].partitionBeginPtr[i] = NULL;
            partionDataInfo[myThreadID].numOfPartitionElement[i] = 0;
            continue;
        }
        //else{


            pivotIndex = searchPivotIndex(argument->inputArray,currentIndex,startIndex+elementCount-1,pivotsArray[i]);
            partionDataInfo[myThreadID].partitionBeginPtr[i] = argument->inputArray + currentIndex;
            pivotEndIndex = pivotIndex;
            if(pivotIndex < 0 )
            {
                actualIndex = -1*(pivotIndex + 1);

                partionDataInfo[myThreadID].numOfPartitionElement[i] = actualIndex - currentIndex;
                currentIndex = actualIndex ;
            }
            else
            {
                partionDataInfo[myThreadID].numOfPartitionElement[i] = pivotEndIndex - currentIndex + 1;
                currentIndex = pivotEndIndex + 1;
            }
       // }
    }
    if(currentIndex <= startIndex + elementCount - 1)
    {
        partionDataInfo[myThreadID].partitionBeginPtr[i] = argument->inputArray + currentIndex;
        partionDataInfo[myThreadID].numOfPartitionElement[i] = startIndex + elementCount -  currentIndex;
    }
    
     mylib_barrier_second(barrierAttrSecond,argument->threadNumber);

     int currentThreadElementCount = 0;
     struct miscInfoArray *currThreadMergeInfo = (struct miscInfoArray*)malloc(sizeof(struct miscInfoArray)*argument->threadNumber);
     for(i = 0;i < argument->threadNumber; i++)
     {
        currentThreadElementCount += partionDataInfo[i].numOfPartitionElement[myThreadID]; 
        currThreadMergeInfo[i].beginPtr =partionDataInfo[i].partitionBeginPtr[myThreadID]; 
        currThreadMergeInfo[i].elementCount =partionDataInfo[i].numOfPartitionElement[myThreadID]; 
        currThreadMergeInfo[i].currPosition = 0;
     }
   
     prefixSumPacketInfoList[myThreadID].prefixSumPacket = currentThreadElementCount;
     prefixSumPacketInfoList[myThreadID].prefixSum = 0;
     
     mylib_barrier_second(barrierAttrSecond,argument->threadNumber);
     
     computePrefixSum(argument->threadNumber, myThreadID);
     int offset = prefixSumPacketInfoList[myThreadID].prefixSum;
     
     partitionMerge(currThreadMergeInfo,argument->threadNumber,argument->inputArray+offset,myThreadID);

     mylib_barrier_second(barrierAttrMain,argument->threadNumber+1);
}

int *pqsort(int* input, int num_elements, int num_threads)
{
  // YOUR CODE GOES HERE
  
    // Variable declaration
    struct timeval start, end;  // timeval struct to measure run time
    int threadId = 0, threadNumLocal = 0, i=0;

    // check if number of input elements to sort is less than number of provided thread
    if(num_elements < num_threads)
    {
        num_threads = num_elements;
    }
    //thread parameter struct to pass to thread function
    struct threadParam *threadFuncArg = (struct threadParam*)malloc(sizeof(struct threadParam)*num_threads); 
    
    // create the pthread_t variable for all the threads
    pthread_t *threadList = (pthread_t*)malloc(sizeof(pthread_t)*num_threads);
    // Initialize the parameters of thread arguments structure
    for(;threadId < num_threads; threadId++)
    {
        threadFuncArg[threadId].threadID = threadId;
        threadFuncArg[threadId].inputArray = input;
        threadFuncArg[threadId].inputArraySize = num_elements;
        threadFuncArg[threadId].threadNumber = num_threads;
    }
    
    // Memory Allocation to global variable defined in header.h
    customArray = (struct miscInfoArray*)malloc(sizeof(struct miscInfoArray) * num_threads);
    barrierAttr = (struct barrier_node*)malloc(sizeof(struct barrier_node));
    barrierAttrSecond = (struct barrier_node*)malloc(sizeof(struct barrier_node));
    partionDataInfo = (struct partitionData*)malloc(sizeof(struct partitionData) * num_threads);
    prefixSumPacketInfoList = (struct prefixSumPacketInfo*)malloc(sizeof(struct prefixSumPacketInfo)*num_threads);
    memset(prefixSumPacketInfoList,0,sizeof(struct prefixSumPacketInfo) * num_threads);
    
    // Initialize the barrier function attribute
    mylib_init_barrier (barrierAttr);
    mylib_init_barrier (barrierAttrSecond);

    // calculate the number of neighbours for each thread from the number of threads
    threadNumLocal = num_threads;

    logThreadNum = log(threadNumLocal)/log(2);
    
    // upper bound on number of threads for odd number of nodes
    if(1<<logThreadNum < num_threads){
        logThreadNum++;
    }
    
    // get the start time of the sort
    gettimeofday(&start, NULL);
     
    // create the list of threads and call the thread function with the thread arguments
    for(i=0;i< num_threads; i++)
    {
        pthread_create(threadList+i,NULL,threadFunction,threadFuncArg+i);
    }

    // wait for all the thread to join here. This will wait until the last thread finishes its work and is merged to the parent
    // process. When all the threads are done with their work they will terminate on join.
    barrierAttrMain = (struct barrier_node*)malloc(sizeof(struct barrier_node));
    mylib_init_barrier(barrierAttrMain);
    mylib_barrier_second(barrierAttrMain, num_threads + 1);
    
    // get the end time of the sort
    gettimeofday(&end, NULL);

    printf("Parallel Run Time in millisecond to sort is: %ld\n",timevaldiff(&start,&end));
    
    return input; //return appropriately
}

