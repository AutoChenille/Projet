#include "doubles_lists.h"


// Inits a new list.
struct list* list_new()
{
    return NULL;
};

// Returns if a list is empty.
int list_is_empty(struct list* l)
{
    return l == NULL;
}

// Returns the length of the list.
int list_len(struct list* l)
{
    int len = 0;

    for (struct list* p = l; p; p = p->next)
        len++;

    return len;
}

// Prints array.
void list_print(struct list* l)
{
    printf("{");
    while(l)
    {
        printf(" %f", l->value);
        l = l->next;
    }
    printf(" }\n");
}

// Frees memory for lists.
void list_destroy(struct list* l)
{
    while (l)
    {
        struct list* next = l->next;
        free(l);
        l = next;
    }
}

// Inserts a value in list.
struct list* list_insert_head(struct list* l, double v)
{
    struct list* tmp;
    tmp = malloc(sizeof(struct list));
    tmp->value = v;
    tmp->next = l;
    return tmp;
}

