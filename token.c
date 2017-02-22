/*
 * Name:    token.c
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    25. 9. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * Token.
 */

#include "token.h"

/*
 * Create and initialization new token.
 */
token_t* token_ctor() {
    token_t* t = (token_t*)GBC_malloc(sizeof(token_t));
    if (t == NULL) {
        return NULL;
    }
    init_token(t);
    return t;
}

/*
 * Clear and destroy token.
 */
void token_dtor(token_t* t) {
    clean_token(t);
    GBC_free(t);
}

/*
 * Initialization token.
 */
void init_token(token_t* Token) {
    Token->type      = EMPTY;
    Token->data.svar = NULL;
}

/*
 * Frees allocated memory for data and set status to start.
 */
void clean_token(token_t* Token) {
    if((Token->type == SIMPLE_ID || Token->type == FULLY_QUALIFIED_ID || 
        Token->type == STRING) && Token->data.svar != NULL) 
    {
        GBC_free(Token->data.svar);
    }

    Token->type = EMPTY;
    Token->data.svar = NULL;
}

/*
 * Set type of token.
 */
void token_set_type(token_t* Token, type_t type) {
    Token->type = type;
}

/*
 * Set type to INTEGER, convert string and save to date.
 */
int token_set_integer(token_t* Token, const char* str) {
    Token->type = INTEGER;
    sscanf(str, "%i", &(Token->data.ivar));
    if (errno == ERANGE) {
        clean_token(Token);
        return true;
    }
    return false;
}

/*
 * Set type to DOUBLE, convert string and save to date.
 */
int token_set_double(token_t* Token, const char* str) {
    Token->type = DOUBLE;
    sscanf(str, "%lg", &(Token->data.dvar));
    if (errno == ERANGE) {
        clean_token(Token);
        return true;
    }
    return false;
}

/*
 * Set type to STRING and save to date.
 */
void token_set_string(token_t* Token, const char* str, type_t type) {
    int len = strlen(str);
    Token->type = type;
    Token->data.svar = GBC_malloc(sizeof(char)*(len+1));
    strcpy(Token->data.svar, str);
    Token->data.svar[len] = '\0';
}