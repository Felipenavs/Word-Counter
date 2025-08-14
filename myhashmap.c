#include <stdlib.h>
#include <string.h>
#include "myutility.h"
#include "myhashmap.h"

#define ARRAY_SIZE 53

//Struct to hold a word node. Holds all the information requiered to sort 
//the linked hashmap as new nodes are being added
typedef struct Word {
    size_t count;
    char * word;
    struct Word *up;
    struct Word *down;
} Word_Node;

static Word_Node * myhashmap[ARRAY_SIZE];

//returns the index position in the array for the word passed
static int get_pos(const char * word){

    int pos;
    if (is_apostrophe(word[0])) {
        pos = 0;
    } else if (is_uppercase(word[0])) {
        pos = word[0] - 'A' + 1;
    } else if (is_lowercase(word[0])) {
        pos = word[0] - 'a' + 27;
    } else {
        // handle unexpected character
        pos = -1;
    }

    return pos;
}

//move a node updwards in the linked list
static void move_nodes(Word_Node * node, Word_Node * previous_node, size_t pos){
    
    previous_node->down = node->down;
    if(node->down){ //not at the bottom of the list
        node->down->up = previous_node;
    }
    node->down = previous_node;
    node->up = previous_node->up;
    previous_node->up = node;
    if(node->up){//not at the top of the list
        node->up->down = node;
    }else{ //at the top of the list
        myhashmap[pos] = node;
    }
}

//recursive function. Compares current node with the one inmmediately on top, if current node is greater by count 
//or equal by count but greater lexicographically, it calls move_nodes() to move current node upwards
static void cmp_previous(Word_Node * node, Word_Node * previous_node, size_t pos){

    if(previous_node == NULL){
        return;
    }
    
    if(node->count > previous_node->count){
        move_nodes(node, previous_node, pos);
        cmp_previous(node, node->up, pos);
    }

    if(node->count == previous_node->count){
        int cmp = strcmp(node->word, previous_node->word);
        if(cmp > 0){
            move_nodes(node, previous_node, pos);
            cmp_previous(node, node->up, pos);
        }
    }
}

//removes the node and frees the word and node 
static void remove_word(Word_Node * node){

    if(node == NULL){
        return;
    }

    int pos = get_pos(node->word);

    if(pos < 0){
        return;
    }


    if(node->up){
        node->up->down = node->down;
        if(node->down){
            node->down->up = node->up;
        }
    }else{
        myhashmap[pos] = node->down;
        if(node->down){
            node->down->up = NULL;  
        }
    }

    node->up = NULL;
    node->down = NULL;
    free(node->word);
    free(node);
}

//adds a node to the hashmap
void add_word(const char * word){

    int pos = get_pos(word);
    if(pos < 0){
        return;
    }

    unsigned slen = strlen(word) + 1;
    char * copy = malloc(slen);
    memcpy(copy, word, slen);

    if(myhashmap[pos] == NULL){
        Word_Node * new_node = (Word_Node *) malloc(sizeof(Word_Node));
        new_node->word = copy;
        new_node->count = 1;
        new_node->up = NULL;
        new_node->down = NULL;
        myhashmap[pos] = new_node;
        return;
    }

    Word_Node * current_node = myhashmap[pos];
    Word_Node * previous_node = NULL;

    while(current_node != NULL){

        int cmp = strcmp(copy, current_node->word);

        if(cmp == 0){
            current_node->count +=1;
            cmp_previous(current_node, current_node->up, pos);
            break;
        }

        previous_node = current_node;
        current_node = current_node->down;

        if(current_node == NULL){
            Word_Node * new_node = (Word_Node *) malloc(sizeof(Word_Node));
            new_node->word = copy;
            new_node->count = 1;
            new_node->up = previous_node;
            new_node->down = NULL;
            previous_node->down = new_node;
            cmp_previous(new_node, new_node->up, pos);
        }
   
    }
    

}

//prints all the words in decreasing order by count of appereances.
void print_words(){

    int flag =1;

    while(flag)
    {

        Word_Node * node = NULL;
        int i = 0;

        for(i; i<ARRAY_SIZE;i++)
        {
            if(myhashmap[i])
            {
                node = myhashmap[i];
                break;
            }
        }

    
        if(node)
        {
            for ( i; i < ARRAY_SIZE; i++)
            {
                if(myhashmap[i]->count > node->count)
                {
                    node = myhashmap[i];
                }
            }

            printf("%s %zu", node->word, node->count);
            remove_word(node);
        }
        else //no more word nodes, end printing
        {
            flag =0;
        }
    }
    
}