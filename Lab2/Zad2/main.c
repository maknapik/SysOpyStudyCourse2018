#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <time.h>
#include <ftw.h>

int type;
struct tm* base;
/*********************************************************************************/
void fileinfo(struct stat* info)
{
    if(S_ISDIR(info->st_mode)) printf("d");
    else printf("-");

    if(info->st_mode & S_IRUSR) printf("r");
    else printf("-");

    if(info->st_mode & S_IWUSR) printf("w");
    else printf("-");

    if(info->st_mode & S_IXUSR) printf("x");
    else printf("-");

    if(info->st_mode & S_IRGRP) printf("r");
    else printf("-");

    if(info->st_mode & S_IWGRP) printf("w");
    else printf("-");

    if(info->st_mode & S_IXGRP) printf("x");
    else printf("-");

    if(info->st_mode & S_IROTH) printf("r");
    else printf("-");

    if(info->st_mode & S_IWOTH) printf("w");
    else printf("-");

    if(info->st_mode & S_IXOTH) printf("x");
    else printf("-");
}
/*********************************************************************************/
void analyzedir(char* dirpath)
{
    strcat(dirpath,"/");
    struct dirent* file;
    DIR* directory;

    if(directory = opendir(dirpath))
    {
        char* realpath=malloc(PATH_MAX+1);
        char lastmodification[20];
        struct stat info;

        while(file = readdir(directory))
        {
            if(strcmp(".",file->d_name) == 0 || strcmp("..",file->d_name) == 0) continue;

            strcpy(realpath, dirpath);
            strcat(realpath, file->d_name);

            lstat(realpath, &info);

            if(S_ISDIR(info.st_mode))
            {
                analyzedir(realpath);
            }
            else if(!S_ISREG(info.st_mode)) continue;
            if(timedifference(base, localtime(&(info.st_mtime))) != type) continue;

            struct tm* timeinfo = localtime(&(info.st_mtime));
            strftime(lastmodification, 20, "%b %d %H:%M", timeinfo);
            printf("#Absolute path: %s\n", realpath);
            printf("#Size: %d\n", (int)info.st_size);
            printf("#Permissions: ");
            fileinfo(&info);
            printf("\n#Last modification: %s\n\n", lastmodification);
        }
        closedir(directory);
        free(realpath);
    }
    else
    {
        printf("Cannot open path: %s", dirpath);
    }
}
/*********************************************************************************/
static int nftwanalyze(const char *dirpath, const struct stat *info, int flag, struct FTW *ftwbuf)
{
    if(timedifference(base, localtime(&info->st_mtime)) != type) return 0;

    char lastmodification[20];
    struct tm* timeinfo = localtime(&info->st_mtime);
    strftime(lastmodification, 20, "%b %d %H:%M", timeinfo);
    printf("#Absolute path: %s\n", realpath(dirpath, NULL));
    printf("#Size: %d\n", (int)info->st_size);
    printf("#Permissions: ");
    fileinfo(info);
    printf("\n#Last modification: %s\n\n", lastmodification);
    return 0;
}
/*********************************************************************************/
struct tm* getdate(const char* date)
{
    time_t rawtime;
    time ( &rawtime );

    char mon[2], day[2], year[4];
    mon[0] = date[3];
    mon[1] = date[4];
    day[0] = date[0];
    day[1] = date[1];
    year[0] = date[6];
    year[1] = date[7];
    year[2] = date[8];
    year[3] = date[9];
    struct tm* returndate = malloc(sizeof(struct tm*));
    returndate->tm_mon = atoi(mon) - 1;
    returndate->tm_mday = atoi(day);
    returndate->tm_year = atoi(year);
    return returndate;
}
/*********************************************************************************/
int timedifference(const struct tm* base, const struct tm* actual) //0 equal, 1 base is later, -1 actual is later
{
    if(base->tm_year == actual->tm_year+1900)
    {
        if(base->tm_mon == actual->tm_mon)
        {
            if(base->tm_mday == actual->tm_mday) return 0;
            else if(base->tm_mday > actual->tm_mday) return 1;
            else return -1;
        }
        else if(base->tm_mon > actual->tm_mon) return 1;
        else return -1;
    }
    else if(base->tm_year > actual->tm_year+1900) return 1;
    else return -1;
}
/*********************************************************************************/
char *comp;
/*********************************************************************************/
int main(int argc, const char* argv[])
{
    if(argc < 4)
    {
        printf("Not enough arguments. Format: path [e]arlier|[l]ater|= date (date format: 07.05.1997 - seventh of may of 1997)");
        return 0;
    }

    if(strcmp("e", argv[2]) == 0) type = 1;
    else if(strcmp("l", argv[2]) == 0) type = -1;
    else type = 0;

    base = getdate(argv[3]);

    char realp[PATH_MAX+1];
    realpath(argv[1],realp);
    analyzedir((char*)&realp);
    printf("\n*******************************NFTW****************************************\n");
    nftw((char*)&realp, nftwanalyze, 10, FTW_PHYS);
}
