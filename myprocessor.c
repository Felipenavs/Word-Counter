#include <fcntl.h>    
#include <unistd.h> 
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "myutility.h"
#include "myhashmap.h"


#define BUFFSIZE 2048

//main buffer to read from the file
static char buffer[BUFFSIZE];


//helper function to increase size of the sub_buffer if the word is greater than the current size
static void increase_sbuff_size(char ** sub_buffer, int * current_sbuff_size){

    * current_sbuff_size *= 2;
    char * temp = realloc(*sub_buffer, *current_sbuff_size);
    if(!temp){
        free(*sub_buffer);
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    *sub_buffer = temp;
}

//helper function to determine if a file ends in '.txt' 
static int ends_with_txt(const char *filename) {
    const char *dot = strrchr(filename, '.'); // find last dot
    return dot && strcmp(dot, ".txt") == 0;
}


//process the file adhering to the parameters set by the project description
void process_file(const char * file_path){

    int bytes;
    int word_size = 0;
    int sub_buffer_size = 32;

    char * sub_buffer = malloc(sub_buffer_size);
    if (!sub_buffer) 
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) 
    {
        free(sub_buffer);
        perror(file_path);
        exit(EXIT_FAILURE);
    }

    //runs until EOF 
    while ((bytes = read(fd, buffer, BUFFSIZE)) > 0)
    {
        
        for(int i=0; i<bytes; i++)
        {

            unsigned char character = buffer[i];

            if((is_dash(character) || !is_valid_char(character)) && (word_size < 1))
            {
                continue;
            }

            if(is_dash(character) && (word_size > 0))
            {   //if current char is a dash, make sure that the previously saved char is not a dash as well
                if(is_dash(sub_buffer[word_size-1]))
                {
                    sub_buffer[word_size-1] = '\0';
                    word_size =0;
                    add_word(sub_buffer);
                    continue;
                }

                if(word_size == sub_buffer_size)
                {
                    increase_sbuff_size(&sub_buffer, &sub_buffer_size);
                }

                sub_buffer[word_size] = character;
                word_size +=1;
                continue;
            }

            if((word_size>0) && (!is_valid_char(character)))
            {
                if(word_size == sub_buffer_size)
                {
                    increase_sbuff_size(&sub_buffer, &sub_buffer_size);
                }
                sub_buffer[word_size] = '\0';
                word_size =0;
                add_word(sub_buffer);
                continue;
            }

            if(word_size == sub_buffer_size)
            {
                increase_sbuff_size(&sub_buffer, &sub_buffer_size);
            }
            sub_buffer[word_size] = character;
            word_size +=1;
        }

        
    }

    if(bytes < 0)
    {
        free(sub_buffer);
        perror(file_path);
        exit(EXIT_FAILURE);
    }

    //EOF
    if(word_size > 0)
    {
        //makes sure word doesnt end with a dash
        if(is_dash(sub_buffer[word_size-1]))
        {
            sub_buffer[word_size-1] = '\0';
            add_word(sub_buffer);
        }
        else
        {
            if(word_size == sub_buffer_size)
            {
                increase_sbuff_size(&sub_buffer, &sub_buffer_size);
            }

            sub_buffer[word_size] = '\0';
            add_word(sub_buffer);
        }
    }

    free(sub_buffer);
    close(fd);
}

//process directories and sub-directories recursively. 
//Skip entries that start with a '.' ej: parent dir, own dir and hidden files. 
//Also skips any file that doesnt end with ".txt"
void process_directory(const char * dir_path){

    DIR * dir = opendir(dir_path);
    if(!dir)
    {
        perror(dir_path);
        return;
    }

    size_t buff_size = 1024;
    char * path = malloc(buff_size);
    if(!path)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    struct dirent * de = NULL;
    struct stat sb;

    while((de = readdir(dir)) != NULL)
    {
        //skips parent dir, own dir, and hidden files
        if(de->d_name[0] == '.'){   
            continue;
        }

        //makes sure buffer is big enough for the path name to be constructed
        if((strlen(dir_path) + strlen(de->d_name)+2) >= buff_size){
            free(path);
            buff_size = (strlen(dir_path) + strlen(de->d_name))+100;
            path = malloc(buff_size);
            if(!path)
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
        }

        sprintf(path,"%s/%s", dir_path, de->d_name);

        int st = stat(path, &sb);
        if(st !=0)
        {
            perror(path);
            continue;
        }

        //if it is a regular file that end in .txt it process the file
        if(S_ISREG(sb.st_mode)){

            if(ends_with_txt(de->d_name))
            {
                process_file(path);
            }

        }//if it is a directory, makes a recursive call to process it
        else if(S_ISDIR(sb.st_mode))
        {
            process_directory(path);
        }
    }

    free(path);
    closedir(dir);
}