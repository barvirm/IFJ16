/* 
 * Name:	postfix.h
 * Author:	Jakub Sochůrek
 * Email:	xsochu01@stud.fit.vutbr.cz
 * Login:	xsochu01
 * Date:	10/10/2016
 * Team members:	Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 * 
 * Description:	File include  postfix notation 
*/



#ifndef POSTFIX_H_INCLUDED
#define POSTFIX_H_INCLUDED

#include <stdbool.h>
#include "scanner.h"
#include "token.h"
#include "buffer.h"
#include "ial.h"
#include "error.h"
#include "parser.h"
#include "ilist.h"
#include "gbc.h"

#define t_size 23

typedef struct zas {
  token_t *data;
  int lessthan;
  int isExpression;
  char *expressionId;
  struct zas *next;
} stackItem;

typedef struct {
  stackItem *First;
  stackItem *Top;
} stack_t;

typedef struct lItem {
    token_t data;
    struct lItem *previous;
    struct lItem *next;
} listItem;

typedef struct{
    listItem *First;
    listItem *Current;
    listItem *Last;
} list_t;


void stack_push_token(stack_t *, token_t *);
void stack_pop(stack_t *);
stackItem* stack_top(stack_t *);
void stack_init(stack_t *);
void stack_delete(stack_t *);

void list_init(list_t *);
void list_activate(list_t *);
void list_delete(list_t *);
int insert_copy_of_token_last(list_t *, token_t *);

int isRule(stack_t *);
int includeComparison(list_t *);

int processExpression(int, node_t **, node_data_t **);
int createPostfix(list_t *, int);
int create3AK(list_t *, node_t **, node_data_t **);

int function3AK(list_t *, node_t **, node_data_t **);

int checkFunctionSyntax(token_t *, stack_t *, list_t *);

#endif

