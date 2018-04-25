#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>
#ifndef DLL
#include "library.h"
#endif

int main(int argc, char * argv[])
{
	#ifdef DLL
void *handle = dlopen("./library.so", RTLD_LAZY); //open library
	char**(*create_table)(int, int) = dlsym(handle, "create_table");
	void(*delete_table)(char**, int) = dlsym(handle, "delete_table");
    void(*add_block)(char**, int, int) = dlsym(handle, "add_block");
    void(*delete_block)(char**, int) = dlsym(handle, "delete_block");
    char*(*find_block)(char**, int, int, int) = dlsym(handle, "find_block");
    void(*s_initialize)(int, int) = dlsym(handle, "s_initialize");
    void(*s_add_block)(int, int) = dlsym(handle, "s_add_block");
    void(*s_delete_block)(int, int) = dlsym(handle, "s_delete_block");
    char*(*s_find_block)(int, int, int) = dlsym(handle, "s_find_block");
#endif
	////////////////////////////////////////////////////////////////////
	if (argc < 4)
	{
		printf("Not enough arguments: tablesize blocksize static/dynamic [operations]");
		return 0;
	}
	////////////////////////////////////////////////////////////////////
	/*FILE *f = fopen("raport2.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return(0);
    }*/
    ////////////////////////////////////////////////////////////////////
	int elements = atoi(argv[1]); //number of elements in table
	int size = atoi(argv[2]); //size of a single block
	////////////////////////////////////////////////////////////////////
    struct tms *start = (struct tms*) malloc (sizeof(struct tms));
    struct tms *stop = (struct tms*) malloc (sizeof(struct tms));

    struct timespec *start_real = (struct timespec*) malloc (sizeof(struct timespec));
    struct timespec *stop_real = (struct timespec*) malloc (sizeof(struct timespec));
    ////////////////////////////////////////////////////////////////////
    if (strcmp("static", argv[3]) == 0)
    {
        if(strcmp("c", argv[4]) == 0) //create table
        {
            printf("Creating table");
            times(start);
            clock_gettime(CLOCK_REALTIME,start_real);

            s_initialize(elements, size);

            times(stop);
            clock_gettime(CLOCK_REALTIME, stop_real);
            long int power = (long int)pow(10, 9);
	    long int ns = (stop_real->tv_sec*power - start_real->tv_sec*power + stop_real->tv_nsec - start_real->tv_nsec);
            printf("\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power),ns % power);

            /*fprintf(f, "Creating table");
            fprintf(f, "\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power) ,ns % power);*/
        }
        else
        {
            printf("Table must be created firstly (operation: c)");
            //fclose(f);
            return 0;
        }
        for(int i = 5 ; i < argc ; i++)
        {
            if(strcmp("f", argv[i]) == 0) //find element
            {
                printf("\nFinding index\n");
                times(start);
                clock_gettime(CLOCK_REALTIME,start_real);

                s_find_block(atoi(argv[++i]), elements, size);

                times(stop);
                clock_gettime(CLOCK_REALTIME, stop_real);
                long int power = (long int)pow(10, 9);
	    long int ns = (stop_real->tv_sec*power - start_real->tv_sec*power + stop_real->tv_nsec - start_real->tv_nsec);
            printf("\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power),ns % power);

            /*fprintf(f, "Creating table");
            fprintf(f, "\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power) ,ns % power);*/
            }
            else if(strcmp("ad", argv[i]) == 0) //add and delete blocks
            {
                printf("\nDelete and add blocks");
                times(start);
                clock_gettime(CLOCK_REALTIME,start_real);

                for(int j = 0 ; j < atoi(argv[i+1]) ; j++)
                	s_delete_block(j, size);
                for(int j = 0 ; j < atoi(argv[i+1]) ; j++)
                    	s_add_block(j, size);

                times(stop);
                clock_gettime(CLOCK_REALTIME, stop_real);
                long int power = (long int)pow(10, 9);
	    long int ns = (stop_real->tv_sec*power - start_real->tv_sec*power + stop_real->tv_nsec - start_real->tv_nsec);
            printf("\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power),ns % power);

            /*fprintf(f, "Creating table");
            fprintf(f, "\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power) ,ns % power);*/

                i++;
            }
            else if(strcmp("adc", argv[i]) == 0) //add and delete blocks
            {
                printf("\nDelete and add blocks - cross");
                times(start);
                clock_gettime(CLOCK_REALTIME,start_real);

                for(int j = 0 ; j < atoi(argv[i+1]) ; j++)
                {
                    s_delete_block(j, size);
                    s_add_block(j, size);
                }

                times(stop);
                clock_gettime(CLOCK_REALTIME, stop_real);
                long int power = (long int)pow(10, 9);
	    long int ns = (stop_real->tv_sec*power - start_real->tv_sec*power + stop_real->tv_nsec - start_real->tv_nsec);
            printf("\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power),ns % power);

            /*fprintf(f, "Creating table");
            fprintf(f, "\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power) ,ns % power);*/

                i++;
            }
            else
            {
                printf("Not recognized operation\n");
                //fclose(f);
                return 0;
            }

        }
    }
    else if (strcmp("dynamic", argv[3]) == 0)
    {
        char** table;
        //operations: c - create table, f <num> - find element, ad <num> - add and delete blocks, adc <num> - previous cross
        if(strcmp("c", argv[4]) == 0) //create table
        {
            printf("Creating table");
            times(start);
            clock_gettime(CLOCK_REALTIME,start_real);

            table = create_table(elements, size);

            times(stop);
            clock_gettime(CLOCK_REALTIME, stop_real);
            long int power = (long int)pow(10, 9);
	    long int ns = (stop_real->tv_sec*power - start_real->tv_sec*power + stop_real->tv_nsec - start_real->tv_nsec);
            printf("\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power),ns % power);

            /*fprintf(f, "Creating table");
            fprintf(f, "\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power) ,ns % power);*/
        }
        else
        {
            printf("Table must be created firstly (operation: c)");
            //fclose(f);
            return 0;
        }

        for(int i = 5 ; i < argc ; i++)
        {
            if(strcmp("f", argv[i]) == 0) //find element
            {
                printf("\nFinding index\n");
                times(start);
                clock_gettime(CLOCK_REALTIME,start_real);

                find_block(table, elements, size, atoi(argv[++i]));

                times(stop);
                clock_gettime(CLOCK_REALTIME, stop_real);
                long int power = (long int)pow(10, 9);
	    long int ns = (stop_real->tv_sec*power - start_real->tv_sec*power + stop_real->tv_nsec - start_real->tv_nsec);
            printf("\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power),ns % power);

            /*fprintf(f, "Creating table");
            fprintf(f, "\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power) ,ns % power);*/
            }
            else if(strcmp("ad", argv[i]) == 0) //add and delete blocks
            {
                printf("\nDelete and add blocks");
                times(start);
                clock_gettime(CLOCK_REALTIME,start_real);

                for(int j = 0 ; j < atoi(argv[i+1]) ; j++)
                delete_block(table, j);
                for(int j = 0 ; j < atoi(argv[i+1]) ; j++)
                add_block(table, size, elements-atoi(argv[i+1])-1);

                times(stop);
                clock_gettime(CLOCK_REALTIME, stop_real);
                long int power = (long int)pow(10, 9);
	    long int ns = (stop_real->tv_sec*power - start_real->tv_sec*power + stop_real->tv_nsec - start_real->tv_nsec);
            printf("\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power),ns % power);

            /*fprintf(f, "Creating table");
            fprintf(f, "\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power) ,ns % power);*/

                i++;
            }
            else if(strcmp("adc", argv[i]) == 0) //add and delete blocks
            {
                printf("\nDelete and add blocks - cross");
                times(start);
                clock_gettime(CLOCK_REALTIME,start_real);

                for(int j = 0 ; j < atoi(argv[i+1]) ; j++)
                {
                    delete_block(table, j);
                    add_block(table, size, elements-atoi(argv[i+1])-1);
                }

                times(stop);
                clock_gettime(CLOCK_REALTIME, stop_real);
                long int power = (long int)pow(10, 9);
	    long int ns = (stop_real->tv_sec*power - start_real->tv_sec*power + stop_real->tv_nsec - start_real->tv_nsec);
            printf("\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power),ns % power);

            /*fprintf(f, "Creating table");
            fprintf(f, "\nUser time: %ld \nSystem time: %ld \nReal time: %ld s %ld ns\n",
                stop->tms_utime-start->tms_utime,
                stop->tms_stime-start->tms_stime,
                ns / (power) ,ns % power);*/

                i++;
            }
            else
            {
                printf("Not recognized operation\n");
                delete_table(table, elements);
                //fclose(f);
                return 0;
            }

        }
        delete_table(table, elements);
    }
    else
    {
        printf("Not recognized method (use static or dynamic)");
        //fclose(f);
        return 0;
    }
#ifdef DLL
	//close library after doing all stuff
	dlclose(handle);
#endif
}

