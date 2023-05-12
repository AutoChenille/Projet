#include <stdio.h>
#include <stdlib.h>
#include "useful.h"

int max(int x, int y)
{
    return x > y ? x : y;
}

void print(char* s)
{
    printf("%s\n", s);
}

void printi(int n)
{
    printf("%i\n", n);
}

void printc(char n)
{
    printf("%c\n", n);
}

struct Node* newList()
{
    struct Node *head = malloc(sizeof(struct Node));
    head->data = 0;
    head->next = NULL;
    return head;
}

void free_list(struct Node* list)
{
    struct Node *current = list;

    while(current != NULL)
    {
        struct Node *tmp = current;
        current = current->next;
        free(tmp);
    }    
}

void insert_list(struct Node* list, char nb)
{
    struct Node *new = malloc(sizeof(struct Node));
    new->data = nb;
    new->next = list->next;
    list->next = new;
}

int remove_list(struct Node* list, char nb)
{
    if(list->next == NULL)
        return 0;
    struct Node *current = list;
    while(current->next != NULL && current->next->data != nb)
        current = current->next;
    
    if(!(current->next == NULL))
    {
        struct Node* tmp = current->next;
        current->next = current->next->next;
        free(tmp);
        return 1;
    }
    return 0;
}

void print_list(struct Node* list)
{
    struct Node* current = list->next;
    printf("list:");
    while(current != NULL)
    {
        printf("%c", current->data);
        printf("->");
        current = current->next;
    }
    printf("\n");
}
