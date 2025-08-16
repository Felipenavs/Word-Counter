 #include <unistd.h> 
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "myprocessor.h"
#include "myhashmap.h"



int main(int argc, char *argv[]){
    
    struct stat sb;
    int st = 0;

    if(argc == 1)
    {
        char s[] = "No Arguments Passed\n";
        if(write(STDOUT_FILENO, s, strlen(s)) == -1)
        {
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }


    for(int i =1; i<argc; i++)
    {
        st = stat(argv[i], &sb);

        if(st !=0)
        {
            perror(argv[i]);
            printf("%d",st);
            continue;
        }

        if(S_ISREG(sb.st_mode))
        {
            process_file(argv[i]);
        }
        else if(S_ISDIR(sb.st_mode))
        {
            process_directory(argv[i]);
        }
    }

    print_words();
    exit(EXIT_SUCCESS);
}