#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
 
#include "query_ioctl.h"
 
void get_vars(int i, int fd)
{
    query_arg_t q;
 
    q.i = i;
    if (ioctl(fd, QUERY_GET_VARIABLES, &q) == -1)
    {
        perror("query_apps ioctl get");
    }
    else
    {
        printf("Data : %d\n", q.data);
    }
}
void clr_vars(int i, int fd)
{
    if (ioctl(fd, QUERY_CLR_VARIABLES) == -1)
    {
        perror("query_apps ioctl clr");
    }
}
void set_vars(int i, int fd)
{
    int v;
    query_arg_t q;
 
    printf("Enter Data: ");
    scanf("%d", &v);
    getchar();
    q.i = i;
    q.data = v;
    if (ioctl(fd, QUERY_SET_VARIABLES, &q) == -1)
    {
        perror("query_apps ioctl set");
    }
}
 
int main(int argc, char *argv[])
{
    char *file_name = malloc(sizeof(char)*100);
    int fd;
    enum
    {
        e_get,
        e_clr,
        e_set
    } option;
 
    if (argc == 1)
    {
        option = e_get;
    }
    else if (argc == 3)
    {
        sprintf(file_name, "/dev/query%d", atoi(argv[2]));
        printf("%s\n",file_name);
        if (strcmp(argv[1], "-g") == 0)
        {            
            option = e_get;
        }
        else if (strcmp(argv[1], "-c") == 0)
        {
            option = e_clr;
        }
        else if (strcmp(argv[1], "-s") == 0)
        {
            option = e_set;
        }
        else
        {
            fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
        return 1;
    }
    fd = open(file_name, O_RDWR);
    if (fd == -1)
    {
        perror("query_apps open");
        return 2;
    }
 
    switch (option)
    {
        case e_get:
            get_vars(atoi(argv[2]), fd);
            break;
        case e_clr:
            clr_vars(atoi(argv[2]), fd);
            break;
        case e_set:
            set_vars(atoi(argv[2]), fd);
            break;
        default:
            break;
    }
 
    close (fd);
 
    return 0;
}
