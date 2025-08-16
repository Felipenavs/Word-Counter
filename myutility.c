#include "myutility.h"

int is_lowercase(char a){
    if(a <= 122 && a >=97){
        return 1;
    }
    return 0;
}

int is_uppercase(char a) {
    return (a >= 'A' && a <= 'Z');
}

int is_apostrophe(char a) {
    return (a == '\'');
}

int is_dash(char a) {
    return (a == '-');
}

int is_valid_char(char a){
    return (is_lowercase(a)|| is_uppercase(a) || is_apostrophe(a));
}