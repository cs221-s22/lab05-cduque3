#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scan.h"

void scan_table_init(struct scan_table_st *tt) {
    // initialize the count to zero, like we did for the map in lab01
    tt->head = NULL;
    tt->len = 0;
}

struct scan_token_st *scan_table_new_token(struct scan_table_st *tt) {
   struct scan_token_st *tp = calloc(1, sizeof(struct scan_token_st)); //create memory for new token
    if (!tt->head) { //if table empty, make newly allocated token the head of the table
        tt->head = tp;
    } else {
        struct scan_token_st *walk = tt->head; //use to iterate through token table
        while (1) {
            if (!walk->next) { //once end of table reached, put newly allocated token at end
                walk->next = tp;
                break;
            } else { //keep looping until end of table reached
                walk = walk->next;
            }
        }
    }

    // increment the length of reserved tokens
    tt->len += 1; //increase token table length
    return tp; //return newly created token; remember, this token points to the end of the table
}

// scan_read_token is a helper function to generalize populating a token
char *scan_read_token(struct scan_token_st *tp, char *p,
    int len, enum scan_token_enum id) {

    // fill in the ID of the new token
    tp->id = id;

    // fill in the name of the new token for 'len' bytes
    int i;
    for (i = 0; i < len; i++) {
        tp->name[i] = *p;
        p += 1;
    }
    tp->name[i] = '\0'; // end of token marked as null
    return p;
}

bool scan_is_whitespace(char c) {
    return c == ' ' || c == '\t';
}

char *scan_whitespace(char *p, char *end) {
    while (scan_is_whitespace(*p) && p < end) {
        p += 1;
    }
    return p;
}

//check if value is digit
bool scan_is_digit(char c) {
    return c >= '0' && c <= '9';
}

//check if value is letter (for hex)
bool scan_is_letter(char c) {
    return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

//check if token is binary
bool scan_is_bin(char c, char d) {
    return c == '0' && d == 'b';
}

//check if token is hex
bool scan_is_hex(char c, char d) {
    return c == '0' && d == 'x';
}

//scan hex token
char *scan_hexlit(struct scan_token_st *tp, char *p) {

    int len = 0;
    char *new_p = p + 2; //points to value after '0x'
    char *next = p + 2; //use to iterate through input 
    while(true) { //check subsequent values to see when hex token ends
        if (!(scan_is_digit(*next)) && !(scan_is_letter(*next))) {
            break;
        }
        len++;
        next++;
    }
    p = scan_read_token(tp, new_p, len, TK_HEXLIT); //get hex token, omitting '0x'

    return p;
}

//scan bin token
char *scan_binlit(struct scan_token_st *tp, char *p) {

    int len = 0;
    char *new_p = p + 2; //points to value after '0b'
    char *next = p + 2; //use to iterate through input
    while(true) { //check subsequent values to see when binary token ends
        if ((*next != '0') && (*next != '1') ) {
            break;
        }
        len++;
        next++;
    }
    p = scan_read_token(tp, new_p, len, TK_BINLIT); //get binary token, omitting '0b'

    return p;
}

//scan integer token
char *scan_intlit(struct scan_token_st *tp, char *p) {

    int len = 1;
    char* next = p + 1; //use to iterate through input
    while(true) { //check subsequent values to see when integer token ends
        if (!(scan_is_digit(*next))) {
            break;
        }
        len++;
        next++;
    }
 
    p = scan_read_token(tp, p, len, TK_INTLIT); //get integer token

    return p;
}

char *scan_token(struct scan_token_st *tp, char *p, char *end) {

	char *next = p + 1; //use to check for hex and binary tokens
    
    if (p == end) {
        p = scan_read_token(tp, p, 0, TK_EOT);
    } else if (scan_is_whitespace(*p)) {
        // skip over the whitespace
        p = scan_whitespace(p, end);
        // recurse to get the next token
        p = scan_token(tp, p, end);
    } else if (scan_is_hex(*p, *next)) {
        p = scan_hexlit(tp, p);
    } else if (scan_is_bin(*p, *next)) {
        p = scan_binlit(tp, p);
    } else if (scan_is_digit(*p)) {
        p = scan_intlit(tp, p);
    } else if (*p == '+') {
        p = scan_read_token(tp, p, 1, TK_PLUS);
    } else if (*p == '-') {
        p = scan_read_token(tp, p, 1, TK_MINUS);
    } else if (*p == '*') {
        p = scan_read_token(tp, p, 1, TK_MULT);
    } else if (*p == '/') {
        p = scan_read_token(tp, p, 1, TK_DIV);
    } else if (*p == '(') {
        p = scan_read_token(tp, p, 1, TK_LPAREN);
    } else if (*p == ')') {
        p = scan_read_token(tp, p, 1, TK_RPAREN);
    } else {
    	printf("Invalid entry \n");
    	exit(0);
    }
    return p;
}

//p = argv[1]
void scan_table_scan(struct scan_table_st *tt, char *p) {
    //point to last character of given string, use to stop scanning once this value is reached
    char *end = p + strlen(p);

    // loop over the input text, getting one token each time through the loop
    while(true) {
        struct scan_token_st *tp = scan_table_new_token(tt); //create new token, put at end of table
        p = scan_token(tp, p, end); 
        if (tp->id == TK_EOT) {
            break;
        }
    }
}

void scan_table_print(struct scan_table_st *tt) {

    // define a variable containing the list of token ID names
    char *id_names[NUM_TOKENS] = SCAN_STRINGS;

    //point to head of table
    struct scan_token_st *tp = tt->head;
    while (tp) { //loop until end of token table
        // print the ID and name of that token
        printf("%s(\"%s\")\n", id_names[tp->id], tp->name);
        tp = tp->next; //move on to next token in token table
    }
}

int main(int argc, char **argv) {
    struct scan_table_st table;

    if (argc == 1) {
        printf("Usage: lab02 <expression>");
        return -1;
    }

    scan_table_init(&table);
    scan_table_scan(&table, argv[1]);
    scan_table_print(&table);

    return 0;
}
