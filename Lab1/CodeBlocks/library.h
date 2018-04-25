#pragma once

extern char gtable[1000000][1000];
///////////dynamic table//////////////////////////
char** create_table(int elements, int size);
void delete_table(char** table, int elements);
void add_block(char **table, int size);
void delete_block(char **table, int index);
int find_block(char **table, int elements, int size, int index);
//////////////static table///////////////////////
void s_initialize(int elements, int size);
void s_add_block(int index, int size);
void s_delete_block(int index, int size);
int s_find_block(int index, int elements, int size);

