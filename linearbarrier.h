#ifndef _LinearBarrier_H_
#define _LinearBarrier_H_
#include "header.h"

void mylib_init_barrier(struct barrier_node *b);

void mylib_barrier (struct barrier_node *b, int num_threads);

#endif
