#pragma once

int max(int x, int y);
void print(char* s);
void printi(int n);
void printc(char n);

struct Node
{
    char data;
    struct Node *next;
};

struct Node* newList();

void free_list(struct Node* list);
void insert_list(struct Node* list, char nb);
int remove_list(struct Node* list, char nb);
void print_list(struct Node* list);
