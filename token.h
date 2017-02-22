/*
 * Name:    token.h
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    11. 11. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * Token.
 */

#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "gbc.h"

/**
 * Types of token.
 */
typedef enum {
    EMPTY,              ///< Empty
    INTEGER,            ///< Integer
    DOUBLE,             ///< Double
    STRING,             ///< String
    SIMPLE_ID,          ///< Simple identifier
    FULLY_QUALIFIED_ID, ///< Fully qualified identifier

    ASIGN,              ///< =
    MUL,                ///< *
    DIV,                ///< /
    PLUS,               ///< +
    MINUS,              ///< -
    LESS,               ///< <
    ELESS,              ///< <=
    GREAT,              ///< >
    EGREAT,             ///< >=
    NOT,                ///< !=
    EQUAL,              ///< ==
    BRACKET_LEFT,       ///< (
    BRACKET_RIGHT,      ///< )
    BRACE_LEFT,         ///< {
    BRACE_RIGHT,        ///< }
    SEMI,               ///< ;
    COMMA,              ///< ,

    RES_BOOLEAN,        ///< boolean
    RES_BREAK,          ///< break
    RES_CLASS,          ///< class
    RES_CONTINUE,       ///< continue
    RES_DO,             ///< do
    RES_DOUBLE,         ///< double
    RES_ELSE,           ///< else
    RES_FALSE,          ///< false
    RES_FOR,            ///< for
    RES_IF,             ///< if
    RES_INT,            ///< int
    RES_RETURN,         ///< return
    RES_STRING,         ///< String
    RES_STATIC,         ///< static
    RES_TRUE,           ///< true
    RES_VOID,           ///< void
    RES_WHILE,          ///< while

    END_OF_FILE,        ///< End of file    
} type_t;

/**
 * Variable data in token.
 */
typedef union {
    char*    svar;
    int      ivar;
    double   dvar;
} data_t;

/**
 * Token.
 */
typedef struct {
    type_t type;    ///< Type
    data_t data;    ///< Data
    int line;
} token_t;


/**
 * Create and initialization new token.
 * 
 * @param Token Token
 */
 token_t* token_ctor();

/**
 * Clear and destroy token.
 * 
 * @param Token Token
 */
void token_dtor(token_t* t);

/**
 * Initialization token.
 * 
 * @param Token Token
 */
void init_token(token_t* Token);

/**
 * Frees allocated memory for data and set status to start.
 *
 * @param Token Token
 */
void clean_token(token_t* Token);

/**
 * Set type of token.
 */
void token_set_type(token_t* Token, type_t type);

/**
 * Set type to INTEGER, convert string and save to date.
 *
 * @param Token Token
 * @param str String to convert.
 */
int token_set_integer(token_t* Token, const char* str);

/**
 * Set type to DOUBLE, convert string and save to date.
 *
 * @param Token Token
 * @param str String to convert.
 */
int token_set_double(token_t* Token, const char* str);

/**
 * Set type to STRING and save to date.
 *
 * @param Token Token
 * @param str String to save.
 */
void token_set_string(token_t* Token, const char* str, type_t type);

#endif