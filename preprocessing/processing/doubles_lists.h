#ifndef DOUBLES_LISTS_H
#define DOUBLES_LISTS_H

// Libraries.
// ================
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
// ================

// Main structure.
struct list
{
    double value;
    struct list* next;
};

// Prototypes.
struct list* list_new();
int list_is_empty(struct list* l);
int list_len(struct list* l);
void list_print(struct list* l);
void list_destroy(struct list* l);
struct list* list_insert_head(struct list* l, double v);
struct list* list_remove_head(struct list* l);
double* list_to_array(struct list* list);

#endif
