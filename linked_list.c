#include <stdlib.h>
#include "linked_list.h"

// Function pointers to (potentially) custom malloc() and
// free() functions.
//
static void * (*malloc_fptr)(size_t size) = &malloc;
static void   (*free_fptr)(void* addr)    = &free; 


struct linked_list * linked_list_create(void) {
    struct linked_list * ll = (*malloc_fptr)(sizeof(struct linked_list));
    if (ll == NULL) {
        return NULL;
    }

    ll->head = NULL;
    ll->tail = NULL;
    ll->size = 0;

    return ll;
}

bool linked_list_delete(struct linked_list * ll) {
    if (ll == NULL) {
        return false;
    }

    struct node * curr = ll->head;

    while (curr != NULL) {
        struct node * to_free = curr;
        curr = curr->next;

        (*free_fptr)(to_free);
    }

    ll->head = NULL;
    ll->tail = NULL;
    ll->size = 0;

    (*free_fptr)(ll);

    return true;
}

size_t linked_list_size(struct linked_list * ll) {
    if (ll == NULL) {
        return SIZE_MAX;
    }

    return ll->size;
}

bool linked_list_insert_end(struct linked_list * ll,
                            unsigned int data) {
    if (ll == NULL) {
        return false;
    }

    struct node * new_node = (*malloc_fptr)(sizeof(struct node));
    if (new_node == NULL) {
        return false;
    }

    new_node->next = NULL;
    new_node->data = data;

    if (ll->tail == NULL) {
        ll->head = new_node;
    }
    else {
        ll->tail->next = new_node;
    }
    ll->tail = new_node;
    ll->size += 1;
    
    return true;
}

bool linked_list_insert_front(struct linked_list * ll,
                              unsigned int data) {
    if (ll == NULL) {
        return false;
    }

    struct node * new_node = (*malloc_fptr)(sizeof(struct node));
    if (new_node == NULL) {
        return false;
    }

    new_node->next = ll->head;
    new_node->data = data;

    if (ll->tail == NULL) {
        ll->tail = new_node;
    }

    ll->head = new_node;
    ll->size += 1;

    return true;
}

bool linked_list_insert(struct linked_list * ll,
                        size_t index,
                        unsigned int data) {
    if (ll == NULL) {
        return false;
    }
                            
    if (index > ll->size) {
        return false;
    }

    if (index == 0) {
        return linked_list_insert_front(ll, data);
    }
    if (index == ll->size) {
        return linked_list_insert_end(ll, data);
    }

    struct node * curr = ll->head;

    for (size_t i = 0; i < index-1; i++) {
        curr = curr->next;
    }
    
    struct node * new_node = (*malloc_fptr)(sizeof(struct node));
    if (new_node == NULL) {
        return false;
    }

    new_node->next = curr->next;
    new_node->data = data;

    curr->next = new_node;

    ll->size += 1;

    return true;
}

size_t linked_list_find(struct linked_list * ll,
                        unsigned int data) {
    if (ll == NULL) {
        return SIZE_MAX;
    }

    struct node * curr = ll->head;

    int idx = 0;
    while (curr != NULL) {
        if (curr->data == data) {
            return idx;
        }

        curr = curr->next;
        idx++;
    }
    return SIZE_MAX;
}

bool linked_list_remove(struct linked_list * ll,
                        size_t index) {
    if (ll == NULL) {
        return false;
    }

    if (index >= ll->size) {
        return false;
    }

    struct node * to_free;

    // index=0 and size=1: both head and tail get affected
    if (ll->size == 1) {
        to_free = ll->head;
        ll->head = NULL;
        ll->tail = NULL;
    }
    // the head is affected
    else if (index == 0) {
        to_free = ll->head;
        ll->head = ll->head->next;
    }
    else {
        struct node * curr = ll->head;
        for (size_t i = 0; i < index-1; i++) {
            curr = curr->next;
        }
        to_free = curr->next;
        curr->next = to_free->next;
        
        // the tail is affected
        if (index == ll->size-1) {
            ll->tail = curr;
        }
    }

    ll->size -= 1;
    (*free_fptr)(to_free);

    return true;
}

struct iterator * linked_list_create_iterator(struct linked_list * ll,
                                              size_t index) {
    if (ll == NULL) {
        return NULL;
    }
    if (index >= ll->size) {
        return NULL;
    }

    struct node * curr;
    if (index == ll->size - 1) {
        curr = ll->tail;
    }
    else {
        curr = ll->head;
        for (size_t i = 0; i < index; i++) {
            curr = curr->next;
        }
    }

    struct iterator * new_iter = (*malloc_fptr)(sizeof(struct iterator));
    if (new_iter == NULL) {
        return NULL;
    }

    new_iter->ll = ll;
    new_iter->current_node = curr;
    new_iter->current_index = index;
    new_iter->data = curr->data;

    return new_iter;
}

bool linked_list_delete_iterator(struct iterator * iter) {
    if (iter == NULL) {
        return false;
    }

    (*free_fptr)(iter);
    return true;
}

bool linked_list_iterate(struct iterator * iter) {
    if (iter == NULL) {
        return false;
    }
    if (iter->current_node->next == NULL) {
        return false;
    }

    iter->current_node = iter->current_node->next;
    iter->current_index += 1;
    iter->data = iter->current_node->data;
    return true;
}

bool linked_list_register_malloc(void * (*malloc)(size_t)) {
    malloc_fptr = malloc;
    return true;
}

bool linked_list_register_free(void (*free)(void*)) {
    free_fptr = free;
    return true;
}
