#include "SortedList.h"
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
    // First element points to a list node, move it forward one
    // because the first element has a NULL key.
    SortedListElement_t* p = list->next;
    if (element == NULL) return;
    
    if (p != NULL) {
        // If we encounter an empty key then we're back to the beginning of the list
        while (p->key != NULL && strcmp(element->key, p->key) > 0) {
            p = p->next;
        }
    } else {
        // Otherwise the list is empty
        if (opt_yield & INSERT_YIELD) {
            sched_yield();
        }
        list->next = element;
        element->next = list;
        list->prev = element;
        element->prev = list;
        return;
    }
    
    // Insert new element between p and it's previous.
    if (opt_yield & INSERT_YIELD) {
        sched_yield();
    }
    
    p->prev->next = element;
    element->prev = p->prev;
    element->next = p;
    p->prev = element;
}

int SortedList_delete(SortedListElement_t *element) {
    if (element == NULL) return 1;
    
    SortedListElement_t* prevEl = element->prev;
    SortedListElement_t* nextEl = element->next;
    
    if (nextEl == NULL || prevEl == NULL) return 1;
    
    // If pointers don't match up return error
    if (nextEl->prev != element || prevEl->next != element) {
        return 1;
    }
    
    if (opt_yield & DELETE_YIELD) {
        sched_yield();
    }
    // If we now have no nodes in the list
    if (prevEl == nextEl) {
        prevEl->next = NULL;
        nextEl->prev = NULL;
    } else {
        prevEl->next = nextEl;
        nextEl->prev = prevEl;
    }
 
    
    return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
    if (list == NULL) return NULL;
    if (key == NULL) return list;
    
    SortedListElement_t* p = list->next;
    if (p != NULL) {
        while (p->key != NULL) {
            if (strcmp(p->key, key) == 0){
                return p;
            }
            
            if (opt_yield & LOOKUP_YIELD)
                sched_yield();
            
            p = p->next;
        }
    }
    
    return NULL;
}

int SortedList_length(SortedList_t *list) {
    int count = 0;
    
    SortedListElement_t* p = list->next;
    if (p == NULL) return count;
    
    while (p->key != NULL) {
        SortedListElement_t* prevEl = p->prev;
        SortedListElement_t* nextEl = p->next;
        // If pointers don't match up return error
        if (nextEl->prev != p || prevEl->next != p) {
            return -1;
        }
        
        if (opt_yield & LOOKUP_YIELD) {
            sched_yield();
        }
        count++;
        p = p->next;
    }
    
    return count;
}
