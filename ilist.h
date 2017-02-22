/*
 * Name:    ilist.h
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    6. 10. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * ...
 */

#ifndef ILIST_H_INCLUDED
#define ILIST_H_INCLUDED

#include <stdlib.h>
#include "error.h"
#include "ial.h"
#define FCHUNCK 8

typedef enum {
    INS_ST,     /// (INS_ST, *node_data_t, , *node_data_t)
    INS_LAB,
    INS_ADD,    /// (INS_ADD,    *node_data_t, *node_data_t, *node_data_t)
    INS_SUB,    /// (INS_SUB,    *node_data_t, *node_data_t, *node_data_t)
    INS_DIV,    /// (INS_DIV,    *node_data_t, *node_data_t, *node_data_t)
    INS_MUL,    /// (INS_MUL,    *node_data_t, *node_data_t, *node_data_t)
    INS_LESS,   /// (INS_LESS,   *node_data_t, *node_data_t, *node_data_t)
    INS_GREAT,  /// (INS_GREAT,  *node_data_t, *node_data_t, *node_data_t)
    INS_ELESS,  /// (INS_ELESS,  *node_data_t, *node_data_t, *node_data_t)
    INS_EGREAT, /// (INS_EGREAT, *node_data_t, *node_data_t, *node_data_t)
    INS_EQUAL,  /// (INS_EQUAL,  *node_data_t, *node_data_t, *node_data_t)
    INS_NOT,    /// (INS_NOT,    *node_data_t, *node_data_t, *node_data_t)

    JMP,        /// (JMP,             , , *ilist_item_t)
    JMPZ,       /// (JMP, *node_data_t, , *ilist_item_t)

    PUSH_PARAM, /// (PUSH_PARAM, *node_data_t, , )

    BEGIN_FUNC, /// (BEGIN_FUNC, *node_data_t, , )          // TODO rename
    END_FUNC,   /// (END_FUNC, , , )
    CALL,       /// (CALL, *node_data_t, , *node_data_t)
    RET,        /// (RET,  *node_data_t, , )

    READ_INT,   /// (READ_INT, , , )
    READ_DOUBLE,/// (READ_DOUBLE, , , )
    READ_STRING,/// (READ_STRING, , , )
    PRINT,      /// (PRINT, , , )

    LENGTH,     /// (LENGHT, , , )
    SUBSTR,     /// (SUBSTR, , , )
    COMPARE,    /// (COMPARE, , , )
    FIND,       /// (FIND, , , )
    SORT,       /// (SORT, , , )
    
    EXIT        /// (EXIT, , , )
} ins_type_t;

typedef struct {
    ins_type_t type;
    void* op1;
    void* op2;
    void* result;
} instruction_t;

typedef struct item {
    instruction_t ins;
    struct item *next;
} ilist_item_t;

typedef struct {
    ilist_item_t* First;
    ilist_item_t* Last;
    ilist_item_t* Actual;
    ilist_item_t** Function;
    int cf;
} ilist_t;

void generate_while(ilist_t* L, void* op1);

void generate_if_else(ilist_t* L, void* op1);

void generate_instruction(ilist_t* L, ins_type_t type, void* op1, void* op2, void* result);
void generate_function(ilist_t* L, void* op1, void* space);
void generate_if_end(ilist_t* L);
void generate_else_end(ilist_t* L);
void generate_while_condition_end(ilist_t* L);
void generate_while_end(ilist_t* L);

void ilist_init(ilist_t* L);

int ilist_active(ilist_t* L);

void ilist_disactive(ilist_t* L);

int ilist_copy(ilist_t *L, instruction_t *val);

void ilist_add(ilist_t* L, ilist_item_t* new);

void ilist_succ (ilist_t *L);
void ilist_succ_generate(ilist_t *L);

void ilist_first(ilist_t *L);

void ilist_dispose_list(ilist_t *L);

void ilist_delete_first(ilist_t *L);
#endif
