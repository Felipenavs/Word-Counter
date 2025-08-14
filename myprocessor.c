#include <fcntl.h>    
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
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