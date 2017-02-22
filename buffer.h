/*
 * Name:    buffer.h
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    25. 9. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * Buffer.
 */

#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gbc.h"
#include "error.h"

#define BUFFER_CHUNK 512

/**
 * Buffer. Size of buffer represent BUFFER_SIZE. 
 */
typedef struct{
    int allocated;
    int top;    ///< Counter 
    char* data;
} buffer_t;

/**
 * Initialization buffer.
 */
int init_buffer(buffer_t* buffer);

/**
 * Clear data buffer.
 */
void clean_buffer(buffer_t* buffer);

/**
 * Add char to buffer.
 */
int buffer_char_add(char c, buffer_t* buffer);

void buffer_free(buffer_t* buffer);

#endif