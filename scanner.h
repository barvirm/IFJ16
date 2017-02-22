/*
 * Name:    scanner.h
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    25. 9. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * ...
 */

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
 
#include "token.h"
#include "buffer.h"
#include "error.h"

/**
 * Initialization scanner.
 *
 * @param f The input file for reading lexeme 
 */
void init_scanner(FILE *f);

/**
 * Lexeme retrieves and converts it to a token.
 *
 * @param Token Token
 * @return On lexical error, function returns non zero value.
 */
int get_token(token_t* Token);

#endif