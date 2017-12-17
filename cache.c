/*
 *
 * Last modified : 2017.12.17
 * Hanyang University
 * Computer Science & Engineering
 * Seon Namkung
 *
 * FIFO cache source file.
 *
 */

#include "cache.h"

/*
 * Initiate Cache
 * Must call function when start caching.
 */
void init_cache(){
        cache = (Cache *)malloc(sizeof(Cache));
        Node * header = (Node *)malloc(sizeof(Node));
        Node * trailer = (Node *)malloc(sizeof(Node));


        header->next = trailer;
        trailer->next = NULL;

        strcpy(header->command,"/Header\0");
        strcpy(trailer->command,"/Trailer\0");


        cache->header = header;
        cache->trailer = trailer;
        cache->total_size = 0;

        return;
}

/*
 * Insert node at first that rigth after header.
 */
void insert_at_first(Node * node){
        Node * temp = cache->header;
        if(cache->total_size >= MAX_CACHE_SIZE) {
                delete_at_last();
        }

        node->next = temp->next;
        temp->next = node;

        cache->total_size++;
}

/*
 * Delete oldest node which rigth before trailer.
 * Return deleted node.
 */
Node * delete_at_last(){
        if(cache->total_size ==0) {
                return NULL;
        }
        Node * node = cache->header;
        Node * temp;

        for(int i =0; i<cache->total_size-1; i++) {
                node = node->next;
        }
        temp = node->next;
        node->next = node->next->next;

        cache->total_size--;

        return temp;
}

/*
 * Find object in cache by order.
 * If find, return the node;
 * If not, return Null
 */
Node * find_Node(int order){
        if(order > cache->total_size) {
                return NULL;
        }
        Node * node = cache->header;
        for(int i =0; i<order; i++) {
                node = node->next;
        }
        return node;
}

/*
 * Print cached nodes in cache.
 */
void print_cache(){
        Node * node = cache->header;
        while(node->next != NULL) {
                printf("%s-> ",node->command);
                node = node->next;
        }
        fputs("Trailer\n",stdout);
        return;
}
