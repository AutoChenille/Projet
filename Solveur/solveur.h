#include <stddef.h>

#pragma once

void check_possibilities(int matrix[][9][9]);

void check_line(int matrix[][9][9], size_t i, size_t j);
void check_column(int matrix[][9][9], size_t i, size_t j);
void check_zone(int matrix[][9][9], size_t i, size_t j);

void intersect(int m1[], int m2[], int m3[]);
