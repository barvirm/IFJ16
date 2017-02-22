/*
 * Name: parser.h
 * Author: Marek Barvir
 * Email: barvirm@seznam.cz
 * Login: xbarvi00
 * Date: 25.9.2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: Parser
*/

#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED

#include "scanner.h"
#include "error.h"
#include "ial.h"
#include "postfix.h"
#include "ilist.h"
#include "interpret.h"
#include "sematic.h"
#include <stdbool.h>

/*
 * Starting point for syntax check
 * @return false error
 * @return true program without syntax error
*/
bool program();
/*
 * Check syntax in class
 * @return false error
 * @return true class without syntax error
*/
bool CLASS_BODY();
/*
 * Check parameters in function and save to local table
 * @param data table information
 * @param local_table table for local variable
 * @return false error
 * @return true param without syntax error
*/
bool PARAMS(node_data_t *data,node_t **local_table);
bool PARAMS_N(node_data_t *data,node_t **local_table);


/*
 * Check syntax in body of function,if,while add record to g. or l. table
 * @param local_table table for local variable
 * @param block i = if, w = while, f = function
 * @return false error
 * @return true body without syntax error
*/
bool FUNC_BODY(node_t **local_table,char block);

/*
 * Check variable type and add record to variable
 * @param data table information
 * @return false error
 * @return true parameters without error
*/
bool VAR_TYPE(node_data_t *data);
/*
 * Check condition and syntax
 * @param local_table table for local variable
 * @return false error
 * @return true while without error
*/
bool WHILE(node_t *local_table);
/*
 * Check type and add record to function, (global variable)
 * @param tmp table information
 * @return false error
 * @return true parameters without error
*/
bool TYPE(node_data_t *tmp);
/*
 * Check if s is buildin function
 * @param s full_id string
 * @return false its not buildin func
 * @return true its buildin func
*/
bool is_buildin_func(char *s);
/*
 * Check parameters add record to local table
 * @param local_table table for local variable
 * @param f information if its buildin function
 * @param func table information about function
 * @return false Error
 * @return true No error
*/
bool buildin_func(node_t **local_table, char f,node_data_t *func);
/*
 * Solve expresion add temporary variable to table
 * @param local_table table for local variable
 * @param result table record for save result
 * @param e true if expression can be empty
 * @return false Error in expression
 * @return true No problem
*/
bool skip_expresions(char e,node_t **local_table,node_data_t **result);
/*
 * Solve condition add temporary variable to table
 * @param local_table table for local variable
 * @param result table record for save result
 * @return false Error in condition
 * @return true No problem
*/
bool skip_condition(node_t **local_table,node_data_t **result);

/*
 * Create id record
 * @param s id
 * @return id
*/
char *create_id_record(char *s);
/*
 * Create full id record
 * @param class_id class id
 * @param simple_id simple id
 * @return full id (class_id.simple_id)
*/
char *create_full_id(char *class_id,char *simple_id);
/*
 * Add type to tmp
 * @param tmp table information
 * @param t type
 * @return NULL at error
*/
char *data_add_type(node_data_t *tmp, char t);

/*
 * Initialization table data
 * @param data table data
*/
void init_table_data(node_data_t *data);
/*
 * Initialization table data as const
 * @param type type of variable
 * @param data table data
*/
void init_const(node_data_t *data,char type);
/*
 * Initialization data in table find by id
 * @param table table
 * @param id id record
*/
void set_table_data_init(node_t **table,char *id);
/*
 * Save error token to error_token
*/
void save_error_token();

/*
 * Search record in table first in local table, second global table
 * @param local_table table
 * @param id id
 * @return NULL if record is not find
 * @return record
*/
node_data_t *search_local_global(node_t **local_table, char *id);

int parser( FILE *file_for_read, ilist_t *instruction, ilist_t *initialization);

#endif
