/*
 *
 * Last modified : 2017.12.017
 * Hanyang University
 * Computer Science & Engineering
 * Seon Namkung
 *
 * FIFO cache header file.
 *
 */
#ifndef __DLL__
#define __DLL__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CACHE 256
#define MAX_CACHE_SIZE 20

typedef struct node {
        struct node * next;
        char command[CACHE];
}Node;

typedef struct cache {
        Node * header;
        Node * trailer;
        int total_size;
}Cache;

Cache*cache;

void init_cache();
void insert_at_first(Node *);
Node * delete_at_last();
Node * find_Node(int);
void print_cache();

#endif
