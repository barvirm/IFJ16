/*
 * Name:    buffer.c
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    25. 9. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * Buffer.
 */

#include "buffer.h"

/*
 * Clean buffer.
 */

void clean_buffer(buffer_t* buffer) {
    memset(buffer->data,'\0', buffer->allocated); 
    buffer->top = 0;
}

/*
 * Initialization buffer.
 */
int init_buffer(buffer_t* buffer) {
    buffer->top = 0;
    buffer->data = (char*)GBC_malloc(sizeof(char) * (BUFFER_CHUNK));
    if (buffer->data == NULL) {
        return INTERN_ERROR;
    }
    buffer->allocated = BUFFER_CHUNK;
    clean_buffer(buffer);
    return NO_ERROR;
}

/*
 * Add char to buffer. 
 */
int buffer_char_add(char c, buffer_t* buffer) {
    char* str;

    if(buffer->top == buffer->allocated-1)
    {
        str = (char*)GBC_realloc(buffer->data, sizeof(char) * (buffer->allocated + BUFFER_CHUNK));
        if( str == NULL ) {
            return INTERN_ERROR;
        }
        buffer->data = str;
        buffer->allocated += BUFFER_CHUNK;
    }
    buffer->data[buffer->top++] = c;
    return NO_ERROR;
}

void buffer_free(buffer_t* buffer) {
    if( buffer->data != NULL) {
        GBC_free(buffer->data);
    }
    buffer->data      = NULL;
    buffer->allocated = 0;
    buffer->top       = 0;
}