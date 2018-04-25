#include "library.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

char gtable[1000000][1000] = { 0 };
//////////////dynamic table/////////////////////
char** create_table(int elements, int size)
{
	char **table = calloc(elements, sizeof(char*));
	for (int i = 0; i < elements; i++)
	{
		table[i] = calloc(1, sizeof(char)*size);
		for(int j = 0 ; j < size ; j++)
		{
            		table[i][j] = '0' + rand() % 72;
		}
	}
	return table;
}
/////////////////////////////////////////////////
void delete_table(char** table, int elements)
{
	for (int i = 0; i < elements; i++)
	{
		free(table[i]);
		table[i] = NULL;
	}
	free(table);
	table = NULL;
}
/////////////////////////////////////////////////
void add_block(char **table, int size, int index)
{
    	if(table[index] == NULL)
	{
		table[index] = calloc(1, sizeof(char)*size);
	}
}
/////////////////////////////////////////////////
void delete_block(char **table, int index)
{
	free(table[index]);
	table[index] = NULL;
}
/////////////////////////////////////////////////
char* find_block(char **table, int elements, int size, int index)
{
	int sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum += table[index][i];
	}
	int min = 99999;
	char *ind = NULL;
	for (int i = 0; i < elements; i++)
	{
		if (i == index || table[i] == NULL) continue;
		int s = 0;
		for (int j = 0; j < size; j++)
		{
			s += table[i][j];
		}
		s -= sum;
		if (s < 0) s *= -1;
		if (min > s)
		{
			min = s;
			ind = table[i];
		}
	}
	return ind;
}
/////////////////static table////////////////////
void s_initialize(int elements, int size)
{
	for (int i = 0; i < elements; i++)
	{
		for (int j = 0; j < size; j++) gtable[i][j] = '0' + rand() % 72;
	}
}
void s_add_block(int index, int size)
{
	for(int j = 0 ; j < size ; j++)
	{
		gtable[index][j] = '0' + rand() % 72;
	}
}
void s_delete_block(int index, int size)
{
	for (int j = 0; j < size; j++)
	{
		gtable[index][j] = '0';
	}
}
char* s_find_block(int index, int elements, int size)
{
	int sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum += gtable[index*5][i];
	}
	int min = 99999;
	char *ind = NULL;
	for (int i = 0; i < elements; i++)
	{
		if (i == index || gtable[i][0] == '0') continue;
		int s = 0;
		for (int j = 0; j < size; j++)
		{
			s += gtable[i][j];
		}
		s -= sum;
		if (s < 0) s *= -1;
		if (min > s)
		{
			min = s;
			ind = gtable[i];
		}
	}
	return ind;
}
