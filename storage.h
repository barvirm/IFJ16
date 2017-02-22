/*
 * Name:    storage.h
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    6. 10. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * ...
 */


#ifndef STORAGE_H_INCLUDED
#define STORAGE_H_INCLUDED

#include <stdlib.h>
#include <stdbool.h>
#include "error.h"
#include "ilist.h"		///< ilist_item_t
#include "token.h"

#define FRAME_CHUNK 8



#define tINTEGER 'i'
#define tDOUBLE 'd'
#define tSTRING 's'
#define tVOID 'v'

#define LOCAL 'l'
#define GLOBAL 'g'
#define CONST 'c'

#define tEMPTY 'e'

typedef struct {
    char type;
    data_t data;    ///< Data (ivar, dvar, svar)
} var_t;

/**
 * Frame struct
 */
typedef struct frame_t{
    int Count;
    var_t* Data;
    node_data_t* Result;
    ilist_item_t* Function;
    struct frame_t *next;
} frame_t;

/**
 *  Frame stack. Top frame is Local and bottom Global. Temp is temporaries frame for save parameters.
 */
typedef struct {
    frame_t* Local;
    frame_t* Temp;
} storage_t;

/**
 * Initialization storage, create Global and Temp frames.
 */
void storage_init(storage_t* s);

/**
 * Disponse storage.
 */
void storage_disponse(storage_t* s);

/**
 * Push Temp frame to top of stack.
 */
void storage_push_temp(storage_t* s);

void storage_push(storage_t* s, frame_t* frame);

/**
 * Pop top frame (Local).
 */
void storage_pop(storage_t* s);
    
/**
 * Return TRUE when is storage empty else TRUE.
 */
int storage_empty(storage_t* s);

/**
 * Allocate memory for frame.
 */
frame_t* frame_ctor();

/**
 * Free memory of frame.
 */
void frame_dtor(frame_t* f);

/**
 * Clear datas.
 */
void frame_clear(frame_t* f);

/**
 * Append data to end of datas array.
 */
int frame_append(frame_t* f, var_t v);

/**
 * Set pointer to result.
 */
void frame_set_result(frame_t* f, var_t* v);

/**
 * Set pointer to continue in program.
 */
void frame_set_function(frame_t* f, ilist_item_t* v);

/**
 * Reserve space in datas array.
 */
int frame_reserve(frame_t* f, int val);

/**
 * Return value from datas array.
 */
int frame_get(frame_t* f, int offset,  var_t* dest);

/**
 * Set value in datas array.
 */
int frame_set(frame_t* f, int offset, var_t val);

/**
 * Return value from local frame.
 */
int storage_get_local(storage_t* s, int offset,  var_t* dest);

/**
 * Set value from local frame.
 */
int storage_set_local(storage_t* s, int offset,  var_t val);

#endif
