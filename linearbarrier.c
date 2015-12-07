#include <pthread.h>
#include <stdio.h>
#include "linearbarrier.h"
#include <stdlib.h>

void mylib_init_barrier(struct barrier_node *b)
{
    b -> count = 0;
    pthread_mutex_init(&(b -> count_lock), NULL);
    pthread_cond_init(&(b -> ok_to_proceed), NULL);
}

void mylib_barrier(struct barrier_node *b, int num_threads)
{
    pthread_mutex_lock(&(b -> count_lock));
    b -> count ++;
    if (b -> count == num_threads) {
        b -> count = 0;
        int *sortedNByPSquareElements = (int*)malloc(sizeof(int)*num_threads*num_threads);
        int i = 0 , totalElement= 0, k=0;
        int pivotIndexFactor = num_threads/2 - 1;
        int pivotIndex = 0;
        
        for(;i<num_threads;i++)
        {
            k=0;
            for(;k < customArray[i].elementCount; k++)
            {
                sortedNByPSquareElements[totalElement++] = customArray[i].beginPtr[k];
            }
        }
        qsort(sortedNByPSquareElements,totalElement,sizeof(int),compare_int_keys);

        //Find all the p-1 pivots from sorted NByPSquareElementArray
        pivotsArray = (int*)malloc(sizeof(int) * (num_threads - 1));
        for(i = 1;i < num_threads;i++)
        {
            pivotIndex =   i*num_threads + pivotIndexFactor;
            if(pivotIndex >= totalElement)
            {
                break;
            }
            //Store unique pivots.
            if(!pivotsCount ||( pivotsArray[pivotsCount - 1] != sortedNByPSquareElements[pivotIndex]))
            {
                pivotsArray[pivotsCount++] = sortedNByPSquareElements[pivotIndex];
            }
        }
        pthread_cond_broadcast(&(b -> ok_to_proceed));
    }
    else{
        while (pthread_cond_wait(&(b -> ok_to_proceed),
                    &(b -> count_lock)) != 0);
    }
    pthread_mutex_unlock(&(b -> count_lock));
}

void mylib_barrier_second(struct barrier_node *b, int num_threads)
{
    pthread_mutex_lock(&(b -> count_lock));
    b -> count ++;
    if (b -> count == num_threads) {
        b -> count = 0;
        pthread_cond_broadcast(&(b -> ok_to_proceed));
    }
    else
        while (pthread_cond_wait(&(b -> ok_to_proceed),
                    &(b -> count_lock)) != 0);
    pthread_mutex_unlock(&(b -> count_lock));
}

// Function to compute the prefix Sum
void computePrefixSum(int num_threads, int thread_id)
{
    int i = 0;
    int neighbourNode = 0;
    for(; i < logThreadNum; i++)
    {
        neighbourNode = thread_id ^ (1<<i); 
        if((neighbourNode < num_threads) && (neighbourNode > thread_id))
        {
            prefixSumPacketInfoList[neighbourNode].prefixSumPacket += prefixSumPacketInfoList[thread_id].prefixSumPacket;
            prefixSumPacketInfoList[neighbourNode].prefixSum += prefixSumPacketInfoList[thread_id].prefixSumPacket;
            prefixSumPacketInfoList[thread_id].prefixSumPacket = prefixSumPacketInfoList[neighbourNode].prefixSumPacket;

        }
        mylib_barrier_second(barrierAttrSecond,num_threads);

    }
}

