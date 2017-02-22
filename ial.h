/* 
 * Name:	ial.h
 * Author:	David Smékal, Jakub Sochůrek, Marek Barvíř
 * Email:	xsmeka13@stud.fit.vutbr.cz, xsochu01@stud.fit.vutbr.cz, xbarvi00@stud.fit.vutbr.cz
 * Login:	xsmeka13, xsochu01, xbarvi00
 * Date:	28/9/2016
 * Team members:	Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 * 
 * Description:	File include source code for biuld-in functions that uses IAL algorithms and binary tree
*/

#ifndef IAL_H_INCLUDED
#define IAL_H_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "gbc.h"
#include "error.h"
#include "token.h"

#define STR_MAX 256

/* Definition of list element */
typedef struct elemList {
    struct elemList *nextElement;
    int index;
} *elemListPtr;

/* Definition of sublist element */
typedef struct subList {
    struct subList *nextList;
    elemListPtr thisList;
    elemListPtr last;
    int index;
} *subListPtr;

/* Definition of list */
typedef struct { 
    subListPtr Act;
    subListPtr First;
    subListPtr Last;
} mainList;

typedef struct node {
    char *id;
    struct data *data;          // TODO what is here ?
    struct node *left;
    struct node *right;
} node_t;

typedef struct data {
    char struct_type; // f (function) or c (const) or g (global) or l (local)
    int offset;
    bool inicializate;
    bool defined;
    data_t data_u;
    char *func_type;
    int type_size_act;
    int type_size_max;
} node_data_t;

int find(char *string, char *search);		// tasked

char *sort(char *string);					// tasked

int createLists(mainList *, int *);
void clearElemList(elemListPtr ptr);
void getElemList (mainList *L, elemListPtr *ptr);

void Error(char *);
void InitList (mainList *);
void DisposeList (mainList *);
void InsertFirst (mainList *, int);
void InsertLast (mainList *L, int);
void setFirstActive (mainList *);
void CopyFirst (mainList *, elemListPtr *);
void NextIsFirst (mainList *);
void PostDelete (mainList *);
void PostInsert (mainList *, char *);
int  Active (mainList *);

/*
 * Create new node
 * @param id Identificator (string)
 * @param data Structure data
 * @return Pointer to new node
 * @return NULL if malloc dont have enought memory
*/

void init_data(node_data_t *d);

/* 
 * Delete all tree
 * @param root Pointer to root of binary tree
*/
void bin_tree_dispose(node_t **root);

/*
 * Insert new node
 * @param root Pointer to root of binary tree
 * @param id Identificator (string)
 * @param data Structure data
 * @return Pointer to new node
 * @return NULL if already exist
*/
node_t *bin_tree_insert(node_t **root, char *id, node_data_t *data);
/*
 * Search note by identificator
 * @param root Pointer to root of binary tree
 * @param id Identificator (string)
 * @return Pointer to searched node
 * @return NULL if dont find any
*/
node_t *search_note(node_t **root,char *id);

/*
 * Search note by id a delete it
 * @param root Pointer to root of binary tree
 * @param id Identificator (string)
*/
void delete_note(node_t **root,char *id);

/*
 * Delete all pointers and struct in note
 * @param node Node ready for wipe all data in it
*/
void free_node(node_t *node);

bool bin_tree_trought(node_t **root,int (*fce)(node_t **root));

#endif