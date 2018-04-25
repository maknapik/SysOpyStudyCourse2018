#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>

void lib_sort(char *filename, int elements, int size);
char* lib_get_record(FILE *file, int size, int index);
void lib_insert_record(FILE *file, int size, int index, char* content);
void sys_sort(char *filename, int elements, int size);
char* sys_get_record(int file, int size, int index);
void sys_insert_record(int file, int size, int index, char* content);
void generate(char *filename, int elements, int size);
void lib_read_file(char *filename, int elements, int size);

int main(int argc, char* argv[])
{
    if(argc < 5)
    {
        printf("Not enough arguments. (operation filename [other parameters])");
        return 0;
    }

    struct tms *start = (struct tms*) malloc (sizeof(struct tms));
    struct tms *stop = (struct tms*) malloc (sizeof(struct tms));

    if(strcmp("copy", argv[1]) == 0) printf("\nElements: %i, size: %i\n", atoi(argv[4]), atoi(argv[5]));
    else printf("\nElements: %i, size: %i\n", atoi(argv[3]), atoi(argv[4]));

    if(strcmp("generate", argv[1]) == 0)
    {
        printf("\nData generation\n");
        times(start);
        generate(argv[2], atoi(argv[3]), atoi(argv[4]));
        times(stop);
        printf("\nUser time: %ld \nSystem time: %ld \n", stop->tms_utime-start->tms_utime, stop->tms_stime-start->tms_stime);
    }
    if(strcmp("sort", argv[1]) == 0)
    {
        if(argc < 6)
        {
            printf("\nSorting\nMethod was not given [sys/lib]. By default sys");
            times(start);
            sys_sort(argv[2], argv[3], argv[4]);
            times(stop);
            printf("\nUser time: %ld \nSystem time: %ld \n", stop->tms_utime-start->tms_utime, stop->tms_stime-start->tms_stime);
        }
        else
        {
            if(strcmp("sys", argv[5]) == 0)
            {
                printf("\nSorting sys\n");
                times(start);
                sys_sort(argv[2], atoi(argv[3]), atoi(argv[4]));
                times(stop);
                printf("\nUser time: %ld \nSystem time: %ld \n", stop->tms_utime-start->tms_utime, stop->tms_stime-start->tms_stime);
            }
            if(strcmp("lib", argv[5]) == 0)
            {
                printf("\nSorting lib\n");
                times(start);
                lib_sort(argv[2], atoi(argv[3]), atoi(argv[4]));
                times(stop);
                printf("\nUser time: %ld \nSystem time: %ld \n", stop->tms_utime-start->tms_utime, stop->tms_stime-start->tms_stime);
            }

        }
    }
    if(strcmp("copy", argv[1]) == 0)
    {
        if(argc < 7)
        {
            printf("\nCopying\nMethod was not given [sys/lib]. By default sys");
            times(start);
            sys_copy(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
            times(stop);
            printf("\nUser time: %ld \nSystem time: %ld \n", stop->tms_utime-start->tms_utime, stop->tms_stime-start->tms_stime);
        }
        else
        {
            if(strcmp("sys", argv[6]) == 0)
            {
                printf("\nCopying sys\n");
                times(start);
                sys_copy(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
                times(stop);
                printf("\nUser time: %ld \nSystem time: %ld \n", stop->tms_utime-start->tms_utime, stop->tms_stime-start->tms_stime);
            }
            if(strcmp("lib", argv[6]) == 0)
            {
                printf("\nCopying lib\n");
                times(start);
                lib_copy(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
                times(stop);
                printf("\nUser time: %ld \nSystem time: %ld \n", stop->tms_utime-start->tms_utime, stop->tms_stime-start->tms_stime);
            }
        }
    }
    return 0;
}
/*****************************************************************/
void lib_copy(char *file1, char* file2, int elements, int bufsize)
{
    FILE *from = fopen(file1, "r");
    if (from == NULL || fseek(from, 0, SEEK_SET) != 0)
    {
        printf("Error while opening the file\n");
        return 0;
    }
    FILE *to = fopen(file2, "w");
    if (to == NULL || fseek(to, 0, SEEK_SET) != 0)
    {
        printf("Error while opening the file\n");
        return 0;
    }

    char* bufor =(char*)calloc(bufsize,sizeof(char));
    bufsize++;

    for(int i = 0 ; i < elements ;)
    {
        fread(bufor, sizeof(bufor[0]), bufsize, from);
        if(feof(from)) break;
        if(strchr(bufor, '\n') != NULL)
        {
            i++;
        }
        fwrite(bufor, sizeof(char), bufsize, to);
    }

    fclose(from);
    fclose(to);
}
/*****************************************************************/
void sys_copy(char *file1, char* file2, int elements, int bufsize)
{
    int from = open(file1, O_RDONLY);
    if (from < 0)
    {
        printf("Error while opening the file.\n");
        return 0;
    }
    int to = open(file2, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if (to < 0)
    {
        printf("Error while opening the file.\n");
        return 0;
    }

    char* bufor =(char*)calloc(bufsize,sizeof(char));
    bufsize++;

    for(int i = 0 ; i < elements ;)
    {
        if(read(from, bufor, bufsize) == 0) break;
        if(strchr(bufor, '\n') != NULL)
        {
            i++;
        }
        write(to, bufor, bufsize);
    }

    close(from);
    close(to);
}
/*****************************************************************/
void lib_sort(char *filename, int elements, int size)
{
    FILE *file = fopen(filename, "r+");
    if (file == NULL || fseek(file, 0, SEEK_SET) != 0)
    {
        printf("Error while opening the file\n");
        exit(1);
    }
    for(int i = 1; i < elements; i++)
    {
        int j = i;
        char *x = lib_get_record(file, size, i);
        while((j > 0) && (lib_get_record(file, size, j-1)[0] > x[0]))
        {
            lib_insert_record(file, size, j, lib_get_record(file, size, j-1));
            j--;
        }
        lib_insert_record(file, size, j, x);
    }
    fclose(file);
}
/*****************************************************************/
char* lib_get_record(FILE *file, int size, int index)
{
    char* content =(char*)calloc(size,sizeof(char));
    fseek(file, (size+1)*index, 0); //+1 - \n
    fread(content, sizeof(content[0]), size, file);
    return content;
}
/*****************************************************************/
void lib_insert_record(FILE *file, int size, int index, char* content)
{
    fseek(file, (size+1)*index, 0);
    fwrite(content, sizeof(content[0]), size, file);
}
/*****************************************************************/
void sys_sort(char *filename, int elements, int size)
{
    int file = open(filename, O_RDWR);
    if (file < 0)
    {
        printf("Error while opening the file.\n");
        return 0;
    }

    for(int i = 1; i < elements; i++)
    {
        int j = i;
        char *x = sys_get_record(file, size, i);
        while((j > 0) && (sys_get_record(file, size, j-1)[0] > x[0]))
        {
            sys_insert_record(file, size, j, sys_get_record(file, size, j-1));
            j--;
        }
        sys_insert_record(file, size, j, x);
    }
    close(file);
}
/*****************************************************************/
char* sys_get_record(int file, int size, int index)
{
    lseek(file, (size+1)*index, SEEK_SET);
    char* content =(char*)calloc(size,sizeof(char));
    read(file, content, size);
    return content;
}
/*****************************************************************/
void sys_insert_record(int file, int size, int index, char* content)
{
    lseek(file, (size+1)*index, SEEK_SET);
    write(file, content, size);
}
/*****************************************************************/
void generate(char *filename, int elements, int size)
{
    FILE *file = fopen(filename, "w+");
    if (file == NULL || fseek(file, 0, SEEK_SET) != 0)
    {
        printf("Error while opening the file\n");
        return 0;
    }

    for(int i = 0 ; i < elements ; i++)
    {
        char content[size];
        for(int j = 0 ; j < size ; j++)
        {
            content[j] = '0' + rand() % 72;
        }
        fwrite(content, sizeof(content[0]), size, file);
        fwrite("\n", sizeof(char), 1, file);
    }
    fclose(file);
}
/*****************************************************************/
void lib_read_file(char *filename, int elements, int size)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL || fseek(file, 0, SEEK_SET) != 0)
    {
        printf("Error while opening the file\n");
        return 0;
    }

    for(int i = 0 ; i < elements ; i++)
    {
        char content[size];
        fread(content, sizeof(content[0]), size, file);
        printf("\"");
        for(int j = 0 ; j < size ; j++)
        {
            printf("%c", content[j]);
        }
        fseek(file, 1, 1);
        printf("\"\n");
    }
    fclose(file);
}
